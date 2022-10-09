#pragma once
#include "antlr4-runtime.h"
#include <string>
#include <vector>
#include <sstream>

enum ErrType {SEMANTIC, CODEGEN}; ///ADD MORE TYPES

struct WPLError {
  ErrType type;
  antlr4::Token* token;
  std::string message;
  //WPLError();
  //WPLError(antlr4::Token* tok, std::string msg) {token = tok; message = msg;}
  std::string toString() {
    std::ostringstream e;
    e << '[' << token->getLine() << ',' << token->getCharPositionInLine()
      << "]: " << message;
    return e.str();
  }
};

class ErrorHandler {
  public:
    void addCodegenError(antlr4::Token* t, std::string msg) {
      WPLError* e = new WPLError();
      e->type = SEMANTIC;
      e->token = t;
      e->message = msg;
      errors.push_back(e);
    }
void addSemanticError(antlr4::Token* t, std::string msg) {
      WPLError* e = new WPLError();
      e->type = CODEGEN;
      e->token = t;
      e->message = msg;
      errors.push_back(e);
    }
    ///
    void addError(ErrType typ, antlr4::Token* t, std::string msg){
    	WPLError* e = new WPLError();
	e->type = typ; /////CHANGE THIS!!!!
	e->token = t;
	e->message = msg;
	errors.push_back(e);
    }
    std::vector<WPLError*>& getErrors() { return errors; }

    std::string errorList() {
      std::ostringstream errList;
      for (WPLError* e : errors) {
        errList << e->toString() << std::endl;
      }
      return errList.str();
    }

    bool hasErrors() { return !errors.empty(); }
  private:
    std::vector<WPLError*> errors;
};

