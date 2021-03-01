#ifndef _POSITIONING_H_
#define _POSITIONING_H_

#include "attribute.h"
#include "common.h"
#include "image_proc.h"
namespace ipo {
class IPositioning {
 public:
  virtual int SetGoldenSampleImage(const cv::Mat &golden_sample_img) = 0;
  virtual int SetRect(const PositioningRectEnums &rect_type, const cv::Rect &rect) = 0;
  virtual int SetAttribute(const int &attribute_type, const double &value) = 0;
  virtual cv::Mat GetResult(const cv::Mat &sample_img) = 0;
};

class FeatureMatching : public IPositioning {
 public:
  int SetGoldenSampleImage(const cv::Mat &golden_sample_img);
  int SetRect(const PositioningRectEnums &rect_type, const cv::Rect &rect);
  // template <typename T>
  int SetAttribute(const int &attribute_type, const double &value);
  cv::Mat GetResult(const cv::Mat &sample_img);

 private:
  cv::Mat GetHomography(const cv::Mat &sample_img);

 private:
  //----cv::Rect----
  cv::Rect template_rect;
  cv::Rect searching_rect;
  //----cv::Mat----
  cv::Mat golden_sample_image;
  cv::Mat template_img;
  cv::Mat searching_img;
  //----attribute----
  int hessian_threshold;
  float lowe_ratio;
};

class TemplateMatching : public IPositioning {
 public:
  int SetGoldenSampleImage(const cv::Mat &golden_sample_img);
  int SetRect(const PositioningRectEnums &rect_type, const cv::Rect &rect);
  // template <typename T>
  int SetAttribute(const int &attribute_type, const double &value);
  cv::Mat GetResult(const cv::Mat &sample_img);

 private:
  //----cv::Rect----
  cv::Rect template_rect;
  cv::Rect searching_rect;
  //----cv::Mat----
  cv::Mat golden_sample_image;
  cv::Mat template_img;
  cv::Mat searching_img;
  //----attribute----
  double angle_tolerance;
  int number_of_levels;
  double similarity_score;
};

//=========method 1=========
class Creator {
 public:
  // virtual void Create(const PositioningTypeEnums &type) = 0;
 protected:
  IPositioning *ptr;
};

class Positioning::PimplPositioning : public Creator {
 public:
  // PimplPositioning();
  PimplPositioning(const PositioningTypeEnums &type);
  ~PimplPositioning();
  int SetGoldenSampleImage(const cv::Mat &golden_sample_img);
  int SetRect(const PositioningRectEnums &rect_type, const cv::Rect &rect);
  int SetAttribute(const int &attribute_type, const double &value);
  cv::Mat GetResult(const cv::Mat &sample_img);
};

//=========method 2=========
// class Creator {
//  public:
//   virtual IPositioning *GetInstance(const PositioningTypeEnums &type) = 0;
// };
// class Positioning : public Creator {
//  public:
//   IPositioning *GetInstance(const PositioningTypeEnums &type) {
//     switch (type) {
//       case PositioningTypeEnums::FEATURE_MATCHING: {
//         return new FeatureMatching();
//       }
//       case PositioningTypeEnums::TEMPLATE_MATCHING: {
//         return new TemplateMatching();
//       }
//     }
//   }

// };

// class Positioning {
//  public:
//   static std::shared_ptr<IPositioning> Create(const PositioningTypeEnums &type) {
//     switch (type) {
//       case PositioningTypeEnums::FEATURE_MATCHING: {
//         std::shared_ptr<FeatureMatching> const positioning(std::make_shared<FeatureMatching>());
//         return positioning;
//       }
//       case PositioningTypeEnums::TEMPLATE_MATCHING: {
//         std::shared_ptr<TemplateMatching> const positioning(std::make_shared<TemplateMatching>());
//         return positioning;
//       }
//       default: {
//         std::cout << "(enum)PositioningTypeEnums There is no such enum in the enumeration list." << std::endl;
//         std::cout << "Switch to the default algorithm : TemplateMatching" << std::endl;
//       }
//     }
//   }
// };

}  // namespace ipo
#endif  // _POSITIONING_H_