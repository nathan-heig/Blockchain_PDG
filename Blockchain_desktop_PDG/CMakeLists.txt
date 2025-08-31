cmake_minimum_required(VERSION 3.16)
project(CryptoWallet VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Quick QuickControls2 Qml Core5Compat Test)
find_package(Boost REQUIRED COMPONENTS system)
add_subdirectory(../Blockchain ${CMAKE_CURRENT_BINARY_DIR}/blockchain_core_build)

qt_standard_project_setup(REQUIRES 6.5)

# Configurer les ressources QRC
set(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/resources.qrc)
qt6_add_resources(RESOURCE_FILES ${RESOURCES})

qt_add_executable(appBlockchain_desktop_PDG
    src/main.cpp
    ${RESOURCE_FILES}
)

# Si vous avez des classes C++ backend, ajoutez-les ici
# target_sources(appBlockchain_desktop_PDG PRIVATE
#     WalletBackend.cpp
#     WalletBackend.h
#     TransactionModel.cpp
#     TransactionModel.h
#     MiningController.cpp
#     MiningController.h
# )

target_link_libraries(appBlockchain_desktop_PDG
    PRIVATE Qt6::Core
    PRIVATE Qt6::Quick
    PRIVATE Qt6::QuickControls2
    PRIVATE Qt6::Qml
    PRIVATE Qt6::Core5Compat
)

set_target_properties(appBlockchain_desktop_PDG PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.cryptowallet.app
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Installation rules for deployment
target_link_libraries(appBlockchain_desktop_PDG
    PRIVATE Qt6::Quick Qt6::QuickControls2 Qt6::Qml Qt6::Core5Compat blockchain_core Boost::system
)

include(GNUInstallDirs)
install(TARGETS appBlockchain_desktop_PDG
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Configure QML module path for deployment tools
set(QML_IMPORT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/qml CACHE STRING "QML import path" FORCE)

# ============== TESTS SECTION ==============
# Build tests only if we're not cross-compiling
if(NOT CMAKE_CROSSCOMPILING)
    # Basic test executable
    qt_add_executable(test_basic
        tests/test_basic.cpp
    )

    target_link_libraries(test_basic
        PRIVATE Qt6::Test
        PRIVATE Qt6::Core
    )

    # Register the test with CTest
    add_test(NAME test_basic COMMAND test_basic)

    # Set test properties for better output
    set_tests_properties(test_basic PROPERTIES
        TIMEOUT 10
        ENVIRONMENT "QT_LOGGING_RULES=*.debug=false"
    )

    # Add more tests here as needed
    # qt_add_executable(test_wallet
    #     tests/test_wallet.cpp
    # )
    # target_link_libraries(test_wallet
    #     PRIVATE Qt6::Test
    #     PRIVATE Qt6::Core
    #     # Link to your backend classes if testing them
    # )
    # add_test(NAME test_wallet COMMAND test_wallet)
endif()

# ============== DEPLOYMENT HELPERS ==============
# Create a custom target to help with deployment debugging
if(WIN32)
    add_custom_command(TARGET appBlockchain_desktop_PDG POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Build completed for Windows. Executable at: $<TARGET_FILE:appBlockchain_desktop_PDG>"
        COMMAND ${CMAKE_COMMAND} -E echo "QML files should be embedded in resources.qrc"
    )
elseif(APPLE)
    add_custom_command(TARGET appBlockchain_desktop_PDG POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Build completed for macOS. Bundle at: $<TARGET_BUNDLE_DIR:appBlockchain_desktop_PDG>"
    )
else()
    add_custom_command(TARGET appBlockchain_desktop_PDG POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Build completed for Linux. Executable at: $<TARGET_FILE:appBlockchain_desktop_PDG>"
    )
endif()

# Print configuration summary
message(STATUS "========================================")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Qt Version: ${Qt6_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Testing Enabled: YES")
message(STATUS "QML Import Path: ${QML_IMPORT_PATH}")
message(STATUS "========================================")
