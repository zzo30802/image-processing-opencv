#ifndef _POSITIONING_H_
#define _POSITIONING_H_
#include <memory>

#include "common.h"
#include "image_proc.h"
namespace ipo {
enum class PositioningTypeEnums {
  FEATURE_MATCHING = 0,
  TEMPLATE_MATCHING = 1,
};
enum class PositioningRectEnums {
  TEMPLATE_IMG_RECT = 0,
  SEARCHING_IMG_RECT = 1,
};
enum class FeatureAttributeEnums {
  HESSIAN_THRESHOLD = 0,  // 100~3000
  LOWE_RATIO = 1,         // 0~1.0f
};
enum class TemplateAttributeEnums {
  ANGLE_TOLERANCE = 0,
  NUMBER_OF_LEVELS = 1,
  THRESHOLD_SCORE = 2,
};
// class IPositioning {
//  public:
//   virtual int SetGoldenSampleImage(const cv::Mat &golden_sample_img) = 0;
//   virtual int SetRect(const PositioningRectEnums &rect_type, const cv::Rect &rect) = 0;
//   virtual int SetAttribute() = 0;
//   virtual cv::Mat GetResult(const cv::Mat &sample_img) = 0;

//  protected:
//   cv::Mat golden_sample_image;
//   cv::Rect template_rect;
//   cv::Rect searching_rect;
// };

class IPositioning {
 public:
  virtual int SetGoldenSampleImage(const cv::Mat &golden_sample_img) = 0;
  virtual int SetRect(const PositioningRectEnums &rect_type, const cv::Rect &rect) = 0;
  virtual int SetAttribute(const FeatureAttributeEnums &attri, float value) = 0;
  virtual cv::Mat GetResult(const cv::Mat &sample_img) = 0;

  //  protected:
  //   cv::Mat golden_sample_image;
  //   cv::Rect template_rect;
  //   cv::Rect searching_rect;
};

class FeatureMatching : public IPositioning {
 public:
  int SetGoldenSampleImage(const cv::Mat &golden_sample_img);
  int SetRect(const PositioningRectEnums &rect_type, const cv::Rect &rect);
  // template <typename T>
  int SetAttribute(const FeatureAttributeEnums &attri, float value);
  cv::Mat GetResult(const cv::Mat &sample_img);

 private:
  cv::Mat golden_sample_image;
  cv::Rect template_rect;
  cv::Rect searching_rect;

  int hessian_threshold;
  float lowe_ratio;
  cv::Mat template_img;
  cv::Mat searching_img;
  cv::Mat GetHomography(const cv::Mat &sample_img);
};

// class TemplateMatching : public IPositioning {
//  public:
//   int SetGoldenSampleImage(const cv::Mat &golden_sample_img);
//   int SetRect(const PositioningRectEnums &rect_type, const cv::Rect &rect);
//   // template <typename T>
//   int SetAttribute(const TemplateAttributeEnums &attri, const float &value);
//   cv::Mat GetResult(const cv::Mat &sample_img);

//  private:
//   cv::Mat golden_sample_image;
//   cv::Rect template_rect;
//   cv::Rect searching_rect;

//   double angle_tolerance;
//   int number_of_levels;
//   double threshold_score;
// };

class Creator {
 public:
  // virtual void Create(const PositioningTypeEnums &type) = 0;
};

class Positioning : public Creator, public FeatureMatching {
 public:
  Positioning(const PositioningTypeEnums &type) {
    switch (type) {
      case PositioningTypeEnums::FEATURE_MATCHING: {
        ptr = new FeatureMatching();
        // #define xxx 1
        break;
      }
      case PositioningTypeEnums::TEMPLATE_MATCHING: {
        break;
      }
      default: {
        std::cout << "(enum)PositioningTypeEnums There is no such enum in the enumeration list." << std::endl;
        std::cout << "Switch to the default algorithm : TemplateMatching" << std::endl;
        break;
      }
    }
  }
  ~Positioning() {
    delete ptr;
  }
  int SetGoldenSampleImage(const cv::Mat &golden_sample_img) {
    if (ptr->SetGoldenSampleImage(golden_sample_img) != 0) {
      return -1;
    }
    return 0;
  }
  int SetRect(const PositioningRectEnums &rect_type, const cv::Rect &rect) {
    if (ptr->SetRect(rect_type, rect) != 0) {
      return -1;
    }
    return 0;
  }
  int SetAttribute(const FeatureAttributeEnums &attri, float value) {
    if (ptr->SetAttribute(attri, value) != 0)
      return -1;
    return 0;
  }
  // #if xxx == 1
  //   // int Set1(){};
  //   int SetAttribute(const FeatureAttributeEnums &attri, const float &value){};
  // #elif xxx == 2
  //   // int Set2(){};
  //   int SetAttribute(){};
  //   // nt SetAttribute(const FeatureAttributeEnums &attri, const float &value){};
  // #endif

  cv::Mat GetResult(const cv::Mat &sample_img) {
    return ptr->GetResult(sample_img);
  }

 public:
  IPositioning *ptr;
};

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