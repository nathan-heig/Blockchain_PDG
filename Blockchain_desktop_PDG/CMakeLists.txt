cmake_minimum_required(VERSION 3.16)

# Set modern CMake policies
if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW) # Prefer Config mode for FindBoost
endif()

# ================== PROJECT ==================
project(BlockchainSuite VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt automations for MOC/UIC/RCC - CRITICAL pour Windows
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(BUILD_CLI "Build the blockchain_cli executable" ON)
option(BUILD_TESTS "Build tests" ON)
option(USE_SYSTEM_MINIUPNPC "Prefer system miniupnpc discovery" ON)

if(MSVC)
  add_compile_options(/permissive- /Zc:__cplusplus)
endif()

# ================== DEPENDENCIES ==================
find_package(Qt6 6.4 REQUIRED COMPONENTS Core Quick QuickControls2 Qml Core5Compat Test)
qt_standard_project_setup(REQUIRES 6.4)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# Boost detection - prefer vcpkg CONFIG mode
find_package(Boost CONFIG REQUIRED COMPONENTS system)
if(TARGET Boost::system)
  message(STATUS "Using Boost::system from vcpkg CONFIG")
else()
  message(WARNING "Boost::system target not found in CONFIG mode, trying legacy FindBoost")
  find_package(Boost REQUIRED COMPONENTS system)
endif()

# -------- miniupnpc detection --------
find_package(miniupnpc CONFIG QUIET)

set(MINIUPNPC_TARGET "")
set(MINIUPNPC_INC "")

# Si on a une config CMake (vcpkg) : utiliser la cible importée
if(miniupnpc_FOUND)
  set(MINIUPNPC_TARGET miniupnpc::miniupnpc)
  message(STATUS "Found miniupnpc via CONFIG")
else()
  # Sinon, si on souhaite privilégier le système
  if(USE_SYSTEM_MINIUPNPC)
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
      pkg_check_modules(MINIUPNPC QUIET miniupnpc)
      if(MINIUPNPC_FOUND)
        set(MINIUPNPC_TARGET ${MINIUPNPC_LINK_LIBRARIES})
        set(MINIUPNPC_INC ${MINIUPNPC_INCLUDE_DIRS})
      endif()
    endif()

    if(NOT MINIUPNPC_FOUND)
      find_library(MINIUPNPC_LIB_PATH NAMES miniupnpc upnpc
              PATHS
              /usr/lib /usr/local/lib /opt/local/lib
              /opt/homebrew/lib /usr/local/opt/miniupnpc/lib
              $ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib
      )
      find_path(MINIUPNPC_INC miniupnpc/miniwget.h
              PATHS
              /usr/include /usr/local/include /opt/local/include
              /opt/homebrew/include /usr/local/opt/miniupnpc/include
              $ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include
      )
      if(MINIUPNPC_LIB_PATH)
        set(MINIUPNPC_TARGET ${MINIUPNPC_LIB_PATH})
      endif()
    endif()
  endif()
endif()

# Si rien n'a été trouvé, fallback
if(NOT MINIUPNPC_TARGET)
  set(MINIUPNPC_TARGET miniupnpc)
  message(WARNING "miniupnpc not found via vcpkg or system, will try -lminiupnpc")
endif()

# ================== LIB: blockchain_core ==================
add_library(blockchain_core
        src/Block.cpp
        src/Blockchain.cpp
        src/transaction/BlockTransactions.cpp
        src/transaction/Output.cpp
        src/transaction/OutputReference.cpp
        src/transaction/Transaction.cpp
        src/transaction/TransactionPool.cpp
        src/network/NodeNetwork.cpp
        src/test/crypto.cpp
)

set_target_properties(blockchain_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(blockchain_core
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/transaction
        ${CMAKE_CURRENT_SOURCE_DIR}/src/network
        ${CMAKE_CURRENT_SOURCE_DIR}/src/test
        ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

# Ajouter Boost include dirs si nécessaire
if(Boost_INCLUDE_DIRS)
  target_include_directories(blockchain_core PUBLIC ${Boost_INCLUDE_DIRS})
endif()

# Ajouter miniupnpc include dirs si trouvé
if(MINIUPNPC_INC)
  target_include_directories(blockchain_core PUBLIC ${MINIUPNPC_INC})
endif()

# Link libraries
target_link_libraries(blockchain_core
        PUBLIC
        OpenSSL::Crypto
        OpenSSL::SSL
        Threads::Threads
        ${MINIUPNPC_TARGET}
)

# Boost linking
if(TARGET Boost::system)
  target_link_libraries(blockchain_core PUBLIC Boost::system)
elseif(Boost_SYSTEM_LIBRARY)
  target_link_libraries(blockchain_core PUBLIC ${Boost_SYSTEM_LIBRARY})
else()
  message(FATAL_ERROR "Boost::system not found")
endif()

if(TARGET Boost::headers)
  target_link_libraries(blockchain_core PUBLIC Boost::headers)
endif()

if(WIN32)
  target_link_libraries(blockchain_core PUBLIC ws2_32 iphlpapi)
  target_compile_definitions(blockchain_core PUBLIC WIN32_LEAN_AND_MEAN NOMINMAX)
endif()

# ================== APP: Qt Quick ==================
# IMPORTANT: Compiler les ressources QRC AVANT l'exécutable
qt6_add_resources(QRC_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/resources.qrc
)

# Debug pour vérifier
message(STATUS "Building Qt app with resources: ${CMAKE_CURRENT_SOURCE_DIR}/src/resources.qrc")

# Créer l'exécutable avec les ressources compilées
qt_add_executable(appBlockchain_desktop_PDG
    src/main.cpp
    ${QRC_SOURCES}
    src/ui/BlockchainFacade.hpp
)

# S'assurer que c'est une app Windows GUI
if(WIN32)
  set_target_properties(appBlockchain_desktop_PDG PROPERTIES
          WIN32_EXECUTABLE TRUE
  )
endif()

target_link_libraries(appBlockchain_desktop_PDG
        PRIVATE
        Qt6::Core
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Qml
        Qt6::Core5Compat
        blockchain_core
)

# Point tools to your local QML imports
set(QML_IMPORT_PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/qml
        CACHE STRING "QML import path" FORCE
)

set_target_properties(appBlockchain_desktop_PDG PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.cryptowallet.app
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS appBlockchain_desktop_PDG
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# DEBUG: Afficher la taille de l'exe après build
if(WIN32)
  add_custom_command(TARGET appBlockchain_desktop_PDG POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E echo "=== BUILD COMPLETE ==="
          COMMAND ${CMAKE_COMMAND} -E echo "Executable: $<TARGET_FILE:appBlockchain_desktop_PDG>"
          COMMAND ${CMAKE_COMMAND} -E echo "Check that exe size is > 500KB"
          COMMAND powershell -Command "if (Test-Path '$<TARGET_FILE:appBlockchain_desktop_PDG>') { $size = (Get-Item '$<TARGET_FILE:appBlockchain_desktop_PDG>').Length / 1KB; Write-Host \"Exe size: ${size}KB\" -ForegroundColor $(if ($size -gt 500) {'Green'} else {'Red'}) }"
  )
elseif(APPLE)
  add_custom_command(TARGET appBlockchain_desktop_PDG POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E echo "Build macOS: $<TARGET_BUNDLE_DIR:appBlockchain_desktop_PDG>"
  )
else()
  add_custom_command(TARGET appBlockchain_desktop_PDG POST_BUILD
          COMMAND ${CMAKE_COMMAND} -E echo "Build Linux: $<TARGET_FILE:appBlockchain_desktop_PDG>"
          COMMAND ${CMAKE_COMMAND} -E echo "Size check:"
          COMMAND ls -lh "$<TARGET_FILE:appBlockchain_desktop_PDG>"
  )
endif()

# ================== TESTS ==================
include(CTest)
if(BUILD_TESTS AND NOT CMAKE_CROSSCOMPILING)
  enable_testing()
  qt_add_executable(test_basic tests/test_basic.cpp)
  target_link_libraries(test_basic PRIVATE Qt6::Test Qt6::Core)
  add_test(NAME test_basic COMMAND test_basic)
  set_tests_properties(test_basic PROPERTIES TIMEOUT 10 ENVIRONMENT "QT_LOGGING_RULES=*.debug=false")
endif()

# ================== SUMMARY ==================
message(STATUS "========================================")
message(STATUS "Project:        ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build Type:     ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard:   ${CMAKE_CXX_STANDARD}")
message(STATUS "Qt Version:     ${Qt6_VERSION}")
message(STATUS "Tests Enabled:  ${BUILD_TESTS}")
message(STATUS "QML Import Path:${QML_IMPORT_PATH}")
message(STATUS "Resources:      ${CMAKE_CURRENT_SOURCE_DIR}/src/resources.qrc")
message(STATUS "miniupnpc target: ${MINIUPNPC_TARGET}")
message(STATUS "========================================")