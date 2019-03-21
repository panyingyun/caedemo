/*
 * Author: panyingyun(at)gmail.com
 *
 * Description: Demo for Study VTK
 *
 */

#include <cctype>
#include <filesystem>
#include <iostream>

#include <vtkAssembly.h>
#include <vtkCellData.h>
#include <vtkColorTransferFunction.h>
#include <vtkGeometryFilter.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNew.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStreamTracer.h>
#include <vtkStructuredPoints.h>
#include <vtkStructuredPointsReader.h>
#include <vtkTubeFilter.h>
#include <vtkUnstructuredGrid.h>

int main(int argc, char* argv[]) {
    // Reader wind dataset
    vtkNew<vtkStructuredPointsReader> reader;
    reader->SetFileName("wind.vtk");

    // Convert to ploydata
    vtkSmartPointer<vtkGeometryFilter> geometryFilter = vtkSmartPointer<vtkGeometryFilter>::New();
    geometryFilter->SetInputConnection(reader->GetOutputPort());
    geometryFilter->Update();

    // Set ActiveVectors
    auto grid = geometryFilter->GetOutput();
    grid->GetPointData()->SetActiveVectors("wind_velocity");

    // Create Seed Source
    vtkNew<vtkPlaneSource> planeSource;
    planeSource->SetOrigin(17.5, 100.0, 0.0);
    planeSource->SetPoint1(17.5, 100.0, 16.0);
    planeSource->SetPoint2(60, 100.0, 0.0);
    planeSource->SetXResolution(1);
    planeSource->SetYResolution(100);

    // Create StreamTracer
    vtkNew<vtkStreamTracer> streamTracer;
    streamTracer->SetInputConnection(geometryFilter->GetOutputPort());
    streamTracer->SetSourceConnection(planeSource->GetOutputPort());
    streamTracer->SetIntegrationDirectionToBoth();
    streamTracer->SetIntegratorTypeToRungeKutta45();
    streamTracer->SetMaximumPropagation(60);
    streamTracer->SetComputeVorticity(1);

    // Line To Tube
    vtkNew<vtkTubeFilter> streamTube;
    streamTube->SetInputConnection(streamTracer->GetOutputPort());
    streamTube->SetRadius(0.08);
    streamTube->Update();

    // Create Color Map
    auto streamTubeArray = grid->GetPointData()->GetArray("wind_velocity");

    double dataRange[2];
    streamTubeArray->GetRange(dataRange, -1);

    std::cout << "dataRange[0]= " << dataRange[0] << std::endl;
    std::cout << "dataRange[1]= " << dataRange[1] << std::endl;

    auto delta = dataRange[1] - dataRange[0];
    vtkNew<vtkColorTransferFunction> colorTransferFunction;
    colorTransferFunction->SetColorSpaceToDiverging();
    colorTransferFunction->AddRGBPoint(dataRange[0], 0.0, 4.0 / 255.0, 253.0 / 255.0);
    colorTransferFunction->AddRGBPoint(dataRange[0] + delta * 1.0 / 6.0, 0.0, 165.0 / 255.0,
                                       251.0 / 255.0);
    colorTransferFunction->AddRGBPoint(dataRange[0] + delta * 2.0 / 6.0, 5.0 / 255.0, 253.0 / 255.0,
                                       174.0 / 255.0);
    colorTransferFunction->AddRGBPoint(dataRange[0] + delta * 3.0 / 6.0, 2.0 / 255.0, 1.0, 0.0);
    colorTransferFunction->AddRGBPoint(dataRange[0] + delta * 4.0 / 6.0, 172.0 / 255.0, 1.0,
                                       1.0 / 255.0);
    colorTransferFunction->AddRGBPoint(dataRange[0] + delta * 5.0 / 6.0, 251.0 / 255.0,
                                       176.0 / 255.0, 0.0);
    colorTransferFunction->AddRGBPoint(dataRange[1], 210.0 / 255.0, 20.0 / 255.0, 12.0 / 255.0);

    streamTube->GetOutput()->GetPointData()->SetScalars(
        colorTransferFunction->MapScalars(streamTubeArray, VTK_COLOR_MODE_DEFAULT, 1));

    // Model Mapper
    vtkNew<vtkPolyDataMapper> modelmapper;
    modelmapper->SetInputConnection(geometryFilter->GetOutputPort());
    modelmapper->SetLookupTable(colorTransferFunction);
    modelmapper->SetScalarModeToUsePointData();
    modelmapper->SelectColorArray("wind_velocity");
    modelmapper->SetScalarRange(dataRange[0], dataRange[1]);

    // Stream Mapper
    vtkNew<vtkPolyDataMapper> streammapper;
    streammapper->SetInputConnection(streamTube->GetOutputPort());

    //  model Actors
    vtkNew<vtkActor> modelactor;
    modelactor->SetMapper(modelmapper);
    modelactor->GetProperty()->SetOpacity(0.35);
    modelactor->GetProperty()->SetRepresentationToSurface();

    //  stream Actors
    vtkNew<vtkActor> streamactor;
    streamactor->SetMapper(streammapper);

    // Render
    vtkNew<vtkRenderer> render;
    vtkNew<vtkAssembly> assembly;
    assembly->AddPart(modelactor);
    assembly->AddPart(streamactor);
    render->AddActor(assembly);
    render->SetBackground(0.1, 0.2, 0.3);

    // Render Window
    vtkNew<vtkRenderWindow> renderWin;
    renderWin->AddRenderer(render);
    renderWin->SetSize(600, 600);

    // Render WindowInteractor
    vtkNew<vtkRenderWindowInteractor> renderWinIneractor;
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    renderWinIneractor->SetInteractorStyle(style);
    renderWinIneractor->SetRenderWindow(renderWin);
    renderWinIneractor->Initialize();
    renderWinIneractor->Start();

    return 0;
}
