cmake_minimum_required(VERSION 3.16)
project(BlockchainCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)

add_library(blockchain_core
    src/Block.cpp
    src/Blockchain.cpp
    src/cryptography/crypto.cpp
    src/transaction/BlockTransactions.cpp
    src/transaction/Output.cpp
    src/transaction/OutputReference.cpp
    src/transaction/Transaction.cpp
)

target_include_directories(blockchain_core PUBLIC src src/network)
target_link_libraries(blockchain_core PUBLIC Boost::system)

add_executable(blockchain_cli src/main.cpp)
target_link_libraries(blockchain_cli PRIVATE blockchain_core)
