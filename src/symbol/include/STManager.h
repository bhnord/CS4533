#pragma once
#include "Scope.h"
#include <vector>

class STManager {
  public:
    STManager(){};
    Scope& enterScope();
    Scope& exitScope();

    // Pass through methods
    Symbol* addSymbol(Symbol* symbol);
    Symbol* addSymbol(std::string id, SymBaseType t);
    Symbol* findSymbol(std::string id);

    // Miscellaneous (useful for testing)
    Scope& getCurrentScope() { return *currentScope; }
    Scope *getCurrentScope2(){return currentScope;}
    void setCurrentScope(Scope *s) {currentScope = s;}
    int scopeCount() { return scopes.size(); }
    std::string toString() const;

  private:
    std::vector<Scope*> scopes;
    Scope* currentScope = nullptr;
    int scopeNumber = 0;
};
