cmake_minimum_required(VERSION 3.16)
project(BlockchainCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)

# Ajouter cereal (header-only)
include(FetchContent)
FetchContent_Declare(
    cereal
    GIT_REPOSITORY https://github.com/USCiLab/cereal.git
    GIT_TAG v1.3.2
)
FetchContent_MakeAvailable(cereal)

add_library(blockchain_core
    src/Block.cpp
    src/Blockchain.cpp
    src/cryptography/crypto.cpp
    src/transaction/BlockTransactions.cpp
    src/transaction/Output.cpp
    src/transaction/OutputReference.cpp
    src/transaction/Transaction.cpp
)

target_include_directories(blockchain_core PUBLIC src src/network ${cereal_SOURCE_DIR}/include)
target_link_libraries(blockchain_core PUBLIC Boost::system)

add_executable(blockchain_cli src/main.cpp)
target_link_libraries(blockchain_cli PRIVATE blockchain_core)
