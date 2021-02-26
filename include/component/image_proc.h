#ifndef _IMAGE_PROC_H_
#define _IMAGE_PROC_H_

#include "attribute.h"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"

namespace ipo {
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

}  // namespace ipo
#endif  // _IMAGE_PROC_H_