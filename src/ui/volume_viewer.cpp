
#include "volume_viewer.hpp"

#include <imgui.h>
#include <implot.h>
#include <spdlog/spdlog.h>
#include <vtkPointData.h>
#include <vtkProperty.h>

#include <ranges>

#include "core/setting.hpp"
#include "include/def.hpp"
#include "ui/style.hpp"
#include "ui/widget/common_widgets.hpp"
#include "util/common.hpp"
#include "util/imgui_util.hpp"

using namespace imn::ui;

VolumeViewer::VolumeViewer() : BaseWindow(), VtkViewer() {
    _setup();
}

VolumeViewer::~VolumeViewer() {
}

void VolumeViewer::set_volume(std::shared_ptr<cv::Mat> vol) {
    std::lock_guard<std::mutex> lock(_mutex);
    _mat = vol;

    auto shape = vol->size;
    auto image_data = vtkNew<vtkImageData>();
    image_data->GetPointData()->SetScalars(get_vtk_view(vol.get()));
    image_data->SetDimensions(shape[2], shape[1], shape[0]);

    _vol_mapper->SetInputData(image_data);
    _vol_outline_filter->SetInputData(image_data);
}

void VolumeViewer::set_colormap(std::vector<std::pair<float, ImColor>> colormap) {
    std::lock_guard<std::mutex> lock(_mutex);
    double min_v, max_v;
    cv::minMaxLoc(*_mat, &min_v, &max_v);

    _color_tf->RemoveAllPoints();
    _opacity_tf->RemoveAllPoints();

    // leftmost color has higher priority
    for (auto& [fraction, col] : std::ranges::views::reverse(colormap)) {
        // min_v   v        max_v
        //   |-----+----------|
        //   0                1
        auto v = fraction * (max_v - min_v) + min_v;
        _color_tf->AddRGBPoint(v, col.Value.x, col.Value.y, col.Value.z);
        _opacity_tf->AddPoint(v, col.Value.w);
    }

    _color_tf->Modified();
    _opacity_tf->Modified();
}

void VolumeViewer::show() {
    std::lock_guard<std::mutex> lock(_mutex);
    // ImGui::Begin("Vtk", nullptr, VtkViewer::NoScrollFlags());
    render();
    // ImGui::End();
}

void VolumeViewer::_setup() {
    _vol_mapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
    _color_tf = vtkSmartPointer<vtkColorTransferFunction>::New();
    _opacity_tf = vtkSmartPointer<vtkPiecewiseFunction>::New();
    _volume = vtkSmartPointer<vtkVolume>::New();
    _volume_transform = vtkSmartPointer<vtkTransform>::New();

    _vol_outline = vtkSmartPointer<vtkActor>::New();
    _vol_outline_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    _vol_outline_filter = vtkSmartPointer<vtkOutlineFilter>::New();

    // https://discourse.vtk.org/t/how-to-display-volume-rendering-of-large-volume-images/10375
    _vol_mapper->SetCroppingRegionFlags(VTK_CROP_SUBVOLUME);
    // _vol_mapper->SetPartitions(8, 8, 8);
    // _vol_mapper->SetSampleDistance(0.01);
    _vol_mapper->SetBlendMode(vtkVolumeMapper::MAXIMUM_INTENSITY_BLEND);

    auto volume_property = vtkSmartPointer<vtkVolumeProperty>::New();
    volume_property->SetInterpolationTypeToLinear();
    // volume_property->ShadeOn();  // TODO: check me
    // volume_property->SetAmbient(0.4);
    // volume_property->SetDiffuse(0.6);
    // volume_property->SetSpecular(0.2);
    // volume_property->SetSpecularPower(1);
    volume_property->SetColor(0, _color_tf);
    volume_property->SetScalarOpacity(0, _opacity_tf);
    volume_property->SetScalarOpacityUnitDistance(1);

    _volume->SetMapper(_vol_mapper);
    _volume->SetProperty(volume_property);
    _volume->SetUserTransform(_volume_transform);
    // _volume->SetVisibility(false);

    _vol_outline_mapper->SetInputConnection(_vol_outline_filter->GetOutputPort());
    _vol_outline->SetMapper(_vol_outline_mapper);
    _vol_outline->GetProperty()->SetColor(0.5, 0.5, 0.5);
}
