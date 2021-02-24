#include "component/image_proc.h"
#include "component/positioning.h"
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
7 : GetNewRotatedImageSize()
------positioning------
8 : positioning by feature matching(SURF)
9 : positioning by template matching
*/
#define index 8

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
  if (ipo::Variance(src, dst, 5) != 0)
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
int main() {
  cv::resize(src, src, cv::Size(), 0.6, 0.6);

  const double &&angle = 30;
  int width, height;
  ipo::GetNewRotatedImageSize(src, angle, width, height);
  cv::Mat &&dst = cv::Mat::zeros(cv::Size(width, height), src.type());
  dst = ipo::ImageRotateByCenter(src, angle);
  cv::imshow("dst", dst);
  cv::waitKey(0);
  return 0;
}
#elif index == 8
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
  cv::imshow("dst", dst);
  cv::waitKey(0);
}
#endif