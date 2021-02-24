#include "positioning.h"
using namespace ipo;
//===========FeatureMatching============
int FeatureMatching::SetGoldenSampleImage(const cv::Mat &src) {
  if (src.empty()) {
    std::cout << "\nERROR: int ipo::FeatureMatching::SetGoldenSampleImage --(cv::Mat)src.empty()" << std::endl;
    return -1;
  }
  this->golden_sample_image = cv::Mat::zeros(src.size(), src.type());
  this->golden_sample_image = src.clone();
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
int FeatureMatching::SetAttribute(const FeatureAttributeEnums &attri, float value) {
  switch (attri) {
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
      std::cout << "HESSIAN_THRESHOLD 1" << std::endl;
      std::cout << value << std::endl;
      this->hessian_threshold = static_cast<int>(value);
      std::cout << "HESSIAN_THRESHOLD 2" << std::endl;
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
      std::cout << "LOWE_RATIO 1" << std::endl;
      std::cout << value << std::endl;
      this->lowe_ratio = static_cast<float>(value);
      std::cout << "LOWE_RATIO 2" << std::endl;
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
  cv::Mat &&rot_mat = cv::getRotationMatrix2D(rotated_center, -search_angle, 1.0);

  //----rotate----
  // calculate the new image size
  int width, height;
  GetNewRotatedImageSize(sample_img, -search_angle, width, height);
  cv::Mat &&dst = cv::Mat::zeros(sample_img.size(), sample_img.type());
  cv::warpAffine(sample_img, dst, rot_mat, cv::Size(width, height), cv::INTER_LANCZOS4, cv::BORDER_CONSTANT);
  //----shift----
  const int offsetx = rotated_center.x - this->template_rect.x;
  const int offsety = rotated_center.y - this->template_rect.y;
  std::cout << "offsetx : " << offsetx << std::endl;
  std::cout << "offsety : " << offsety << std::endl;
  const cv::Mat &&shifte_mat = (cv::Mat_<double>(2, 3) << 1, 0, -offsetx, 0, 1, -offsety);
  cv::warpAffine(dst, dst, shifte_mat, sample_img.size());
  return dst.clone();
}

//===========TemplateMatching============