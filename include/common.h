#ifndef _COMMON_H_
#define _COMMON_H_

#include <cmath>
#include <experimental/filesystem>
#include <iostream>
#include <string>
#include <vector>

// opencv
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

// boost
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"

// because c++ 11 don't have make_unique, so we make one by myself.
template <typename T, typename... Args>
static std::unique_ptr<T> make_unique(Args &&...args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif  //_COMMON_H_