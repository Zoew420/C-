mkdir build
cd build
cmake ..
ls ./
where MSBuild
MSBuild.exe Project.sln /p:OutputPath=./
ls ./
ls ./Debug/
TestEntry.exe