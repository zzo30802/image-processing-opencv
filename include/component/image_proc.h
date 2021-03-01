#ifndef _IMAGE_PROC_H_
#define _IMAGE_PROC_H_

#ifdef computervision_EXPORTS
#define IPO_API __declspec(dllexport)
#else
#define IPO_API __declspec(dllimport)
#endif

#include "attribute.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

namespace ipo {

#ifdef _WIN32
/**
 * @brief Calculate the diff value between subtraction of grayscale image and fuzzy image. If the value exceed the offset value, then it equal to 255, and vice versa.
 * @param src input image.
 * @param dst output image.
 * @param blur_ksize blurring kernel size.
 * @param offset diff value.
 * @param mode dynamic threshold mode.
**/
IPO_API int DynamicThreshold(const cv::Mat &src, cv::Mat &dst,
                             const int &blur_ksize, const int &offset,
                             const DynamicThresholdTypes &mode);

/**
 * @brief Refer to the B&C function of imageJ to stretch the pixel value
 *  between low_value and high_value. If it exceeds high_value, it is 255, and it
 *  is 0 if it is less than low_value.
 * @param src input image.
 * @param dst output image.
 * @param low_value Low threshold.
 * @param high_value High threshold.
**/
IPO_API int Stretching(const cv::Mat &src, cv::Mat &dst,
                       const int &low_value, const int &high_value);
IPO_API int Variance(const cv::Mat &src, cv::Mat &dst, const int &kernel_size);
IPO_API int FindTheSpecifiedColorByRGB(const cv::Mat &src, cv::Mat &dst,
                                       const int &r, const int &g, const int &b, const double &tolerance);
IPO_API cv::Point TwoLineIntersection(const cv::Point &x1_start, const cv::Point &x1_end,
                                      const cv::Point &x2_start, const cv::Point &x2_end);
IPO_API double GetTwoPointAngle(const cv::Point &pt0, const cv::Point &pt1);
IPO_API int GetNewRotatedImageSize(const cv::Mat &src, const double &angle, int &width, int &height);
IPO_API cv::Mat ImageRotateByCenterAndAdjustBoundary(const cv::Mat &src, const double &angle);
IPO_API cv::Mat ImageRotate(const cv::Mat &src, const double &angle, const cv::Point &center);
IPO_API cv::Mat ImageShift(const cv::Mat &src, const cv::Point2f &from_pt, const cv::Point2f &to_pt);

//====Positioning (PIMPL)====
class Positioning {
 public:
  IPO_API Positioning(const PositioningTypeEnums &type);
  IPO_API ~Positioning();
  IPO_API int SetGoldenSampleImage(const cv::Mat &golden_sample_img);
  IPO_API int SetRect(const PositioningRectEnums &rect_type, const cv::Rect &rect);
  IPO_API int SetAttribute(const int &attribute_type, const double &value);
  IPO_API cv::Mat GetResult(const cv::Mat &sample_img);

 private:
  class PimplPositioning;
  std::auto_ptr<PimplPositioning> p_pimplPositioning;
};

//====Flat-field correction====
class FlatFieldCorrection {
 public:
  IPO_API FlatFieldCorrection();
  IPO_API ~FlatFieldCorrection();
  IPO_API int SetDarkAndBrightFieldImage(const cv::Mat &dark_field_img,
                                         const cv::Mat &bright_field_img,
                                         const int &mean_pixel_value);
  IPO_API cv::Mat GetResult(const cv::Mat &src);

 private:
  class PimplFlatFieldCorrection;
  std::auto_ptr<PimplFlatFieldCorrection> p_pimplFlatFieldCorrection;
};
#else  // Linux
/**
 * @brief Calculate the diff value between subtraction of grayscale image and fuzzy image. If the value exceed the offset value, then it equal to 255, and vice versa.
 * @param src input image.
 * @param dst output image.
 * @param blur_ksize blurring kernel size.
 * @param offset diff value.
 * @param mode dynamic threshold mode.
**/
int DynamicThreshold(const cv::Mat &src, cv::Mat &dst,
                     const int &blur_ksize, const int &offset,
                     const DynamicThresholdTypes &mode);

/**
 * @brief Refer to the B&C function of imageJ to stretch the pixel value
 *  between low_value and high_value. If it exceeds high_value, it is 255, and it
 *  is 0 if it is less than low_value.
 * @param src input image.
 * @param dst output image.
 * @param low_value Low threshold.
 * @param high_value High threshold.
**/
int Stretching(const cv::Mat &src, cv::Mat &dst,
               const int &low_value, const int &high_value);

/**
 * 
**/
int Variance(const cv::Mat &src, cv::Mat &dst, const int &kernel_size);
int FindTheSpecifiedColorByRGB(const cv::Mat &src, cv::Mat &dst,
                               const int &r, const int &g, const int &b, const double &tolerance);
cv::Point TwoLineIntersection(const cv::Point &x1_start, const cv::Point &x1_end,
                              const cv::Point &x2_start, const cv::Point &x2_end);
double GetTwoPointAngle(const cv::Point &pt0, const cv::Point &pt1);
int GetNewRotatedImageSize(const cv::Mat &src, const double &angle, int &width, int &height);
cv::Mat ImageRotateByCenterAndAdjustBoundary(const cv::Mat &src, const double &angle);
cv::Mat ImageRotate(const cv::Mat &src, const double &angle, const cv::Point &center);
cv::Mat ImageShift(const cv::Mat &src, const cv::Point2f &from_pt, const cv::Point2f &to_pt);

//====Positioning (PIMPL)====
class Positioning {
 public:
  Positioning(const PositioningTypeEnums &type);
  ~Positioning();
  int SetGoldenSampleImage(const cv::Mat &golden_sample_img);
  int SetRect(const PositioningRectEnums &rect_type, const cv::Rect &rect);
  int SetAttribute(const int &attribute_type, const double &value);
  cv::Mat GetResult(const cv::Mat &sample_img);

 private:
  class PimplPositioning;
  std::auto_ptr<PimplPositioning> p_pimplPositioning;
};

//====Flat-field correction====
class FlatFieldCorrection {
 public:
  FlatFieldCorrection();
  ~FlatFieldCorrection();
  int SetDarkAndBrightFieldImage(const cv::Mat &dark_field_img,
                                 const cv::Mat &bright_field_img,
                                 const int &mean_pixel_value);
  cv::Mat GetResult(const cv::Mat &src);

 private:
  class PimplFlatFieldCorrection;
  std::auto_ptr<PimplFlatFieldCorrection> p_pimplFlatFieldCorrection;
};
#endif

}  // namespace ipo
#endif  // _IMAGE_PROC_H_