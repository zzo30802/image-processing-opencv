#ifndef _ATTRIBUTE_H_
#define _ATTRIBUTE_H_
namespace ipo {
enum class DynamicThresholdTypes {
  DYNAMIC_THRES_LIGHT = 0,
  DYNAMIC_THRES_DARK = 1,
  DYNAMIC_THRES_LIGHT_AND_DARK_INRANGE = 2,
  DYNAMIC_THRES_LIGHT_OR_DARK_OUTRANGE = 3,
};
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
};  // namespace ipo

#endif  // _ATTRIBUTE_H_