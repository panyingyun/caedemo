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
#include <vtkDataSetMapper.h>
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

static vtkSmartPointer<vtkColorTransferFunction> generateColorMap(double range[2]) {
    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction =
        vtkSmartPointer<vtkColorTransferFunction>::New();

    auto delta = range[1] - range[0];
    colorTransferFunction->SetColorSpaceToDiverging();
    colorTransferFunction->AddRGBPoint(range[0], 79.0 / 255.0, 73.0 / 255.0, 207.0 / 255.0);
    colorTransferFunction->AddRGBPoint(range[0] + delta * 1.0 / 7.0, 8.0 / 255.0, 3.0 / 255.0,
                                       97.0 / 255.0);
    colorTransferFunction->AddRGBPoint(range[0] + delta * 2.0 / 7.0, 0.0, 248.0 / 255.0,
                                       249.0 / 255.0);
    colorTransferFunction->AddRGBPoint(range[0] + delta * 3.0 / 7.0, 8.0 / 255.0, 135.0 / 255.0,
                                       2.0 / 255.0);
    colorTransferFunction->AddRGBPoint(range[0] + delta * 4.0 / 7.0, 1.0, 1.0, 8.0 / 255.0);
    colorTransferFunction->AddRGBPoint(range[0] + delta * 5.0 / 7.0, 252.0 / 255.0, 98.0 / 255.0,
                                       4.0 / 255.0);
    colorTransferFunction->AddRGBPoint(range[0] + delta * 6.0 / 7.0, 105.0 / 255.0, 0.0, 0.0);
    colorTransferFunction->AddRGBPoint(range[1], 216.0 / 255.0, 84.0 / 255.0, 0.086);
    return colorTransferFunction;
}

int main(int argc, char* argv[]) {
    // Reader wind dataset
    vtkNew<vtkStructuredPointsReader> reader;
    reader->SetFileName("wind.vtk");
    reader->Update();

    // Set ActiveVectors
    auto grid = reader->GetOutput();
    grid->GetPointData()->SetActiveVectors("wind_velocity");

    // Create Color Map
    auto modelArray = reader->GetOutput()->GetPointData()->GetArray("wind_speed");
    double dataRange[2];
    modelArray->GetRange(dataRange, -1);
    auto modelcolormap = generateColorMap(dataRange);
    std::cout << "dataRange[0]= " << dataRange[0] << std::endl;
    std::cout << "dataRange[1]= " << dataRange[1] << std::endl;

    // Create Seed Source
    vtkNew<vtkPlaneSource> planeSource;
    planeSource->SetOrigin(17.5, 100.0, 0.0);
    planeSource->SetPoint1(17.5, 100.0, 16.0);
    planeSource->SetPoint2(60, 100.0, 0.0);
    planeSource->SetXResolution(2);
    planeSource->SetYResolution(100);

    // Create StreamTracer
    vtkNew<vtkStreamTracer> streamTracer;
    streamTracer->SetInputConnection(reader->GetOutputPort());
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
    auto tubeArray = streamTube->GetOutput()->GetPointData()->GetArray("wind_velocity");
    double tubedataRange[2];
    tubeArray->GetRange(tubedataRange, -1);
    auto tubecolormap = generateColorMap(tubedataRange);

    // Model Mapper
    vtkNew<vtkDataSetMapper> modelmapper;
    modelmapper->SetInputConnection(reader->GetOutputPort());
    modelmapper->SetLookupTable(modelcolormap);
    modelmapper->SetScalarModeToUsePointData();
    modelmapper->SelectColorArray("wind_speed");
    modelmapper->SetScalarRange(dataRange[0], dataRange[1]);

    // Stream Mapper
    vtkNew<vtkPolyDataMapper> streammapper;
    streammapper->SetInputConnection(streamTube->GetOutputPort());
    streammapper->SetLookupTable(tubecolormap);
    streammapper->SelectColorArray("wind_velocity");
    streammapper->SetScalarModeToUsePointData();
    streammapper->SetScalarRange(tubedataRange[0], tubedataRange[1]);

    //  model Actors
    vtkNew<vtkActor> modelactor;
    modelactor->SetMapper(modelmapper);
    modelactor->GetProperty()->SetRepresentationToWireframe();
    modelactor->GetProperty()->SetOpacity(0.0);
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
