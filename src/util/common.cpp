
#include "common.hpp"

#include <vtkFloatArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkUnsignedIntArray.h>
#include <vtkUnsignedShortArray.h>
#include <wtypes.h>

int get_unique_id() {
    static int id = 0;
    return id++;
}

std::tuple<int, int> get_screen_resolution() {
    // SetProcessDPIAware();
    const HWND hDesktop = GetDesktopWindow();

    RECT desktop;
    GetWindowRect(hDesktop, &desktop);
    // The top left corner will have coordinates (0,0)
    // and the bottom right corner will have coordinates
    // (horizontal, vertical)
    return {desktop.right, desktop.bottom};
}

vtkSmartPointer<vtkDataArray> get_vtk_view(cv::Mat* mat) {
    vtkSmartPointer<vtkDataArray> rv;
    auto dtype = mat->type();
    if (dtype == CV_8UC1) {
        rv = vtkNew<vtkUnsignedCharArray>();
    } else if (dtype == CV_16UC1) {
        rv = vtkNew<vtkUnsignedShortArray>();
    } else if (dtype == CV_32SC1) {
        rv = vtkNew<vtkUnsignedIntArray>();
    } else if (dtype == CV_32FC1) {
        rv = vtkNew<vtkFloatArray>();
    } else {
        throw std::runtime_error("Unsupported data type");
    }
    int save = 1;  // do not delete buffer when array is deleted
    rv->SetVoidArray(mat->data, mat->total(), save);
    return rv;
}