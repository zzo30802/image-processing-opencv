#ifndef _POSITIONING_H_
#define _POSITIONING_H_

#include "common.h"
#include "image_proc.h"
namespace ipo {
enum PositioningTypeEnums {
  FEATURE_MATCHING = 0,
  TEMPLATE_MATCHING = 1,
};
enum PositioningRectEnums {
  TEMPLATE_IMG_RECT = 0,
  SEARCHING_IMG_RECT = 1,
};
enum FeatureAttributeEnums {
  HESSIAN_THRESHOLD = 0,  // 100~3000
  LOWE_RATIO = 1,         // 0~1.0
};
enum TemplateAttributeEnums {
  ANGLE_TOLERANCE = 0,   // 0~180
  NUMBER_OF_LEVELS = 1,  // 1~5
  THRESHOLD_SCORE = 2,   // 0~1.0
};

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
  int GG(){};

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
};

class Positioning : public Creator {
 public:
  Positioning(const PositioningTypeEnums &type) {
    switch (type) {
      case PositioningTypeEnums::FEATURE_MATCHING: {
        ptr = new FeatureMatching();
        break;
      }
      case PositioningTypeEnums::TEMPLATE_MATCHING: {
        ptr = new TemplateMatching();
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
  int SetAttribute(const int &attribute_type, const double &value) {
    if (ptr->SetAttribute(attribute_type, value) != 0)
      return -1;
    return 0;
  }
  cv::Mat GetResult(const cv::Mat &sample_img) {
    return ptr->GetResult(sample_img);
  }

 private:
  IPositioning *ptr;
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