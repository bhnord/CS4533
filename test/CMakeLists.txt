# Test CMakeLists.txt file
#
include(Testing)
include(ANTLR)
include(Symbol)
include(Semantic)
include(Utility)
include(LLVM)

include(cmake/LexParseTests.cmake)
include(cmake/SymbolTests.cmake)
include(cmake/SemanticTests.cmake)


find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include(AddLLVM)
include(HandleLLVMOptions)


### Set up all tests to be run
add_executable(
  tests
  #${LEXPARSE_TESTS}
  ${SYMBOL_TESTS}
  ${SEMANTIC_TESTS}
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_dependencies(tests parser_lib sym_lib semantic_lib utility_lib)



target_include_directories(tests PUBLIC 
  ${ANTLR_INCLUDE} ${ANTLR_GENERATED_DIR}
  ${SYMBOL_INCLUDE} 
  ${SEMANTIC_INCLUDE}
  ${UTILITY_INCLUDE}
  ${CODEGEN_INCLUDE}
  ${LLVM_BINARY_DIR}/include
  ${LLVM_INCLUDE_DIR}
  )


target_link_libraries(tests PRIVATE
      ${ANTLR_RUNTIME_LIB}
      #lexparse_lib
      sym_lib
      semantic_lib
      parser_lib
      utility_lib
      codegen_lib
  ${LLVM_LIBS}
      Catch2::Catch2WithMain
      Threads::Threads)

# Get the tests registered with CTest
catch_discover_tests(tests)
