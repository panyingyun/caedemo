#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkCellData.h>
#include <vtkColorTransferFunction.h>
#include <vtkContourFilter.h>
#include <vtkDataArray.h>
#include <vtkDataSetMapper.h>
#include <vtkDataSetTriangleFilter.h>
#include <vtkDoubleArray.h>
#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkGeometryFilter.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLine.h>
#include <vtkOpenGLProjectedTetrahedraMapper.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPlaneSource.h>
#include <vtkPointData.h>
#include <vtkPointSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataWriter.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkStreamTracer.h>
#include <vtkStructuredPointsReader.h>
#include <vtkThreshold.h>
#include <vtkTubeFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkVolumeProperty.h>
#include <vtkWarpVector.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLUnstructuredGridReader.h>
#ifdef BUILD_VR
#include <vtkOpenVRRenderWindow.h>
#include <vtkOpenVRRenderWindowInteractor.h>
#include <vtkOpenVRRenderer.h>
#endif

int main(int argc, char* argv[]) {
    // Create Point Position
    double X[3] = {1.0, 0.0, 0.0};
    double Y[3] = {0.0, 0.0, 1.0};
    double Z[3] = {0.0, 0.0, 0.0};

    // PID
    vtkIdType PID[3];
    // add pointdata and cell data
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();

    for (unsigned int i = 0; i < 3; i++) {
        PID[i] = points->InsertNextPoint(X[i], Y[i], Z[i]);
        std::cout << PID[i] << std::endl;
    }

    vtkSmartPointer<vtkLine> line0 = vtkSmartPointer<vtkLine>::New();
    line0->GetPointIds()->SetId(0, PID[0]);
    line0->GetPointIds()->SetId(1, PID[1]);

    vtkSmartPointer<vtkLine> line1 = vtkSmartPointer<vtkLine>::New();
    line1->GetPointIds()->SetId(0, PID[0]);
    line1->GetPointIds()->SetId(1, PID[2]);

    vtkSmartPointer<vtkLine> line2 = vtkSmartPointer<vtkLine>::New();
    line2->GetPointIds()->SetId(0, PID[1]);
    line2->GetPointIds()->SetId(1, PID[2]);

    lines->InsertNextCell(line0);
    lines->InsertNextCell(line1);
    lines->InsertNextCell(line2);
    //创建vtkPolyData类型的数据
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    polydata->SetPoints(points);
    polydata->SetLines(lines);

    // add weight Data Array
    vtkSmartPointer<vtkDoubleArray> weights = vtkSmartPointer<vtkDoubleArray>::New();

    auto nc = weights->GetNumberOfComponents();
    auto nt = weights->GetNumberOfTuples();
    auto nv = weights->GetNumberOfValues();

    std::cout << "before nc = " << nc << std::endl;
    std::cout << "before nt = " << nt << std::endl;
    std::cout << "before nv = " << nv << std::endl;

    weights->SetNumberOfComponents(1);
    weights->InsertTuple1(0, 0.1);
    weights->InsertTuple1(1, 0.5);
    weights->InsertTuple1(2, 0.9);
    weights->SetName("weights");

    nc = weights->GetNumberOfComponents();
    nt = weights->GetNumberOfTuples();
    nv = weights->GetNumberOfValues();
    std::cout << "after nc = " << nc << std::endl;
    std::cout << "after nt = " << nt << std::endl;
    std::cout << "after nv = " << nv << std::endl;

    // add RGB Data Array
    vtkSmartPointer<vtkDoubleArray> rgb = vtkSmartPointer<vtkDoubleArray>::New();

    nc = rgb->GetNumberOfComponents();
    nt = rgb->GetNumberOfTuples();
    nv = rgb->GetNumberOfValues();

    std::cout << "nc = " << nc << std::endl;
    std::cout << "nt = " << nt << std::endl;
    std::cout << "nv = " << nv << std::endl;

    rgb->SetNumberOfComponents(2);
    rgb->InsertTuple2(0, 0.1, 9.0);
    rgb->InsertTuple2(1, 0.5, 5.0);
    rgb->InsertTuple2(2, 0.9, 1.0);
    rgb->SetName("rgb");

    nc = rgb->GetNumberOfComponents();
    nt = rgb->GetNumberOfTuples();
    nv = rgb->GetNumberOfValues();
    std::cout << "nc = " << nc << std::endl;
    std::cout << "nt = " << nt << std::endl;
    std::cout << "nv = " << nv << std::endl;

    polydata->GetPointData()->SetScalars(weights);
    //polydata->GetPointData()->AddArray(rgb);

    polydata->GetCellData()->AddArray(weights);

    //属性数据的访问
	//访问数据方式有两种：
	//1、通过元组下标进行访问
	//2、通过GetComponent进行访问

    //vtkDoubleArray* rgbarray =
    //    vtkDoubleArray::SafeDownCast(polydata->GetPointData()->GetArray("rgb"));
    //for (int i = 0; i < rgbarray->GetNumberOfTuples(); i++) {
    //    auto w = rgbarray->GetTuple2(i);
    //    std::cout << "w[" << i << "] = " << w[0] << " , " << w[1] << std::endl;
    //    auto w0 = rgbarray->GetComponent(i, 0);
    //    auto w1 = rgbarray->GetComponent(i, 1);
    //    std::cout << "w0 = " << w0 << std::endl;
    //    std::cout << "w1 = " << w1 << std::endl;
    //}

    // write point to vtkPolyData
    vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
    writer->SetFileName("E:\\michaelproject\\refactor\\vtkstudy\\ch01\\abc.vtk");
    writer->SetInputData(polydata);
    writer->Write();
}
