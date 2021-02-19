#include "component/image_proc.h"
namespace ipo {
int DynamicThreshold(const cv::Mat &src, cv::Mat &dst,
                     const int &blur_ksize, const int &offset,
                     const DynamicThresholdTypes &mode) {
  CV_Assert(!src.empty());
  if (dst.empty()) {
    dst = cv::Mat::zeros(src.size(), src.type());
  }

  // convert to grayscale
  cv::Mat gray_src;
  if (ConvertMatToGrayscale(src, gray_src) != 0) {
    std::cout << "\nERROR: int ipo::DynamicThreshold" << std::endl;
    return -1;
  }
  // const int &&channel = src.channels();
  // cv::Mat &&gray_src = cv::Mat::zeros(src.size(), CV_8UC1);
  // switch (channel) {
  //   case 1: {
  //     gray_src = src.clone();
  //     break;
  //   }
  //   case 3: {
  //     cv::cvtColor(src, gray_src, cv::COLOR_BGR2GRAY, 1);
  //     break;
  //   }
  //   default: {
  //     std::cout << "\nERROR: ipo::DynamicThreshold (cv::Mat)src.channels != 1 || 3" << std::endl;
  //     return -1;
  //   }
  // }

  // blur
  cv::Mat &&gray_mean = cv::Mat::zeros(src.size(), CV_8UC1);
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
      cv::Mat &&dst = cv::Mat::zeros(src.size(), CV_8UC1);
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
      cv::Mat &&dst = cv::Mat::zeros(src.size(), CV_8UC3);
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
  cv::Mat mask = cv::Mat::zeros(1000, 1000, CV_8UC1);
  cv::line(mask, pt1_start, pt1_end, uchar(255), 5);
  cv::line(mask, pt2_start, pt2_end, uchar(128), 5);
  cv::circle(mask, intersection, 50, uchar(50), -1);
  return intersection;
}
}  // namespace ipo