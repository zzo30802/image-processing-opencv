#ifndef _CORRECTION_H_
#define _CORRECTION_H_
#include "common.h"
#include "image_proc.h"
namespace ipo {
class FlatFieldCorrection::PimplFlatFieldCorrection {
 public:
  PimplFlatFieldCorrection();
  ~PimplFlatFieldCorrection();
  int SetDarkAndBrightFieldImage(const cv::Mat &dark_field_img,
                                 const cv::Mat &bright_field_img,
                                 const int &mean_pixel_value);
  cv::Mat GetResult(const cv::Mat &src);

 private:
  int mean_pixel_value = 128;  //default
  int img_channel;
  cv::Mat gain;
  cv::Mat dark_field_img;
};
}  // namespace ipo
#endif  // _CORRECTION_H_