cmake_minimum_required(VERSION 3.16)

project(Kvantum)

set(CMAKE_CXX_STANDARD 17)

# Find Qt from Conan - this provides Qt5::Core, Qt5::Widgets, Qt5::Svg, etc.
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Svg Gui)

message(STATUS "Qt5 found via Conan")
message(STATUS "Qt5 installation dir: ${Qt5_DIR}")

# Set Qt paths based on build type (following retargeter pattern)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(Qt_Plugin_Path ${qt_PACKAGE_FOLDER_DEBUG}/bin/archdatadir/plugins)
    set(Qt_Lib_Path ${qt_PACKAGE_FOLDER_DEBUG}/lib)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(Qt_Plugin_Path ${qt_PACKAGE_FOLDER_RELWITHDEBINFO}/bin/archdatadir/plugins)
    set(Qt_Lib_Path ${qt_PACKAGE_FOLDER_RELWITHDEBINFO}/lib)
else()
    set(Qt_Plugin_Path ${qt_PACKAGE_FOLDER_RELEASE}/bin/archdatadir/plugins)
    set(Qt_Lib_Path ${qt_PACKAGE_FOLDER_RELEASE}/lib)
endif()

message(STATUS "Qt Plugin Path: ${Qt_Plugin_Path}")
message(STATUS "Qt Lib Path: ${Qt_Lib_Path}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Using GLOB_RECURSE with CONFIGURE_DEPENDS (CMake 3.12+)
# This tells CMake to check for file changes at build time
file(GLOB_RECURSE KVANTUM_SOURCES CONFIGURE_DEPENDS 
    src/*.cpp
)
file(GLOB_RECURSE KVANTUM_HEADERS CONFIGURE_DEPENDS 
    src/*.h 
    include/*.h
)
# Define only the themes we want to include
set(KVANTUM_THEMES
    bin/themes/kvantum/Blossom/Blossom.qrc
    bin/themes/kvantum/Charcoal/Charcoal.qrc
    bin/themes/kvantum/Adapta/Adapta.qrc
    bin/themes/kvantum/AdaptaNokto/AdaptaNokto.qrc
    bin/themes/kvantum/FTIDark/FTIDark.qrc
    bin/themes/kvantum/FTILight/FTILight.qrc
    bin/themes/kvantum/KvAdapta/KvAdapta.qrc
    bin/themes/kvantum/KvAdaptaDark/KvAdaptaDark.qrc
    bin/themes/kvantum/KvAmbiance/KvAmbiance.qrc
    bin/themes/kvantum/KvArc/KvArc.qrc
    bin/themes/kvantum/KvArcDark/KvArcDark.qrc
    bin/themes/kvantum/KvGnome/KvGnome.qrc
    bin/themes/kvantum/KvGnomeAlt/KvGnomeAlt.qrc
    bin/themes/kvantum/KvGnomeDark/KvGnomeDark.qrc
    bin/themes/kvantum/KvOxygen/KvOxygen.qrc
    bin/themes/kvantum/KvSimplicity/KvSimplicity.qrc
    bin/themes/kvantum/KvSimplicityDark/KvSimplicityDark.qrc
    bin/themes/kvantum/Retrosmart/Retrosmart.qrc
)

add_library(Kvantum STATIC
    ${KVANTUM_SOURCES}
    ${KVANTUM_HEADERS}
    ${KVANTUM_THEMES}
    default/defaulttheme.qrc)

target_link_libraries(Kvantum Qt5::Widgets Qt5::Core Qt5::Svg)
target_include_directories(Kvantum PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src")
target_include_directories(Kvantum PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include")
target_compile_definitions(Kvantum PRIVATE DATADIR)

add_executable(Test
    ThemeTest/main.cpp
    ThemeTest/mainwindow.cpp
    ThemeTest/mainwindow.h
    ThemeTest/mainwindow.ui
    ThemeTest/ThemeTest.qrc)

# Link with Kvantum static library and Qt libraries
target_link_libraries(Test 
    Kvantum 
    Qt5::Widgets 
    Qt5::Gui
    Qt5::Core 
    Qt5::Svg)

# Link Qt support libraries BEFORE platform plugin (order matters!)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_libraries(Test 
        # Qt support libraries first
        ${Qt_Lib_Path}/Qt5FontDatabaseSupportd.lib
        ${Qt_Lib_Path}/Qt5EventDispatcherSupportd.lib
        ${Qt_Lib_Path}/Qt5ThemeSupportd.lib
        ${Qt_Lib_Path}/Qt5WindowsUIAutomationSupportd.lib
        ${Qt_Lib_Path}/Qt5AccessibilitySupportd.lib
        # Platform plugin after support libraries
        ${Qt_Plugin_Path}/platforms/qwindowsd.lib
        ${Qt_Plugin_Path}/styles/qwindowsvistastyled.lib)
else()
    target_link_libraries(Test 
        # Qt support libraries first
        ${Qt_Lib_Path}/Qt5FontDatabaseSupport.lib
        ${Qt_Lib_Path}/Qt5EventDispatcherSupport.lib
        ${Qt_Lib_Path}/Qt5ThemeSupport.lib
        ${Qt_Lib_Path}/Qt5WindowsUIAutomationSupport.lib
        ${Qt_Lib_Path}/Qt5AccessibilitySupport.lib
        # Platform plugin after support libraries
        ${Qt_Plugin_Path}/platforms/qwindows.lib
        ${Qt_Plugin_Path}/styles/qwindowsvistastyle.lib)
endif()

# Windows-specific libraries needed for Qt (exact same as retargeter)
if(WIN32)
    target_link_libraries(Test 
        imm32.lib
        winmm.lib
        ws2_32.lib
        Version.lib
        Netapi32.lib
        Userenv.lib
        Iphlpapi.lib
        Crypt32.lib
        Dwmapi.lib
        Wtsapi32.lib
        # Additional Windows libs that might be needed
        kernel32.lib
        user32.lib
        shell32.lib
        ole32.lib
        oleaut32.lib
        uuid.lib
        advapi32.lib
        gdi32.lib
        winspool.lib
        comdlg32.lib)
endif()

target_include_directories(Test PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include")

# No installation needed - Conan handles packaging directly

message("-- Qt version ${Qt5Core_VERSION_STRING} found via Conan")