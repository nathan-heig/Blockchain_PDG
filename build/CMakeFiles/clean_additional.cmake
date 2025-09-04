# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/appBlockchain_desktop_PDG_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/appBlockchain_desktop_PDG_autogen.dir/ParseCache.txt"
  "CMakeFiles/blockchain_core_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/blockchain_core_autogen.dir/ParseCache.txt"
  "CMakeFiles/test_basic_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/test_basic_autogen.dir/ParseCache.txt"
  "appBlockchain_desktop_PDG_autogen"
  "blockchain_core_autogen"
  "test_basic_autogen"
  )
endif()
