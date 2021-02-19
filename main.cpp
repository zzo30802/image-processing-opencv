#include "component/image_proc.h"

// cv::Mat ImgProcTestFunc(cv::)

int main() {
  // ---load image---
  cv::Mat &&src = cv::imread("D:\\images\\Kenmec\\20210208\\13.png");
  // cv::Mat &&dst = cv::Mat::zeros(src.size(), src.type());
  cv::Mat dst;
  // std::shared_ptr<ipo::ImageProc> obj = std::make_shared<ipo::ImageProc>();
  ipo::DynamicThreshold(src, dst, 21, 10, ipo::DynamicThresholdTypes::DYNAMIC_THRES_LIGHT);
  cv::imshow("dst", dst);
  cv::waitKey(0);
  return 0;
}