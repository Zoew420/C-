mkdir build
cd build
cmake ..
mkdir out
MSBuild.exe Project.sln /p:OutDir=out
cd out
TestEntry.exe