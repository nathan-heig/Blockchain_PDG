cmake_minimum_required(VERSION 3.16)

project(Blockchain_desktop_PDG VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick QuickControls2 Qml Test Core5Compat)
find_package(Boost REQUIRED COMPONENTS system)

add_subdirectory(../Blockchain ${CMAKE_CURRENT_BINARY_DIR}/blockchain_core_build)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(appBlockchain_desktop_PDG
    src/main.cpp
    src/resources.qrc
)

qt_add_qml_module(appBlockchain_desktop_PDG
    URI Blockchain_desktop_PDG
    VERSION 1.0
    QML_FILES
        qml/Main.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appBlockchain_desktop_PDG PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appBlockchain_desktop_PDG
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appBlockchain_desktop_PDG
    PRIVATE Qt6::Quick Qt6::QuickControls2 Qt6::Qml Qt6::Core5Compat blockchain_core Boost::system
)

include(GNUInstallDirs)
install(TARGETS appBlockchain_desktop_PDG
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

enable_testing()
add_executable(test_basic tests/test_basic.cpp)
target_link_libraries(test_basic PRIVATE Qt6::Test Qt6::Core)
add_test(NAME basic COMMAND test_basic)
