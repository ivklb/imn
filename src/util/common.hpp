
#ifndef UTIL__COMMON_HPP
#define UTIL__COMMON_HPP

#include <vtkDataArray.h>

#include <opencv2/opencv.hpp>
#include <string>
#include <tuple>

int get_unique_id();

std::tuple<int, int> get_screen_resolution();

vtkSmartPointer<vtkDataArray> get_vtk_view(cv::Mat* data);

#endif
