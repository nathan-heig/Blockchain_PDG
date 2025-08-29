cmake_minimum_required(VERSION 3.16)
project(BlockchainCore LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost REQUIRED COMPONENTS system)
find_package(OpenSSL REQUIRED)



add_library(blockchain_core
    src/Block.cpp
    src/Blockchain.cpp
    src/transaction/BlockTransactions.cpp
    src/transaction/Output.cpp
    src/transaction/OutputReference.cpp
    src/transaction/Transaction.cpp
    src/network/NodeNetwork.cpp
    src/test/crypto.cpp
)

target_include_directories(blockchain_core PUBLIC src)

target_link_libraries(blockchain_core PUBLIC Boost::system OpenSSL::Crypto OpenSSL::SSL miniupnpc pthread)

add_executable(blockchain_cli src/main.cpp)
target_link_libraries(blockchain_cli PRIVATE blockchain_core)

