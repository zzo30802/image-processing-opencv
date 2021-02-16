git clean -fdx
cmake ../ -G "Visual Studio 15 2017" -A x64
cmake --build . --config Release

ECHO %THIRD_PARTY%
COPY  %THIRD_PARTY%\opencv\build\x64\vc15\bin\opencv_world349.dll  %~dp0\bin\Release
