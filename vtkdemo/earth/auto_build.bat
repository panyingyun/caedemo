md .\build
pushd .\build
cmake -DVTK_DIR:PATH="D:\vtklib\vtk-8.2.0-static-20190304\lib\cmake\vtk-8.2" -G "Visual Studio 15 2017 Win64" ..
cmake --build . --config Release
popd
pause