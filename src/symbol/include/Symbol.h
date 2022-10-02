#pragma once
#include<string>
#include<sstream>
#include<vector>

enum SymType {SCALAR, ARRAY, METHOD};
enum SymBaseType {INT, BOOL, STR, UNDEFINED};


struct Param {
	Param(std::string i, SymBaseType t){id = i; baseType=t;}
	std::string id; //id
	SymBaseType baseType;  //base type
};


struct Symbol
{
  std::string id;
  SymType type;
  SymBaseType baseType;

  ////
  SymType retType;
  // METHOD = return type , ARRAY = element type
  int length = 0; //ARRAY only
  std::vector<Param*> *params = nullptr; //for methods


  Symbol(std::string i, SymBaseType t) {
    id = i;
    baseType = t;
    type = SCALAR;
  }

  ////for functions / processes
  Symbol(std::string i, SymBaseType t, std::vector<Param*> *p){
	  id = i;
	  type = METHOD;
	  baseType = t;
	  params = p;
  }

  ////for arrays
  Symbol(std::string i, SymBaseType t, int len){
	  id = i;
	  baseType = t;
	  type = ARRAY;
	  length = len;
  }

  static std::string getSymBaseTypeName(SymBaseType t) {
  std::ostringstream description;
    std::string typeName = t == INT ? "INT"
      : t == BOOL ? "BOOL" 
      : t == STR  ? "STR"
      : "UNDEFINED";
    description << typeName;
    return description.str();
  }

  std::string toString() const {
    std::ostringstream description;
    std::string typeName = baseType == INT ? "INT"
      : baseType == BOOL ? "BOOL" 
      : baseType == STR ? "STRING"
      : "UNDEFINED";
    description << '[' << id << ", " << typeName << ']';
    return description.str(); 
  }
};
