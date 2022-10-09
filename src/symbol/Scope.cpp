/**
 * @file Scope.cpp
 * @author gpollice
 * @brief Scope (symbol table) implementation
 * @version 0.1
 * @date 2022-07-17
 */

#include "Symbol.h"
#include "Scope.h"

/**
 * @brief add a symbol
 * 
 * @param id 
 * @param t 
 * @return Symbol* the symbol added if there was not already one with the
 *  same key; otherwise, returns the original symbol and does not add the 
 *  new one.
 */
Symbol* Scope::addSymbol(std::string id, SymBaseType t) {
  Symbol* symbol = new Symbol(id, t);
  Symbol* find = findSymbol(id);
  if(find == nullptr){
	  symbols.insert(std::pair(id, symbol));
	  return symbol;
  }
  return nullptr;
}

/**
 * @brief add a symbol
 * 
 * @param symbol 
 * @return Symbol& Symbol* the symbol added if there was not already one with the
 *  same key; otherwise, returns the original symbol and does not add the 
 *  new one.
 */
Symbol* Scope::addSymbol(Symbol* symbol) {
  Symbol* find = findSymbol(symbol->id);
  if(find == nullptr){
	  symbols.insert(std::pair(symbol->id, symbol));
	  return symbol;
  }
  return nullptr;
}

/**
 * @brief Find a symbol in this scope
 * 
 * @param id the key for the symbol
 * @return Symbol* (nullptr if the symbol is not there)
 */
Symbol* Scope::findSymbol(std::string id) {
  Symbol* s = nullptr;
  std::map<std::string, Symbol*>::const_iterator i = symbols.find(id);
  if (i == symbols.end()) {
    s = nullptr;
  } else {
    s = i->second;
  }
  return s;
}

std::string Scope::toString() const {
  std::ostringstream description;
  description << std::endl << "-------------------" << std::endl
    << "SCOPE: " << scopeId;
  if (parent != nullptr) {
    description << " PARENT: " << parent->scopeId;
  }
  description << std::endl << '{';
  for (auto sym : symbols) {
    description << std::endl << "    " << sym.second->toString();
  }
  description << std:: endl << '}' << std::endl;

  return description.str(); 
}

