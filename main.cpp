#include <iostream>

#include "component/image_proc.h"
// #ifdef _WIN32
// #include "client/windows/handler/exception_handler.h"
// #endif

// #ifdef _WIN32
// bool callback(const wchar_t *dump_path, const wchar_t *id,
//               void *context, EXCEPTION_POINTERS *exinfo,
//               MDRawAssertionInfo *assertion, bool succeeded) {
//   if (succeeded)
//     std::cout << "Create dump file success" << std::endl;
//   else
//     std::cout << "Create dump file failed" << std::endl;
//   return succeeded;
// }
// #endif

// ---load image---
cv::Mat &&src = cv::imread("../images/Okonomiyaki.png");
/*
index:
------image_proc------
1 : DynamicThreshold()
2 : Stretching()
3 : Variance()
4 : FindTheSpecifiedColorByRGB()
5 : TwoLineIntersection()
6 : GetTwoPointAngle()
7 : ImageRotateByCenterAndAdjustBoundary()
------positioning------
8 : positioning by feature matching(SURF)
9 : positioning by template matching
------Flate-Field-Correction------
10 : FlatFieldCorrection
------DumpFile------
11 : build dump file (windows)
*/
#define index 10

#if index == 1
// DynamicThreshold
int main() {
  cv::Mat dst;
  if (ipo::DynamicThreshold(src, dst, 21, 50, ipo::DynamicThresholdTypes::DYNAMIC_THRES_LIGHT) != 0)
    return -1;
  cv::imshow("dst", dst);
  cv::waitKey(0);
  return 0;
}
#elif index == 2
// Stretching
int main() {
  cv::Mat dst;
  if (ipo::Stretching(src, dst, 100, 200) != 0)
    return -1;
  cv::imshow("dst", dst);
  cv::waitKey(0);
  return 0;
}
#elif index == 3
// Variance
int main() {
  cv::Mat dst;
  if (ipo::Variance(src, dst, 10) != 0)
    return -1;
  cv::imshow("dst", dst);
  cv::waitKey(0);
  return 0;
}
#elif index == 4
// FindTheSpecifiedColorByRGB
int main() {
  cv::Mat dst;
  if (ipo::FindTheSpecifiedColorByRGB(src, dst, 191, 119, 074, 0.4) != 0)
    return -1;
  cv::imshow("dst", dst);
  cv::waitKey(0);
  return 0;
}
#elif index == 5
// TwoLineIntersection
int main() {
  cv::Mat &&canvas = cv::Mat::zeros(cv::Size(500, 500), CV_8UC1);
  // two lines
  const cv::Point &&A1 = cv::Point(0, 0);
  const cv::Point &&A2 = cv::Point(400, 500);
  const cv::Point &&B1 = cv::Point(100, 500);
  const cv::Point &&B2 = cv::Point(400, 100);
  cv::line(canvas, A1, A2, uchar(120), 3);
  cv::line(canvas, B1, B2, uchar(120), 3);
  // The intersection of two lines
  cv::Point &&pt_of_intersection = ipo::TwoLineIntersection(A1, A2, B1, B2);
  cv::circle(canvas, pt_of_intersection, 10, uchar(255), -1);
  cv::imshow("canvas", canvas);
  cv::waitKey(0);
  return 0;
}
#elif index == 6
// GetTwoPointAngle
int main() {
  cv::Point &&A1 = cv::Point(0, 0);
  cv::Point &&A2 = cv::Point(100, 100);
  double &&angle = ipo::GetTwoPointAngle(A1, A2);
  std::cout << "angle: " << angle << std::endl;
  return 0;
}
#elif index == 7
// GetNewRotatedImageSize()
int main() {
  cv::resize(src, src, cv::Size(), 0.6, 0.6);

  const double &&angle = 30;
  // int width, height;
  // ipo::GetNewRotatedImageSize(src, angle, width, height);
  cv::Mat &&dst = cv::Mat::zeros(src.size(), src.type());
  dst = ipo::ImageRotateByCenterAndAdjustBoundary(src, angle);
  cv::imshow("dst", dst);
  cv::waitKey(0);
  return 0;
}
#elif index == 8
// positioning by feature matching(SURF)
int main() {
  //----Load images----
  cv::Mat &&golden_sample = cv::imread("../images/positioning/golden_sample.jpg");
  cv::Mat &&sample = cv::imread("../images/positioning/sample.jpg");
  //----cropped rect----
  cv::Rect &&searching_rect = cv::Rect(126, 95, 630, 560);
  cv::Rect &&template_rect = cv::Rect(313, 231, 240, 206);

  std::shared_ptr<ipo::Positioning> &&position_obj = std::make_shared<ipo::Positioning>(ipo::PositioningTypeEnums::FEATURE_MATCHING);
  if (position_obj->SetGoldenSampleImage(golden_sample) != 0)
    return -1;
  if (position_obj->SetRect(ipo::PositioningRectEnums::SEARCHING_IMG_RECT, searching_rect) != 0)
    return -1;
  if (position_obj->SetRect(ipo::PositioningRectEnums::TEMPLATE_IMG_RECT, template_rect) != 0)
    return -1;
  if (position_obj->SetAttribute(ipo::FeatureAttributeEnums::HESSIAN_THRESHOLD, 100.0f) != 0)
    return -1;
  if (position_obj->SetAttribute(ipo::FeatureAttributeEnums::LOWE_RATIO, 0.9f) != 0)
    return -1;
  cv::Mat &&dst = position_obj->GetResult(sample);
  if (dst.empty())
    return -1;
  // cv::imwrite("../images/result/positioning/positioning_feature.jpg", dst);
  cv::imshow("dst", dst);
  cv::waitKey(0);
}
#elif index == 9
// positioning by template matching
int main() {
  //----Load images----
  cv::Mat &&golden_sample = cv::imread("../images/positioning/golden_sample.jpg");
  cv::Mat &&sample = cv::imread("../images/positioning/sample.jpg");
  //----cropped rect----
  cv::Rect &&searching_rect = cv::Rect(126, 95, 630, 560);
  cv::Rect &&template_rect = cv::Rect(313, 231, 240, 206);

  std::shared_ptr<ipo::Positioning> &&position_obj = std::make_shared<ipo::Positioning>(ipo::PositioningTypeEnums::TEMPLATE_MATCHING);
  if (position_obj->SetGoldenSampleImage(golden_sample) != 0)
    return -1;
  if (position_obj->SetRect(ipo::PositioningRectEnums::SEARCHING_IMG_RECT, searching_rect) != 0)
    return -1;
  if (position_obj->SetRect(ipo::PositioningRectEnums::TEMPLATE_IMG_RECT, template_rect) != 0)
    return -1;
  if (position_obj->SetAttribute(ipo::TemplateAttributeEnums::ANGLE_TOLERANCE, 20) != 0)
    return -1;
  if (position_obj->SetAttribute(ipo::TemplateAttributeEnums::NUMBER_OF_LEVELS, 1) != 0)
    return -1;
  if (position_obj->SetAttribute(ipo::TemplateAttributeEnums::THRESHOLD_SCORE, 0.8) != 0)
    return -1;
  cv::Mat &&dst = position_obj->GetResult(sample);
  if (dst.empty())
    return -1;
  // cv::imwrite("../images/result/positioning/positioning_template.jpg", dst);
  cv::imshow("dst", dst);
  cv::waitKey(0);
}
#elif index == 10
int main() {
  // image from : https://rawpedia.rawtherapee.com/File:Flatfield_landscape.jpg#file
  //----Load images----
  cv::Mat &&dark_field = cv::imread("../images/flat-field-correction/dark.jpg", 0);
  cv::Mat &&bright_field = cv::imread("../images/flat-field-correction/bright.jpg", 0);
  cv::Mat &&sample = cv::imread("../images/flat-field-correction/sample.jpg", 0);

  std::shared_ptr<ipo::FlatFieldCorrection> &&obj = std::make_shared<ipo::FlatFieldCorrection>();
  if (obj->SetDarkAndBrightFieldImage(dark_field, bright_field, 200) != 0)
    return -1;
  cv::Mat &&dst = obj->GetResult(sample);
  if (dst.empty()) {
    return -1;
  }
  cv::imwrite("../images/result/FlatFieldCorrection/CV_8UC1.jpg", dst);
  cv::imshow("dst", dst);
  cv::waitKey(0);
  return 0;
}
#elif index == 11

int main() {
#ifdef _WIN32
  const wchar_t *dumpPath = L"../dump_file/log";
  google_breakpad::ExceptionHandler eh(
      dumpPath, NULL, callback, NULL,
      google_breakpad::ExceptionHandler::HANDLER_ALL);
#endif
  std::cout << "main start" << std::endl;
  int *a = NULL;
  *a = 0x1;
  std::cout << "main end" << std::endl;
  return 0;
}
#endif