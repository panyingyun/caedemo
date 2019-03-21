/*
 * Author: panyingyun(at)gmail.com
 *
 * Description: Demo for Study VTK
 *
 */

#include <vtkActor.h>
#include <vtkCellData.h>
#include <vtkColorTransferFunction.h>
#include <vtkDataArray.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkJPEGReader.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkTexture.h>
#include <vtkTextureMapToSphere.h>

int main(int argc, char* argv[]) {
    // Create SphereSource
    vtkNew<vtkSphereSource> source;
    source->SetRadius(0.5);
    source->SetPhiResolution(80);
    source->SetThetaResolution(80);

    // Create TextureMapToSphere
    vtkNew<vtkTextureMapToSphere> texturemap;
    texturemap->SetInputConnection(source->GetOutputPort());

    // Create Texture
    vtkNew<vtkJPEGReader> jPEGReader;
    jPEGReader->SetFileName("earth.jpg");
    vtkNew<vtkTexture> texture;
    texture->SetInputConnection(jPEGReader->GetOutputPort());

    // Create Data Set Mapper
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(texturemap->GetOutputPort());

    // Create Actor
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->SetTexture(texture);

    // Create Renderer and RenderWindow and WindowInteractor
    vtkNew<vtkRenderer> render;
    render->AddActor(actor);
    render->SetBackground(0.1, 0.2, 0.3);

    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(render);

    vtkNew<vtkRenderWindowInteractor> rwInteractor;
    vtkNew<vtkInteractorStyleTrackballCamera> style;
    rwInteractor->SetRenderWindow(renderWindow);
    rwInteractor->SetInteractorStyle(style);

    rwInteractor->Initialize();
    rwInteractor->Start();

    return 0;
}
