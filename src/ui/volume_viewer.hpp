
#ifndef UI__VOLUME_VIEWER_HPP
#define UI__VOLUME_VIEWER_HPP

#include <imgui.h>
#include <implot.h>
#include <vtkActor.h>
#include <vtkBoxWidget.h>
#include <vtkColorTransferFunction.h>
#include <vtkImageData.h>
#include <vtkImagePlaneWidget.h>
#include <vtkOutlineFilter.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkTransform.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>

#include <map>
#include <memory>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <tuple>
#include <vector>

#include "base_window.hpp"
#include "vtk_viewer.hpp"

namespace imn::ui {

class VolumeViewer : public BaseWindow, public VtkViewer {
   public:
    VolumeViewer();
    ~VolumeViewer();
    void set_volume(std::shared_ptr<cv::Mat> vol);
    void show() override;

   private:
    std::string _id;

    std::mutex _mutex;
    std::shared_ptr<cv::Mat> _vol;

    vtkSmartPointer<vtkImageData> _image_data;
    vtkSmartPointer<vtkVolume> _volume;
    vtkSmartPointer<vtkSmartVolumeMapper> _vol_mapper;

    vtkSmartPointer<vtkPiecewiseFunction> _opacity_tf;
    vtkSmartPointer<vtkColorTransferFunction> _color_tf;
    vtkSmartPointer<vtkTransform> _volume_transform;

    vtkSmartPointer<vtkOutlineFilter> _vol_outline_filter;
    vtkSmartPointer<vtkActor> _vol_outline;
    vtkSmartPointer<vtkPolyDataMapper> _vol_outline_mapper;
};

}  // namespace imn::ui
#endif
