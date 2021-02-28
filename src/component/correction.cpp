#include "component/correction.h"
#define computervision_EXPORTS
#include "component/image_proc.h"
using namespace ipo;
//===========PIMPL (FlatFieldCorrection)===========
FlatFieldCorrection::FlatFieldCorrection() : p_pimplFlatFieldCorrection(new PimplFlatFieldCorrection) {}
FlatFieldCorrection::~FlatFieldCorrection() {}
int FlatFieldCorrection::SetDarkAndBrightFieldImage(const cv::Mat &dark_field_img,
                                                    const cv::Mat &bright_field_img,
                                                    const int &mean_pixel_value) {
  return p_pimplFlatFieldCorrection->SetDarkAndBrightFieldImage(dark_field_img, bright_field_img, mean_pixel_value);
}

cv::Mat FlatFieldCorrection::GetResult(const cv::Mat &src) {
  return p_pimplFlatFieldCorrection->GetResult(src);
}
//===========

FlatFieldCorrection::PimplFlatFieldCorrection::PimplFlatFieldCorrection() {}
FlatFieldCorrection::PimplFlatFieldCorrection::~PimplFlatFieldCorrection() {}
int FlatFieldCorrection::PimplFlatFieldCorrection::SetDarkAndBrightFieldImage(const cv::Mat &dark_field_img,
                                                                              const cv::Mat &bright_field_img,
                                                                              const int &mean_pixel_value) {
  // check input
  if (dark_field_img.empty()) {
    std::cout << "\nERROR: int ipo::FlatFieldCorrection::SetDarkAndBrightFieldImage --(cv::Mat)dark_field_img.empty()" << std::endl;
    return -1;
  } else if (bright_field_img.empty()) {
    std::cout << "\nERROR: int ipo::FlatFieldCorrection::SetDarkAndBrightFieldImage --(cv::Mat)bright_field_img.empty()" << std::endl;
    return -1;
  } else if (dark_field_img.channels() != bright_field_img.channels()) {
    std::cout << "\nERROR: ipo::FlatFieldCorrection::SetDarkAndBrightFieldImage" << std::endl;
    std::cout << "    --the number of channel between (cv::Mat)dark_field_img && (cv::Mat)bright_field_img channels() are different" << std::endl;
    return -1;
  }

  // print parameter
  if (mean_pixel_value < 0)
    this->mean_pixel_value = 0;
  else if (mean_pixel_value > 255)
    this->mean_pixel_value = 255;
  else
    this->mean_pixel_value = mean_pixel_value;
  std::cout << "userset : this->mean_pixel_value = " << this->mean_pixel_value << std::endl;

  // clone image
  this->dark_field_img = cv::Mat::zeros(dark_field_img.size(), dark_field_img.type());
  this->dark_field_img = dark_field_img.clone();
  cv::Mat &&bright_img = cv::Mat::zeros(bright_field_img.size(), bright_field_img.type());
  bright_img = bright_field_img.clone();

  // according to the channel to process separately
  const int channel = this->dark_field_img.channels();
  this->img_channel = channel;
  const int &width = dark_field_img.cols;
  const int &height = dark_field_img.rows;
  switch (channel) {
    case 1: {
      // allocate memory for gain matrix
      this->gain = cv::Mat::zeros(dark_field_img.size(), CV_64FC1);
      // m
      // const double &mean_value = cv::mean(bright_field_img - dark_field_img)[0];
      for (size_t j = 0; j < height; j++) {
        uchar *p_dark = this->dark_field_img.ptr<uchar>(j);
        uchar *p_bright = bright_img.ptr<uchar>(j);
        double *p_gain = this->gain.ptr<double>(j);
        for (size_t i = 0; i < width; i++) {
          p_gain[i] = static_cast<double>(this->mean_pixel_value / static_cast<double>(p_bright[i] - p_dark[i]));
        }
      }
      break;
    }
    case 3: {
      // allocate memory for gain matrix
      this->gain = cv::Mat::zeros(dark_field_img.size(), CV_64FC3);
      for (size_t j = 0; j < height; j++) {
        cv::Vec3b *p_dark = this->dark_field_img.ptr<cv::Vec3b>(j);
        cv::Vec3b *p_bright = bright_img.ptr<cv::Vec3b>(j);
        cv::Vec3d *p_gain = this->gain.ptr<cv::Vec3d>(j);
        for (size_t i = 0; i < width; i++) {
          for (size_t k = 0; k < 3; k++) {
            p_gain[i][k] = static_cast<double>(this->mean_pixel_value / static_cast<double>(p_bright[i][k] - p_dark[i][k]));
          }
        }
      }
      break;
    }
    default: {
      std::cout << "\nERROR: ipo::FlatFieldCorrection::SetDarkAndBrightFieldImage" << std::endl;
      std::cout << "    --(cv::Mat)channel must be equal to 1 || 3" << std::endl;
      return -1;
    }
  }
  return 0;
}

cv::Mat FlatFieldCorrection::PimplFlatFieldCorrection::GetResult(const cv::Mat &src) {
  // check input
  if (src.empty()) {
    std::cout << "\nERROR: cv::Mat ipo::FlatFieldCorrection::GetResult" << std::endl;
    std::cout << "    --(cv::Mat)src.empty()" << std::endl;
    return {};
  } else if (this->img_channel != src.channels()) {
    std::cout << "\nERROR: cv::Mat ipo::FlatFieldCorrection::GetResult" << std::endl;
    std::cout << "    --(cv::Mat)the channel is different" << std::endl;
    return {};
  } else if (this->gain.empty()) {
    std::cout << "\nERROR: cv::Mat ipo::FlatFieldCorrection::GetResult" << std::endl;
    std::cout << "    --(cv::Mat)this->gain.empty()" << std::endl;
    return {};
  }

  // clone image
  cv::Mat &&sample = cv::Mat::zeros(src.size(), src.type());
  sample = src.clone();

  cv::Mat &&dst = cv::Mat::zeros(src.size(), src.type());

  const int &width = src.cols;
  const int &height = src.rows;
  switch (this->img_channel) {
    case 1: {
      for (int j = 0; j < height; j++) {
        uchar *R = sample.ptr<uchar>(j);
        uchar *D = this->dark_field_img.ptr<uchar>(j);
        double *G = this->gain.ptr<double>(j);
        uchar *p_dst = dst.ptr<uchar>(j);
        for (int i = 0; i < width; i++) {
          double &&value = static_cast<double>(static_cast<double>(R[i] - D[i]) * G[i]);
          if (value < 0)
            value = 0;
          else if (value > 255)
            value = 255;
          p_dst[i] = static_cast<uchar>(value);
        }
      }
      break;
    }
    case 3: {
      for (int j = 0; j < height; j++) {
        cv::Vec3b *R = sample.ptr<cv::Vec3b>(j);
        cv::Vec3b *D = this->dark_field_img.ptr<cv::Vec3b>(j);
        cv::Vec3d *G = this->gain.ptr<cv::Vec3d>(j);
        cv::Vec3b *p_dst = dst.ptr<cv::Vec3b>(j);
        for (int i = 0; i < width; i++) {
          for (int k = 0; k < 3; k++) {
            double &&value = static_cast<double>(static_cast<double>(R[i][k] - D[i][k]) * G[i][k]);
            if (value < 0)
              value = 0;
            else if (value > 255)
              value = 255;
            p_dst[i][k] = static_cast<uchar>(value);
          }
        }
      }
      break;
    }
    default: {
      std::cout << "\nERROR: cv::Mat ipo::FlatFieldCorrection::GetResult" << std::endl;
      return {};
    }
  }
  return dst.clone();
}
