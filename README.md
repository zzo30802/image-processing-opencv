# image-processing-opencv
Some algorithms to do image processing.

## Environment
### Compiler: 
|Win 10|Ubuntu 1604|
|:--:|:--:|
|Visual studio 2017|GCC 7.5.0|

### Third Party
|Package|Version|
|:--:|:--:| 
|OpenCV| 3.4.9| 

# How to build
1. CmakeLists.txt 
    * add_executable : build ..exe
    * add_library : build .dll / .so
2. build
```
// for windows
$ cd build
$ build.bat

// for linux
$ cd build
$ cmake ..
$ make
```

# Description
This repository contains image processing, like positioning, flat-field correction, dynamic threshold ...etc. 

# Function
**enum**
```
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
```

**int ipo::DynamicThreshold(const cv::Mat &src, cv::Mat &dst,
                         const int &blur_ksize, const int &offset,
                         const DynamicThresholdTypes &mode)**

    @param src          input image (CV_8UC1 or CV_8UC3).

    @param dst          output image (CV_8UC1).

    @param blur_ksize   blurring kernel size.

    @param offset       the offset value between blurring image and src image.

    @param mode         (DynamicThresholdTypes) switch to the select mode.

    @return             0:ok; -1:error
    