
#ifndef UI__VOLUME_VIEWER_HPP
#define UI__VOLUME_VIEWER_HPP

#include <imgui.h>
#include <implot.h>
#include <vtkActor.h>
#include <vtkBoxWidget.h>
#include <vtkColorTransferFunction.h>
#include <vtkImagePlaneWidget.h>
#include <vtkOpenGLGPUVolumeRayCastMapper.h>
#include <vtkOutlineFilter.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPolyDataMapper.h>
#include <vtkTransform.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>

#include <map>
#include <memory>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <tuple>
#include <utility>
#include <vector>

#include "ui/base_window.hpp"
#include "ui/widgets/vtk_viewer.hpp"

namespace imn::ui {

class VolumeWidget : public BaseWidget, public VtkViewer {
   public:
    VolumeWidget();
    ~VolumeWidget();
    void set_volume(std::shared_ptr<cv::Mat> vol);
    void set_colormap(std::vector<std::pair<float, ImColor>> colormap);
    void show(const ImVec2 size = ImVec2(0, 0)) override;

   private:
    void _setup();

    std::mutex _mutex;
    std::shared_ptr<cv::Mat> _mat;

    vtkSmartPointer<vtkVolume> _volume;
    vtkSmartPointer<vtkOpenGLGPUVolumeRayCastMapper> _vol_mapper;
    vtkSmartPointer<vtkColorTransferFunction> _color_tf;
    vtkSmartPointer<vtkPiecewiseFunction> _opacity_tf;
    vtkSmartPointer<vtkTransform> _volume_transform;
    vtkSmartPointer<vtkOutlineFilter> _vol_outline_filter;
    vtkSmartPointer<vtkPolyDataMapper> _vol_outline_mapper;
    vtkSmartPointer<vtkActor> _vol_outline;
};

}  // namespace imn::ui
#endif
