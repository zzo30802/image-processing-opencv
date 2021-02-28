#ifndef _COMMON_H_
#define _COMMON_H_

#include <cmath>
#include <experimental/filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// opencv
// #include <opencv2/tracking.hpp>

#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/xfeatures2d.hpp"

// boost
// #include "boost/filesystem.hpp"
// #include "boost/regex.hpp"

// breakpad
#ifdef _WIN32
#include "client/windows/handler/exception_handler.h"
#endif

// because c++ 11 don't have make_unique, so we make one by myself.
template <typename T, typename... Args>
static std::unique_ptr<T> make_unique(Args &&...args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// inline int LoadFolderPath(const std::string &folder_path, std::vector<std::string> &img_path_vector, std::vector<std::string> &img_filename_vector) {
//   std::cout << "LoadFolderPath() start" << std::endl;
//   if (!folder_path.empty()) {
//     if (!boost::filesystem::exists(folder_path)) {
//       std::cout << "Folder : " << folder_path << " does not exist." << std::endl;
//       return -1;
//     }
//     // boost::regex pattern(folder_path + "/[0-9]*.png");
//     boost::filesystem::directory_iterator end_itr;
//     for (boost::filesystem::directory_iterator itr(folder_path); itr != end_itr; ++itr) {
//       if (boost::filesystem::is_directory(itr->status())) {
//       } else if ((itr->path().extension() == ".png") || (itr->path().extension() == ".bmp")) {
//         std::string imgpath = itr->path().string();
//         // std::cout << "imgpath : " << imgpath << std::endl;
//         img_path_vector.push_back(itr->path().string());
//         img_filename_vector.push_back(itr->path().filename().stem().string());
//       }
//       // else if (boost::regex_match(itr->path().string(), pattern)){
//       //   img_path_vector.push_back(itr->path().string());
//       // }
//     }
//   }
//   std::cout << "LoadFolderPath() end" << std::endl;
//   return 0;
// }

inline int ConvertMatToGrayscale(const cv::Mat &src, cv::Mat &gray_dst) {
  const int &&channel = src.channels();
  switch (channel) {
    case 1: {
      gray_dst = cv::Mat::zeros(src.size(), CV_8UC1);
      gray_dst = src.clone();
      break;
    }
    case 3: {
      gray_dst = cv::Mat::zeros(src.size(), CV_8UC1);
      cv::cvtColor(src, gray_dst, cv::COLOR_BGR2GRAY, 1);
      break;
    }
    default: {
      std::cout << "  --> int ConvertMatToGrayscale (cv::Mat)src.channels != 1 || 3" << std::endl;
      return -1;
    }
  }
  return 0;
}

#endif  //_COMMON_H_