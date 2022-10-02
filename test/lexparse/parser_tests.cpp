/**
 * @file parser_tests.cpp
 * @author gpollice
 * @brief Parser tests
 * @version 0.1
 * @date 2022-07-04
 * @copyright Copyright (c) 2022
 * @see https://stackoverflow.com/questions/44515370/how-to-override-error-reporting-in-c-target-of-antlr4
 */

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include "antlr4-runtime.h"
#include "WPLLexer.h"
#include "WPLParser.h"
#include "test_error_handlers.h"

/****************** Positive tests ******************/

TEST_CASE( "Integer expression", "[front-end]" ) {
  antlr4::ANTLRInputStream input("1");
  WPLLexer lexer(&input);
  lexer.removeErrorListeners();
  lexer.addErrorListener(new TestErrorListener());
  antlr4::CommonTokenStream tokens(&lexer);
  WPLParser parser(&tokens);
  parser.removeErrorListeners();
  parser.addErrorListener(new TestErrorListener());
  antlr4::tree::ParseTree* tree = parser.compilationUnit(); // entry at expression

  REQUIRE(tree != NULL);
  std::string tString = tree->toStringTree(&parser);
  CHECK(tString == "(expression 1)");
}
