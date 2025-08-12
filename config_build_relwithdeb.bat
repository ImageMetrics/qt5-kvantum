@ECHO OFF

set BASEDIR=%~dp0
PUSHD %BASEDIR%

set BUILD_TYPE=RelWithDebInfo

echo Cleaning previous build directory...
RMDIR /Q /S build

echo Detecting Conan profile and installing dependencies...
conan profile detect --force
conan install . -s compiler.cppstd=17 --output-folder=./ --build=missing --settings=build_type=%BUILD_TYPE%
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: Conan install failed
    exit /b %ERRORLEVEL%
)

echo Configuring CMake with preset default...
cmake -DCMAKE_BUILD_TYPE=%BUILD_TYPE% --preset default
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake configuration failed
    exit /b %ERRORLEVEL%
)

echo Building qt5-kvantum RelWithDebInfo configuration...
echo.

REM Check if the solution exists
if not exist "build\Kvantum.sln" (
    echo ERROR: Solution file not found at build\Kvantum.sln
    echo CMake configuration may have failed.
    exit /b 1
)

REM Build RelWithDebInfo configuration
cmake --build build --config RelWithDebInfo
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: RelWithDebInfo build failed
    exit /b %ERRORLEVEL%
)

echo.
echo Build completed successfully!
echo Static library: %BASEDIR%build\RelWithDebInfo\Kvantum.lib
echo Test executable: %BASEDIR%build\RelWithDebInfo\Test.exe
echo.

POPD