/**
 * @file st_manager_tests.cpp
 * @author gpollice
 * @brief Tests for the STManager
 * @version 0.1
 * @date 2022-07-18
 */
#include <catch2/catch_test_macros.hpp>
#include "STManager.h"

TEST_CASE("add a scope", "[symbol]") {
  STManager mgr;
  CHECK(mgr.scopeCount() == 0);
  Scope& s = mgr.enterScope();
  CHECK(mgr.scopeCount() == 1);
  Scope& s1 = mgr.getCurrentScope();
  CHECK(&s == &s1);
  CHECK(0 == s1.getId());
}

TEST_CASE("multiple scopes", "[symbol]") {
  STManager mgr;
  Scope &s1 = mgr.enterScope();
  Scope &s2 = mgr.enterScope();
  CHECK(1 == s2.getId());
  CHECK(0 == s2.getParent()->getId());
  Scope &s3 = mgr.enterScope();
  CHECK(1 == s3.getParent()->getId());
  Scope &temp = mgr.exitScope();
  CHECK(s3.getId() == temp.getId());
  Scope &s4 = mgr.enterScope();
  CHECK(1 == s4.getParent()->getId());
  CHECK(3 == s4.getId());
}

TEST_CASE("multiple scopes with symbols", "[symbol]") {
  STManager mgr;
  Scope &s1 = mgr.enterScope();
  mgr.addSymbol((new Symbol("a", BOOL)));
  mgr.addSymbol((new Symbol("b", INT)));
  CHECK(mgr.findSymbol("a")->baseType == BOOL);
  CHECK(mgr.findSymbol("x") == nullptr);
  Scope &s2 = mgr.enterScope();
  mgr.addSymbol((new Symbol("a", INT)));
  mgr.addSymbol((new Symbol("c", INT)));
  CHECK(mgr.findSymbol("a")->baseType == INT);
  Scope &s3 = mgr.enterScope();
  mgr.addSymbol((new Symbol("c", BOOL)));
  mgr.addSymbol((new Symbol("d", INT)));
  CHECK(mgr.findSymbol("b")->baseType == INT);
  CHECK(mgr.findSymbol("c")->baseType == BOOL);
  mgr.exitScope();
  Scope &s4 = mgr.enterScope();
  mgr.addSymbol((new Symbol("a", INT)));
  mgr.addSymbol((new Symbol("avar", BOOL)));
  CHECK(mgr.findSymbol("c")->baseType == INT);
  CHECK(mgr.findSymbol("d") == nullptr);
  // Uncomment the following to see the symbol table string
  //CHECK(mgr.toString() == "foo");
}
