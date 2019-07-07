mkdir build
cd build
cmake ..
ls ./
mkdir out
MSBuild.exe Project.sln /p:OutDir=out
cd out
ls ./
TestEntry.exe