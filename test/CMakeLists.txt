# Test CMakeLists.txt file
#
include(Testing)
include(ANTLR)
include(Symbol)
include(Semantic)
include(Utility)

include(cmake/LexParseTests.cmake)
include(cmake/SymbolTests.cmake)
include(cmake/SemanticTests.cmake)

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
target_include_directories(tests PUBLIC ${ANTLR_INCLUDE} ${ANTLR_GENERATED_DIR} ${SYMBOL_INCLUDE} ${SEMANTIC_INCLUDE} ${UTILITY_INCLUDE}) 
target_link_libraries(tests PRIVATE
      ${ANTLR_RUNTIME_LIB}
      #lexparse_lib
      sym_lib
      semantic_lib
      parser_lib
      utility_lib
      Catch2::Catch2WithMain
      Threads::Threads)

# Get the tests registered with CTest
catch_discover_tests(tests)
