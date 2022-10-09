/**
 * @file scope_tests->cpp
 * @author gpollice
 * @brief Scope unit tests
 * @version 0.1
 * @date 2022-07-17
 */
#include <catch2/catch_test_macros.hpp>
#include "Scope.h"
#include <iostream>

TEST_CASE("Check symbol return from add", "[symbol]") {
  Scope* scope = new Scope();
  Symbol* s = scope->addSymbol("a", SymBaseType::BOOL);
  CHECK(s->id == "a");
  Symbol* s1 = new Symbol("b", SymBaseType::INT);
  Symbol* s2 = scope->addSymbol(s1);
  // Check same values
  CHECK(s1->id == s2->id);
  CHECK(s1->type == s2->type);
  // Check same object
  CHECK(s1 == s2);
  // Uncomment the next in order to see what a scope string looks like->
  //CHECK(scope->toString() == "foo");
}

TEST_CASE("Find existing element", "[symbol") {
  Scope* scope = new Scope();
  Symbol* s = scope->addSymbol("a", SymBaseType::BOOL);
  Symbol& s1 = *(scope->findSymbol("a"));
  CHECK(&s1 == s);
}

TEST_CASE("Look for non-existent element", "[symbol]") {
  Scope* scope = new Scope();
  Symbol* s = scope->addSymbol("a", SymBaseType::BOOL);
  CHECK(s!=nullptr);
  Symbol* s1 = scope->findSymbol("b");
  CHECK(s1 == nullptr);
}

TEST_CASE("Duplicate symbol", "[symbol]") {
  Scope* scope = new Scope();
  Symbol* s = scope->addSymbol("a", SymBaseType::BOOL);
  CHECK(s!=nullptr);
  Symbol* s1 = new Symbol("a", SymBaseType::INT);
  Symbol* s2 = scope->addSymbol(s1);

  CHECK(s2 ==nullptr);
  CHECK(s2 != s1);
}
