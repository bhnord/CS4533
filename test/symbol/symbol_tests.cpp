#include <catch2/catch_test_macros.hpp>
#include "Symbol.h"

TEST_CASE("Create a Symbol", "[symbol]") {
  Symbol s("a", SymBaseType::BOOL);
  CHECK(s.id== "a");
  CHECK(s.baseType == BOOL);
  //CHECK("foo" == s.toString());
  CHECK("[a, BOOL]" == s.toString());
}
