#include "component/image_proc.h"

#include "common.h"
#include "component/positioning.h"

namespace ipo {
int DynamicThreshold(const cv::Mat &src, cv::Mat &dst,
                     const int &blur_ksize, const int &offset,
                     const DynamicThresholdTypes &mode) {
  // CV_Assert(!src.empty());
  if (dst.empty()) {
    dst = cv::Mat::zeros(src.size(), CV_8UC1);
  }

  // convert to grayscale
  cv::Mat gray_src;
  if (ConvertMatToGrayscale(src, gray_src) != 0) {
    std::cout << "\nERROR: int ipo::DynamicThreshold" << std::endl;
    return -1;
  }

  // blur
  cv::Mat &&gray_mean = cv::Mat::zeros(gray_src.size(), CV_8UC1);
  cv::blur(gray_src, gray_mean, cv::Size(blur_ksize, blur_ksize));
  // for loop
  const int &w = src.rows;  // width
  const int &h = src.cols;  // height
  int &&pixel_intensity = 0;
  for (int j = 0; j < w; j++) {
    const uchar *p_gray_src = gray_src.ptr<uchar>(j);
    const uchar *p_gray_mean = gray_mean.ptr<uchar>(j);
    uchar *p_dst = dst.ptr<uchar>(j);
    for (int i = 0; i < h; i++) {
      switch (mode) {
        case DynamicThresholdTypes::DYNAMIC_THRES_LIGHT: {
          pixel_intensity = p_gray_src[i] - p_gray_mean[i];
          if (pixel_intensity >= offset)
            p_dst[i] = 255;
          else
            p_dst[i] = 0;
          break;
        }
        case DynamicThresholdTypes::DYNAMIC_THRES_DARK: {
          pixel_intensity = p_gray_mean[i] - p_gray_src[i];
          if (pixel_intensity >= offset)
            p_dst[i] = 255;
          else
            p_dst[i] = 0;
          break;
        }
        case DynamicThresholdTypes::DYNAMIC_THRES_LIGHT_AND_DARK_INRANGE: {
          pixel_intensity = p_gray_src[i] - p_gray_mean[i];
          if (pixel_intensity >= -offset && pixel_intensity <= offset)
            p_dst[i] = 255;
          else
            p_dst[i] = 0;
          break;
        }
        case DynamicThresholdTypes::DYNAMIC_THRES_LIGHT_OR_DARK_OUTRANGE: {
          pixel_intensity = p_gray_src[i] - p_gray_mean[i];
          if (pixel_intensity < -offset || pixel_intensity > offset)
            p_dst[i] = 255;
          else
            p_dst[i] = 0;
          break;
        }
        default: {
          std::cout << "\nERROR: ipo::DynamicThreshold (enum)DynamicThresholdTypes No matching enum " << std::endl;
          return -1;
        }
      }
    }
  }
  return 0;
}

int Stretching(const cv::Mat &src, cv::Mat &dst,
               const int &low_value, const int &high_value) {
  // check image
  if (src.empty()) {
    std::cout << "\nError: int Stretching --(cv::Mat)src.empty()" << std::endl;
    return -1;
  }

  const int &&channel = src.channels();
  switch (channel) {
    case 1: {
      // clone image
      if (dst.empty()) {
        dst = cv::Mat::zeros(src.size(), CV_8UC1);
      }
      dst = src.clone();

      int n_rows = src.rows;  // y
      int n_cols = src.cols;  // x
      if (src.isContinuous()) {
        std::cout << "isContinuous" << std::endl;
        n_cols = n_cols * n_rows;
        n_rows = 1;
      }
      uchar *p_data_mat;
      double scale = 0.0;
      if (high_value == low_value)
        scale = 100;
      else
        scale = static_cast<double>(256 / (high_value - low_value));

      for (int j = 0; j < n_rows; j++) {
        p_data_mat = dst.ptr<uchar>(j);
        for (int i = 0; i < n_cols; i++) {
          double value = 255;
          if (p_data_mat[i] >= high_value) {  // 如果pixel質比max_pixel_value大，則=255
            value = 255;
          }
          if (p_data_mat[i] <= low_value) {  // 如果pixel質比max_pixel_value大，則=0
            value = 0;
          }
          if (low_value < p_data_mat[i] && p_data_mat[i] < high_value) {
            // value = (scale * static_cast<double>(p_data_mat[i]));
            value = (p_data_mat[i] - low_value) * scale;
            // std::cout << value << std::endl;
            if (value > 255)
              value = 255;
            if (value < 0)
              value = 0;
          }
          p_data_mat[i] = static_cast<uchar>(value);
        }
      }
      return 0;
    }
    case 3: {
      // clone image
      if (dst.empty()) {
        dst = cv::Mat::zeros(src.size(), CV_8UC3);
      }
      dst = src.clone();

      int n_rows = src.rows;  // y
      int n_cols = src.cols;  // x
      if (src.isContinuous()) {
        std::cout << "isContinuous" << std::endl;
        n_cols = n_rows * n_cols;
        n_rows = 1;
      }

      uchar *p;
      double scale = 0.0;
      if (high_value == low_value)
        scale = 100;
      else
        scale = static_cast<double>(256 / (high_value - low_value));

      for (int j = 0; j < n_rows; ++j) {
        cv::Vec3b *ptr = dst.ptr<cv::Vec3b>(j);
        for (int i = 0; i < n_cols; ++i) {
          double value = 255;
          for (int v = 0; v < 3; v++) {
            if (ptr[i][v] >= high_value) {  // 如果pixel質比max_pixel_value大，則=255
              value = 255;
            }
            if (ptr[i][v] <= low_value) {  // 如果pixel質比max_pixel_value大，則=0
              value = 0;
            }
            if (low_value < ptr[i][v] && ptr[i][v] < high_value) {
              // value = (ptr[i][v] - low_value) * (scale * static_cast<double>(ptr[i][v]));
              value = (ptr[i][v] - low_value) * scale;
              // std::cout << value << std::endl;
              if (value > 255)
                value = 255;
              if (value < 0)
                value = 0;
            }
            ptr[i][v] = static_cast<uchar>(value);
          }
        }
      }
      return 0;
    }
    default: {
      std::cout << "\nERROR: int Stretching --(cv::Mat)src.channel must be equal to 1 or 3" << std::endl;
      return -1;
    }
  }
}

int Variance(const cv::Mat &src, cv::Mat &dst, const int &kernel_size) {
  // clone image
  if (src.empty()) {
    std::cout << "\nERROR: int Variance --(cv::Mat)src.empty()" << std::endl;
    return -1;
  } else if (dst.empty()) {
    dst = cv::Mat::zeros(src.size(), CV_8UC1);
  }

  // convert to grayscale
  cv::Mat gray_src;
  if (ConvertMatToGrayscale(src, gray_src) != 0) {
    std::cout << "\nERROR: int ipo::Variance" << std::endl;
    return -1;
  }
  int &w = gray_src.cols;
  int &h = gray_src.rows;
  // if (gray_src.isContinuous() && dst.isContinuous()) {
  //   std::cout << "(cv::Mat)src, dst isContinuous()" << std::endl;
  // }
  const int &&padding = kernel_size / 2;
  const int &&pad_sift = padding * 2;
  const int &&n = kernel_size * kernel_size;
  cv::Mat &&temp = cv::Mat(kernel_size, kernel_size, 0);
  for (int j = 0; j < h; j++) {
    const uchar *p_gray_src = gray_src.ptr<uchar>(j);
    uchar *p_dst = dst.ptr<uchar>(j);
    for (int i = 0; i < w; i++) {
      if (j >= (h - pad_sift) && i < (w - pad_sift))
        temp = gray_src(cv::Rect(i, h - kernel_size, kernel_size, kernel_size));
      else if (i >= (w - pad_sift) && j < (h - pad_sift))
        temp = gray_src(cv::Rect(w - kernel_size, j, kernel_size, kernel_size));
      else if (j >= (h - pad_sift) && i >= (w - pad_sift)) {
        temp = gray_src(cv::Rect(w - kernel_size, h - kernel_size, kernel_size, kernel_size));
      } else {
        temp = gray_src(cv::Rect(i, j, kernel_size, kernel_size));
      }
      const double &mean = cv::mean(temp)[0];  // 取得temp的像素平均值
      cv::Scalar mean_, dev_;
      cv::meanStdDev(temp, mean_, dev_);
      double &&value = std::pow(dev_[0], 2);
      // std::cout << "value : " << value << std::endl;
      if (value > 255)
        value = 255;
      else if (value < 0)
        value = 0;
      p_dst[i] = static_cast<int>(value);
    }
  }
  return 0;
}

int FindTheSpecifiedColorByRGB(const cv::Mat &src, cv::Mat &dst,
                               const int &r, const int &g, const int &b, const double &tolerance) {
  // check input
  if (src.type() != CV_8UC3) {
    std::cout << "\nERROR: int FindTheSpecifiedColorByRGB --(cv::Mat)src.type() != CV_8UC3" << std::endl;
    return -1;
  }
  std::cout << "---FindTheSpecifiedColorByRGB UserSet---" << std::endl;
  std::cout << "r : " << r << std::endl;
  std::cout << "g : " << g << std::endl;
  std::cout << "b : " << b << std::endl;
  std::cout << "tolerance : " << tolerance << std::endl;
  std::cout << "----------------------------------------" << std::endl;
  int s_thres[2] = {100, 255};
  int v_thres[2] = {0, 255};

  // convert src to dst
  cv::Mat &&hsv_s = cv::Mat::zeros(src.size(), CV_8UC3);  // channel = 3
  cv::cvtColor(src, hsv_s, CV_BGR2HSV);
  cv::Mat &&selected_color = cv::Mat::zeros(src.size(), CV_8UC3);  // channel = 1
  // cv::Mat &&bitwise_and = cv::Mat::zeros(src.size(), CV_8UC1);  // overlap region

  // convert b,g,r to h,s,v
  cv::Mat &&bgr = cv::Mat(1, 1, CV_8UC3);
  bgr.at<cv::Vec3b>(0, 0)[0] = b;
  bgr.at<cv::Vec3b>(0, 0)[1] = g;
  bgr.at<cv::Vec3b>(0, 0)[2] = r;
  cv::Mat &&bgr2hsv = cv::Mat(1, 1, CV_8UC3);
  cv::cvtColor(bgr, bgr2hsv, CV_BGR2HSV);
  int &&lower_h = static_cast<int>(bgr2hsv.at<cv::Vec3b>(0, 0)[0]) - cvRound(90 * tolerance);
  int &&lower_s = static_cast<int>(bgr2hsv.at<cv::Vec3b>(0, 0)[1]) - cvRound(128 * tolerance);
  int &&lower_v = static_cast<int>(bgr2hsv.at<cv::Vec3b>(0, 0)[2]) - cvRound(128 * tolerance);
  int &&upper_h = static_cast<int>(bgr2hsv.at<cv::Vec3b>(0, 0)[0]) + cvRound(90 * tolerance);
  int &&upper_s = static_cast<int>(bgr2hsv.at<cv::Vec3b>(0, 0)[1]) + cvRound(128 * tolerance);
  int &&upper_v = static_cast<int>(bgr2hsv.at<cv::Vec3b>(0, 0)[2]) + cvRound(128 * tolerance);
  /*
  h: 0~180  if h = -1 -> h =179 
  s: 0~255
  v: 0~255
  */
  cv::Scalar lower, upper;
  if (lower_h < 0 || lower_s < 0 || lower_v < 0 || upper_h > 255 || upper_s > 255 || upper_v > 255) {
    if (lower_s < 0) {
      lower_s = 0;
    }
    if (lower_v < 0) {
      lower_v = 0;
    }
    if (upper_s > 255) {
      upper_s = 255;
    }
    if (upper_v > 255) {
      upper_v = 255;
    }
    if (lower_h < 0) {
      lower_h = lower_h + 180;
    }
    if (upper_h > 179) {
      upper_h = upper_h - 180;
    }
    // fix the h & s
    s_thres[0] = lower_s;
    s_thres[1] = upper_s;
    v_thres[0] = lower_v;
    v_thres[1] = upper_v;
    // 0~lower upper~255
    cv::Mat mask1_s = cv::Mat::zeros(src.size(), src.type());
    cv::Mat mask2_s = cv::Mat::zeros(src.size(), src.type());
    cv::Mat mask1_gs = cv::Mat::zeros(src.size(), src.type());
    cv::Mat mask2_gs = cv::Mat::zeros(src.size(), src.type());
    // get mask1
    cv::inRange(hsv_s, cv::Scalar(0, s_thres[0], v_thres[0]), cv::Scalar(upper_h, s_thres[1], v_thres[1]), mask1_s);
    // get mask2
    cv::inRange(hsv_s, cv::Scalar(lower_h, s_thres[0], v_thres[0]), cv::Scalar(255, s_thres[1], v_thres[1]), mask2_s);
    // get the union of mask1 & mask2
    cv::bitwise_or(mask1_s, mask2_s, selected_color);
  } else {  // 沒有超過上下限的範圍
    s_thres[0] = lower_s;
    s_thres[1] = upper_s;
    v_thres[0] = lower_v;
    v_thres[1] = upper_v;
    lower = cv::Scalar(lower_h, s_thres[0], v_thres[0]);
    upper = cv::Scalar(upper_h, s_thres[1], v_thres[1]);
    cv::inRange(hsv_s, lower, upper, selected_color);
  }
  dst = cv::Mat::zeros(selected_color.size(), selected_color.type());
  dst = selected_color.clone();
  return 0;
}

cv::Point TwoLineIntersection(const cv::Point &x1_start, const cv::Point &x1_end,
                              const cv::Point &x2_start, const cv::Point &x2_end) {
  // line1 : a1x + b1y = c1
  const cv::Point pt1_start{x1_start};
  const cv::Point pt1_end{x1_end};
  const double &&a1 = pt1_end.y - pt1_start.y;
  const double &&b1 = pt1_end.x - pt1_start.x;
  const double &&c1 = pt1_start.x * (pt1_end.y - pt1_start.y) - pt1_start.y * (pt1_end.x - pt1_start.x);

  // line 2 : a2x + b2y = c2
  const cv::Point pt2_start{x2_start};
  const cv::Point pt2_end{x2_end};
  const double &&a2 = pt2_end.y - pt2_start.y;
  const double &&b2 = pt2_end.x - pt2_start.x;
  const double &&c2 = pt2_start.x * (pt2_end.y - pt2_start.y) - pt2_start.y * (pt2_end.x - pt2_start.x);

  // 行列式
  // double dalta = this->Det(a1, b1, a2, b2);
  const double &&dalta = a1 * b2 - b1 * a2;
  const double &&dalta_x = c1 * b2 - b1 * c2;
  const double &&dalta_y = a1 * c2 - c1 * a2;
  const int &&x = static_cast<int>(dalta_x / dalta);
  const int &&y = static_cast<int>(-dalta_y / dalta);

  const cv::Point &&intersection = cv::Point(x, y);
  return intersection;
}

double GetTwoPointAngle(const cv::Point &pt0, const cv::Point &pt1) {
  const cv::Point &&diff_pt = cv::Point((pt1.x - pt0.x), (pt1.y - pt0.y));
  /*
    → x
  ↓
  y     |
        90
        |
  -180-----0---
        |
       270
        |
  */
  // 0, 90, 180, 270
  if ((diff_pt.x == 0) && (diff_pt.y == 0)) {
    return 0;
  } else if ((diff_pt.x == 0) && (diff_pt.y < 0)) {
    return 90.0;
  } else if ((diff_pt.x == 0) && (diff_pt.y > 0)) {
    return 270.0;
  } else if ((diff_pt.x < 0) && (diff_pt.y == 0)) {
    return 180.0;
  } else if ((diff_pt.x > 0) && (diff_pt.y == 0)) {
    return 0;
  }
  double &&angle = 0;
  double &&temp = 0;
  temp = std::fabsf(float(diff_pt.y) / float(diff_pt.x));
  temp = std::atan(temp);
  temp = temp * 180 / CV_PI;
  if ((0 < diff_pt.x) && (0 < diff_pt.y)) {
    angle = 360 - temp;
    return angle;
  }
  if ((0 > diff_pt.x) && (0 < diff_pt.y)) {
    angle = 360 - (180 - temp);
    return angle;
  }
  if ((0 < diff_pt.x) && (0 > diff_pt.y)) {
    angle = temp;
    return angle;
  }
  if ((0 > diff_pt.x) && (0 > diff_pt.y)) {
    angle = 180 - temp;
    return angle;
  }
  std::cout << "\nERROR: double GetTwoPointAngle" << std::endl;
  return -1;
}

int GetNewRotatedImageSize(const cv::Mat &src,
                           const double &angle, int &width, int &height) {
  // check input
  if (src.empty()) {
    std::cout << "int GetNewRotatedImageSize --(cv::Mat)src.empty()" << std::endl;
    return -1;
  }
  const int &width_ = src.cols;
  const int &height_ = src.rows;
  const cv::Point &&image_center = cv::Point(width_ / 2, height_ / 2);
  const cv::Mat &&rotation_mat = cv::getRotationMatrix2D(image_center, -angle, 1.0);
  const double &&abs_cos = std::abs(rotation_mat.at<double>(0, 0));
  const double &&abs_sin = std::abs(rotation_mat.at<double>(0, 1));

  width = static_cast<int>(height_ * abs_sin + width_ * abs_cos);
  height = static_cast<int>(height_ * abs_cos + width_ * abs_sin);
  return 0;
}

cv::Mat ImageRotateByCenterAndAdjustBoundary(const cv::Mat &src, const double &angle) {
  // check input images
  if (src.empty()) {
    std::cout << "\nError: ipo::ImageRotateByCenterAndAdjustBoundary() --(cv::Mat)src.empty" << std::endl;
    return {};
  }

  // std::cout << "angle : " << angle << std::endl;
  const int &width = src.cols;
  const int &height = src.rows;

  cv::Point &&image_center = cv::Point(width / 2, height / 2);
  cv::Mat &&rotation_mat = cv::getRotationMatrix2D(image_center, -angle, 1.0);

  double &&abs_cos = std::abs(rotation_mat.at<double>(0, 0));
  double &&abs_sin = std::abs(rotation_mat.at<double>(0, 1));

  const int &&bound_w = static_cast<int>(height * abs_sin + width * abs_cos);
  const int &&bound_h = static_cast<int>(height * abs_cos + width * abs_sin);

  rotation_mat.at<double>(0, 2) += bound_w / 2 - image_center.x;
  rotation_mat.at<double>(1, 2) += bound_h / 2 - image_center.y;

  cv::Mat &&dst = cv::Mat::zeros(cv::Size(bound_w, bound_h), src.type());
  cv::warpAffine(src, dst, rotation_mat, cv::Size(bound_w, bound_h), cv::INTER_CUBIC);
  return dst.clone();
}

cv::Mat ImageRotate(const cv::Mat &src, const double &angle, const cv::Point &center) {
  // check input images
  if (src.empty()) {
    std::cout << "\nError: ImageRotate() --(cv::Mat)src.empty()" << std::endl;
    return {};
  }
  cv::Mat &&dst = cv::Mat::zeros(src.size(), src.type());
  cv::Mat &&rotate_matrix = cv::getRotationMatrix2D(center, -angle, 1.0);
  cv::warpAffine(src, dst, rotate_matrix, src.size(), cv::INTER_CUBIC);
  return dst.clone();
}

cv::Mat ImageShift(const cv::Mat &src, const cv::Point2f &from_pt, const cv::Point2f &to_pt) {
  // check input images
  if (src.empty()) {
    std::cout << "\nError: ImageRotate() --src is empty" << std::endl;
    return {};
  }
  cv::Mat &&dst = cv::Mat::zeros(src.size(), src.type());
  const double &&dx = to_pt.x - from_pt.x;
  const double &&dy = to_pt.y - from_pt.y;
  std::cout << "dx : " << dx << std::endl;
  std::cout << "dy : " << dy << std::endl;
  cv::Mat shift_mat = (cv::Mat_<double>(2, 3) << 1, 0, dx, 0, 1, dy);
  cv::warpAffine(src, dst, shift_mat, dst.size());
  return dst.clone();
}

//====PositioningCreator====

}  // namespace ipo