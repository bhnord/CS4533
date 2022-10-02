#pragma once
#include "Symbol.h"
#include <map>

class Scope {
  public:
    Scope() { parent = nullptr; }
    Scope(Scope* p) { parent = p; }
    // with symbols in this scope only
    Symbol* addSymbol(Symbol *symbol);
    Symbol* addSymbol(std::string id, SymBaseType t);
    Symbol* findSymbol(std::string id);
    Scope* getParent() { return parent; }
    void setId(int id) { scopeId = id; }  // used by STManager
    int getId() { return scopeId; }
    std::string toString() const;

  private:
    int scopeId = -1;    // The index in the symbol table manager.
    Scope* parent;
    std::map<std::string, Symbol*> symbols;
};
