mkdir build
cd build
cmake .. -G "Visual Studio 15 2017" -A Win32
mkdir out
MSBuild.exe Project.sln /p:Configuration=Release /p:Platform=Win32
cd Release
TestEntry.exe