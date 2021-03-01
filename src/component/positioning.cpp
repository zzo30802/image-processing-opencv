#include "component/positioning.h"

#include "component/image_proc.h"

using namespace ipo;

//===========PIMPL (Positioning)===========
// Positioning::Positioning() : p_pimplPositioning(new PimplPositioning){}
Positioning::Positioning(const PositioningTypeEnums &type) : p_pimplPositioning(new PimplPositioning(type)) {}
Positioning::~Positioning() {}
int Positioning::SetGoldenSampleImage(const cv::Mat &golden_sample_img) {
  return p_pimplPositioning->SetGoldenSampleImage(golden_sample_img);
}
int Positioning::SetRect(const PositioningRectEnums &rect_type, const cv::Rect &rect) {
  return p_pimplPositioning->SetRect(rect_type, rect);
}
int Positioning::SetAttribute(const int &attribute_type, const double &value) {
  return p_pimplPositioning->SetAttribute(attribute_type, value);
}
cv::Mat Positioning::GetResult(const cv::Mat &sample_img) {
  return p_pimplPositioning->GetResult(sample_img);
}

//===========FeatureMatching============
int FeatureMatching::SetGoldenSampleImage(const cv::Mat &golden_sample_img) {
  if (golden_sample_img.empty()) {
    std::cout << "\nERROR: int ipo::FeatureMatching::SetGoldenSampleImage --(cv::Mat)src.empty()" << std::endl;
    return -1;
  }
  this->golden_sample_image = cv::Mat::zeros(golden_sample_img.size(), golden_sample_img.type());
  this->golden_sample_image = golden_sample_img.clone();
  return 0;
}

int FeatureMatching::SetRect(const PositioningRectEnums &rect_type, const cv::Rect &rect) {
  // check input
  if (rect.empty()) {
    std::cout << "\nERROR: int FeatureMatching::SetRect --(cv::Rect)rect.empty()" << std::endl;
    return -1;
  }
  switch (rect_type) {
    case PositioningRectEnums::TEMPLATE_IMG_RECT: {
      this->template_rect = rect;
      break;
    }
    case PositioningRectEnums::SEARCHING_IMG_RECT: {
      this->searching_rect = rect;
      break;
    }
    default: {
      std::cout << "\nERROR: int FeatureMatching::SetRect" << std::endl;
      std::cout << "    (enum)PositioningRectEnums : There is no such enum in the enumeration list." << std::endl;
      return -1;
    }
  }
  return 0;
}

// template <typename T>
int FeatureMatching::SetAttribute(const int &attribute_type, const double &value) {
  switch (attribute_type) {
    case FeatureAttributeEnums::HESSIAN_THRESHOLD: {
      if (value < 100) {
        std::cout << "\nERROR: int FeatureMatching::SetAttribute" << std::endl;
        std::cout << "    100 < int HESSIAN_THRESHOLD < 3000" << std::endl;
        return -1;
      } else if (value > 3000) {
        std::cout << "\nERROR: int FeatureMatching::SetAttribute" << std::endl;
        std::cout << "    100 < int HESSIAN_THRESHOLD < 3000" << std::endl;
        return -1;
      }
      this->hessian_threshold = static_cast<int>(value);
      break;
    }
    case FeatureAttributeEnums::LOWE_RATIO: {
      if (value < 0) {
        std::cout << "\nERROR: int FeatureMatching::SetAttribute" << std::endl;
        std::cout << "    0 < float LOWE_RATIO < 1.0f" << std::endl;
        return -1;
      } else if (value > 1.0f) {
        std::cout << "\nERROR: int FeatureMatching::SetAttribute" << std::endl;
        std::cout << "    0 < float LOWE_RATIO < 1.0f" << std::endl;
        return -1;
      }
      this->lowe_ratio = static_cast<float>(value);
      break;
    }
    default: {
      std::cout << "\nERROR: int FeatureMatching::SetAttribute" << std::endl;
      std::cout << "    (enum)FeatureAttributeEnums : There is no such enum in the enumeration list." << std::endl;
      return -1;
    }
  }
  return 0;
}

cv::Mat FeatureMatching::GetHomography(const cv::Mat &sample_img) {
  // check input
  if (sample_img.empty()) {
    std::cout << "\nERROR: int ipo::FeatureMatching::GetHomography --(cv::Mat)sample_img.empty()" << std::endl;
    return {};
  }
  // print all input attribute
  std::cout << "hessian_threshold: " << this->hessian_threshold << std::endl;
  std::cout << "lowe_ratio: " << this->lowe_ratio << std::endl;

  // crop image (template & searching)
  this->template_img = cv::Mat::zeros(sample_img.size(), sample_img.type());
  this->golden_sample_image(this->template_rect).copyTo(template_img);
  this->searching_img = cv::Mat::zeros(sample_img.size(), sample_img.type());
  sample_img(this->searching_rect).copyTo(searching_img);

  // cv::imshow("this->template_img", this->template_img);
  // cv::imshow("this->searching_img", this->searching_img);
  // cv::waitKey(0);

  // Detect the keypoints using SURF Detector, compute the descriptors
  cv::Ptr<cv::xfeatures2d::SURF> &&detector = cv::xfeatures2d::SURF::create(this->hessian_threshold);
  std::vector<cv::KeyPoint> keypoints_template, keypoints_searching;
  cv::Mat descriptors_object, descriptors_scene;
  detector->detectAndCompute(template_img, cv::noArray(), keypoints_template, descriptors_object);
  detector->detectAndCompute(searching_img, cv::noArray(), keypoints_searching, descriptors_scene);
  if (descriptors_object.empty() || descriptors_scene.empty()) {
    std::cout << "\nERROR: cv::Mat FeatureMatching::GetHomography" << std::endl;
    std::cout << "    detector->detectAndCompute() : (cv::Mat)descriptors_object.empty()" << std::endl;
    return {};
  } else if (keypoints_template.size() < 2 || keypoints_searching.size() < 2) {
    std::cout << "\nERROR: cv::Mat FeatureMatching::GetHomography" << std::endl;
    std::cout << "    detector->detectAndCompute() : (std::vector<cv::KeyPoint>)keypoints_template.size() < 2" << std::endl;
    return {};
  }

  // Matching descriptor vectors with a FLANN based matcher
  // cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::BRUTEFORCE);
  cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create(cv::DescriptorMatcher::FLANNBASED);
  std::vector<std::vector<cv::DMatch>> knn_matches;
  matcher->knnMatch(descriptors_object, descriptors_scene, knn_matches, 2);
  if (knn_matches.empty()) {
    std::cout << "\nERROR: cv::Mat FeatureMatching::GetHomography" << std::endl;
    std::cout << "    (std::vector<std::vector<cv::DMatch>>)knn_matches.emmpty()" << std::endl;
    return {};
  }
  std::cout << "knn_matches.size() : " << knn_matches.size() << std::endl;

  // Filter matches using the Lowe's ratio test
  std::vector<cv::DMatch> good_matches;
  for (size_t i = 0; i < knn_matches.size(); i++) {
    if (knn_matches[i][0].distance < this->lowe_ratio * knn_matches[i][1].distance) {
      good_matches.emplace_back(knn_matches[i][0]);
    }
  }
  if (good_matches.empty()) {
    std::cout << "\nERROR: cv::Mat FeatureMatching::GetHomography" << std::endl;
    std::cout << "    (std::vector<std::vector<cv::DMatch>>)good_matches.emmpty()" << std::endl;
    return {};
  }

  // Get the keypoints from the good matches
  std::vector<cv::Point2f> template_pt_vec;
  std::vector<cv::Point2f> searching_pt_vec;
  for (size_t i = 0; i < good_matches.size(); i++) {
    template_pt_vec.emplace_back(keypoints_template[good_matches[i].queryIdx].pt);
    searching_pt_vec.emplace_back(keypoints_searching[good_matches[i].trainIdx].pt);
  }
  cv::Mat &&H = cv::findHomography(template_pt_vec, searching_pt_vec, cv::RANSAC);
  if (H.empty()) {
    std::cout << "\nERROR: cv::Mat FeatureMatching::GetHomography" << std::endl;
    std::cout << "    (cv::Mat)H.emmpty()" << std::endl;
    return {};
  }
  return H.clone();
}

cv::Mat FeatureMatching::GetResult(const cv::Mat &sample_img) {
  cv::Mat &&H = this->GetHomography(sample_img);
  if (H.empty())
    return {};

  std::vector<cv::Point2f> template_corners(4);
  template_corners[0] = cv::Point2f(0, 0);
  template_corners[1] = cv::Point2f(static_cast<float>(this->template_img.cols), 0);
  template_corners[2] = cv::Point2f(static_cast<float>(this->template_img.cols), static_cast<float>(this->template_img.rows));
  template_corners[3] = cv::Point2f(0, static_cast<float>(this->template_img.rows));
  std::vector<cv::Point2f> detected_corners(4);
  cv::perspectiveTransform(template_corners, detected_corners, H);

  // calculate the rotation angle
  const double &&search_angle = GetTwoPointAngle(detected_corners[0], detected_corners[1]);
  cv::Point2f &&rotated_center = cv::Point2f(this->searching_rect.x + detected_corners[0].x,
                                             this->searching_rect.y + detected_corners[0].y);
  //----shift----
  const int offsetx = rotated_center.x - this->template_rect.x;
  const int offsety = rotated_center.y - this->template_rect.y;

  cv::Mat &&dst = cv::Mat::zeros(sample_img.size(), sample_img.type());
  dst = sample_img.clone();
  // dst = ImageRotate(dst, search_angle, rotated_center);
  // dst = ImageShift(dst, rotated_center, cv::Point(template_rect.x, template_rect.y));
  // cv::Mat &&shifted_mat = (cv::Mat_<double>(3, 3) << 1, 0, -offsetx, 0, 1, -offsety, 0, 0, 1);
  // cv::Mat &&rotated_mat = cv::getRotationMatrix2D(rotated_center, -search_angle, 1.0);
  // cv::Mat &&row = (cv::Mat_<double>(1, 3) << 0, 0, 1);
  // cv::Mat conbine_mat;
  // cv::vconcat(rotated_mat, row, conbine_mat);
  // conbine_mat *= shifted_mat;
  // cv::warpPerspective(dst, dst, conbine_mat, sample_img.size(), cv::INTER_LANCZOS4);
  cv::Mat &&transform_mat = cv::getRotationMatrix2D(rotated_center, -search_angle, 1.0);
  transform_mat.at<double>(0, 2) += -offsetx;
  transform_mat.at<double>(1, 2) += -offsety;
  cv::warpAffine(dst, dst, transform_mat, sample_img.size(), cv::INTER_LANCZOS4);
  return dst.clone();
}

//===========TemplateMatching============
int TemplateMatching::SetGoldenSampleImage(const cv::Mat &golden_sample_img) {
  if (golden_sample_img.empty()) {
    std::cout << "\nERROR: int ipo::TemplateMatching::SetGoldenSampleImage --(cv::Mat)src.empty()" << std::endl;
    return -1;
  }
  this->golden_sample_image = cv::Mat::zeros(golden_sample_img.size(), golden_sample_img.type());
  this->golden_sample_image = golden_sample_img.clone();
  return 0;
}

int TemplateMatching::SetRect(const PositioningRectEnums &rect_type, const cv::Rect &rect) {
  // check input
  if (rect.empty()) {
    std::cout << "\nERROR: int TemplateMatching::SetRect --(cv::Rect)rect.empty()" << std::endl;
    return -1;
  }
  switch (rect_type) {
    case PositioningRectEnums::TEMPLATE_IMG_RECT: {
      this->template_rect = rect;
      break;
    }
    case PositioningRectEnums::SEARCHING_IMG_RECT: {
      this->searching_rect = rect;
      break;
    }
    default: {
      std::cout << "\nERROR: int TemplateMatching::SetRect" << std::endl;
      std::cout << "    (enum)PositioningRectEnums : There is no such enum in the enumeration list." << std::endl;
      return -1;
    }
  }
  return 0;
}

int TemplateMatching::SetAttribute(const int &attribute_type, const double &value) {
  switch (attribute_type) {
    case TemplateAttributeEnums::ANGLE_TOLERANCE: {
      if (value < 0.0) {
        std::cout << "\nERROR: int TemplateMatching::SetAttribute" << std::endl;
        std::cout << "    0 < double angle_tolerance <= 180" << std::endl;
        return -1;
      } else if (value > 180.0) {
        std::cout << "\nERROR: int TemplateMatching::SetAttribute" << std::endl;
        std::cout << "    0 < double angle_tolerance <= 180" << std::endl;
        return -1;
      }
      this->angle_tolerance = static_cast<double>(value);
      break;
    }
    case TemplateAttributeEnums::NUMBER_OF_LEVELS: {
      if (value < 1) {
        std::cout << "\nERROR: int TemplateMatching::SetAttribute" << std::endl;
        std::cout << "    0 < int number_of_levels <= 5" << std::endl;
        return -1;
      } else if (value > 5) {
        std::cout << "\nERROR: int TemplateMatching::SetAttribute" << std::endl;
        std::cout << "    0 < int number_of_levels <= 5" << std::endl;
        return -1;
      }
      this->number_of_levels = static_cast<int>(value);
      break;
    }
    case TemplateAttributeEnums::THRESHOLD_SCORE: {
      if (value < 0) {
        std::cout << "\nERROR: int TemplateMatching::SetAttribute" << std::endl;
        std::cout << "    0 < double threshold_score <= 1.0" << std::endl;
        return -1;
      } else if (value > 1.0) {
        std::cout << "\nERROR: int TemplateMatching::SetAttribute" << std::endl;
        std::cout << "    0 < double threshold_score <= 1.0" << std::endl;
        return -1;
      }
      this->similarity_score = static_cast<double>(value);
      break;
    }
    default: {
      std::cout << "\nERROR: int TemplateMatching::SetAttribute" << std::endl;
      std::cout << "    (enum)TemplateAttributeEnums : There is no such enum in the enumeration list." << std::endl;
      return -1;
    }
  }
  return 0;
}

cv::Mat TemplateMatching::GetResult(const cv::Mat &sample_img) {
  // check input
  if (sample_img.empty()) {
    std::cout << "\nERROR: cv::Mat ipo::TemplateMatching::GetResult --(cv::Mat)sample_img.empty()" << std::endl;
    return {};
  }

  // print all input attribute
  std::cout << "angle_tolerance: " << this->angle_tolerance << std::endl;
  std::cout << "number_of_levels: " << this->number_of_levels << std::endl;
  std::cout << "similarity_score: " << this->similarity_score << std::endl;

  // crop image (template & searching)
  this->template_img = cv::Mat::zeros(sample_img.size(), sample_img.type());
  this->golden_sample_image(this->template_rect).copyTo(template_img);
  this->searching_img = cv::Mat::zeros(sample_img.size(), sample_img.type());
  sample_img(this->searching_rect).copyTo(searching_img);

  // create cv::Mat
  // searching && template image
  cv::Mat &&gray_searching = cv::Mat::zeros(this->searching_img.size(), CV_8UC1);
  cv::Mat &&gray_template = cv::Mat::zeros(this->template_img.size(), CV_8UC1);
  // original image (for finding the global coordinate)
  cv::Mat &&global_gray_searching = cv::Mat::zeros(sample_img.size(), CV_8UC1);
  cv::Mat &&global_gray_template = cv::Mat::zeros(this->template_img.size(), CV_8UC1);
  const int &&channel = sample_img.channels();
  switch (channel) {
    case 1: {
      //----searching && template image----
      gray_searching = this->searching_img.clone();
      gray_template = this->template_img.clone();
      //----the original size image----
      global_gray_searching = sample_img.clone();
      global_gray_template = this->template_img.clone();
      break;
    }
    case 3: {
      //----searching && template image----
      cv::cvtColor(this->searching_img, gray_searching, cv::COLOR_BGR2GRAY, 1);
      cv::cvtColor(this->template_img, gray_template, cv::COLOR_BGR2GRAY, 1);
      //----the original size image----
      cv::cvtColor(sample_img, global_gray_searching, cv::COLOR_BGR2GRAY, 1);
      cv::cvtColor(this->template_img, global_gray_template, cv::COLOR_BGR2GRAY, 1);
      break;
    }
    default: {
      std::cout << "\nERROR: cv::Mat ipo::TemplateMatching::GetResult" << std::endl;
      std::cout << "    image channel must be equal to 1 or 3" << std::endl;
      return {};
    }
  }
  // image Pyramids (downsize, zoom out)
  for (size_t i = 0; i < this->number_of_levels; i++) {
    cv::pyrDown(gray_searching, gray_searching, cv::Size(gray_searching.cols / 2, gray_searching.rows / 2));
    cv::pyrDown(gray_template, gray_template, cv::Size(gray_template.cols / 2, gray_template.rows / 2));
  }

  double &&min_angle = -(this->angle_tolerance);
  double &max_angle = this->angle_tolerance;
  double &&angle_range = std::abs(min_angle) + std::abs(max_angle);
  const double &start = min_angle;
  std::cout << "angle_range : " << angle_range << std::endl;
  std::cout << "start : " << start << std::endl;

  const double &accuracy = 0.1;

  double &&minVal = 0;
  double &&maxVal = 0;
  cv::Point minLoc, maxLoc, location;
  double &&temp_max = 0;
  double &&angle = 0;
  int &&width = gray_searching.cols - gray_template.cols + 1;
  int &&height = gray_searching.rows - gray_template.rows + 1;
  cv::Mat &&result_img = cv::Mat::zeros(width, height, CV_8UC1);

  // calculate once first
  cv::matchTemplate(gray_searching, gray_template, result_img, cv::TM_CCOEFF_NORMED);
  cv::minMaxLoc(result_img, &minVal, &maxVal, &minLoc, &maxLoc, cv::noArray());
  location = maxLoc;
  temp_max = maxVal;

  // rotate loop (find angle)
  cv::Mat rotated_gray_searching;
  for (int i = 0; i < static_cast<int>(angle_range / accuracy) - 1; i++) {
    const double &&current_angle = start + accuracy * i;
    if (GetNewRotatedImageSize(gray_searching, current_angle, width, height) != 0)
      return {};
    width = gray_searching.cols - gray_template.cols + 1;
    height = gray_searching.rows - gray_template.rows + 1;
    result_img = cv::Mat::zeros(width, height, CV_8UC1);
    rotated_gray_searching = cv::Mat::zeros(width, height, CV_8UC1);
    rotated_gray_searching = ImageRotateByCenterAndAdjustBoundary(gray_searching, -current_angle);
    cv::matchTemplate(rotated_gray_searching, gray_template, result_img, cv::TM_CCOEFF_NORMED);
    cv::minMaxLoc(result_img, &minVal, &maxVal, &minLoc, &maxLoc, cv::noArray());
    if (maxVal > temp_max) {
      location = maxLoc;
      temp_max = maxVal;
      angle = current_angle;
    }
  }
  std::cout << "angle : " << angle << std::endl;

  // rotate image
  global_gray_searching = ImageRotate(global_gray_searching, -angle, cv::Point(global_gray_searching.cols / 2, global_gray_searching.rows / 2));
  // find global coordinate
  width = global_gray_searching.cols - global_gray_template.cols + 1;
  height = global_gray_searching.rows - global_gray_template.rows + 1;
  result_img = cv::Mat::zeros(width, height, CV_8UC1);
  cv::matchTemplate(global_gray_searching, global_gray_template, result_img, cv::TM_CCOEFF_NORMED);
  cv::minMaxLoc(result_img, &minVal, &maxVal, &minLoc, &maxLoc, cv::noArray());

  // result
  cv::Mat &&dst = cv::Mat::zeros(sample_img.size(), sample_img.type());
  dst = sample_img.clone();
  // dst = ImageRotate(dst, -angle, cv::Point(dst.cols / 2, dst.rows / 2));
  // dst = ImageShift(dst, maxLoc, cv::Point(template_rect.x, template_rect.y));
  const int offsetx = maxLoc.x - this->template_rect.x;
  const int offsety = maxLoc.y - this->template_rect.y;
  cv::Mat &&transform_mat = cv::getRotationMatrix2D(cv::Point(dst.cols / 2, dst.rows / 2), angle, 1.0);
  transform_mat.at<double>(0, 2) += -offsetx;
  transform_mat.at<double>(1, 2) += -offsety;
  cv::warpAffine(dst, dst, transform_mat, sample_img.size(), cv::INTER_LANCZOS4);
  return dst.clone();
}

//====class Positioning====
Positioning::PimplPositioning::PimplPositioning(const PositioningTypeEnums &type) {
  switch (type) {
    case PositioningTypeEnums::FEATURE_MATCHING: {
      this->ptr = new FeatureMatching();
      break;
    }
    case PositioningTypeEnums::TEMPLATE_MATCHING: {
      this->ptr = new TemplateMatching();
      break;
    }
    default: {
      std::cout << "(enum)PositioningTypeEnums There is no such enum in the enumeration list." << std::endl;
      std::cout << "Switch to the default algorithm : TemplateMatching" << std::endl;
      break;
    }
  }
}
Positioning::PimplPositioning::~PimplPositioning() {
  delete this->ptr;
}
int Positioning::PimplPositioning::SetGoldenSampleImage(const cv::Mat &golden_sample_img) {
  if (this->ptr->SetGoldenSampleImage(golden_sample_img) != 0) {
    return -1;
  }
  return 0;
}
int Positioning::PimplPositioning::SetRect(const PositioningRectEnums &rect_type, const cv::Rect &rect) {
  if (this->ptr->SetRect(rect_type, rect) != 0) {
    return -1;
  }
  return 0;
}
int Positioning::PimplPositioning::SetAttribute(const int &attribute_type, const double &value) {
  if (this->ptr->SetAttribute(attribute_type, value) != 0)
    return -1;
  return 0;
}
cv::Mat Positioning::PimplPositioning::GetResult(const cv::Mat &sample_img) {
  return this->ptr->GetResult(sample_img);
}
