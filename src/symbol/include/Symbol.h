#pragma once
#include<string>
#include<sstream>
#include<vector>

enum SymType {SCALAR, ARRAY, FUNC, PROC};
enum SymBaseType {INT, BOOL, STR, UNDEFINED};


struct Param {
	Param(std::string i, SymBaseType t){id = i; baseType=t;}
	std::string id; //id
	SymBaseType baseType;  //base type
	std::string toString() const{
		std::ostringstream description;
		std::string typeName = baseType == INT ? "INT"
			: baseType == BOOL ? "BOOL"
			: baseType == STR ? "STRING"
			: "UNDEFINED";
		description << '{' << typeName << " " << id << '}';
		return description.str();
	}
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


	//for scalars
	Symbol(std::string i, SymBaseType t) {
		id = i;
		baseType = t;
		type = SCALAR;
	}

	////for functions 
	Symbol(std::string i, SymBaseType t, std::vector<Param*> *p){
		id = i;
		type = FUNC;
		baseType = t;
		params = p;
	}

	//for procs 
	Symbol(std::string i, std::vector<Param*> *p){
		id = i;
		type = PROC;
		baseType = UNDEFINED;
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

		std::ostringstream funcAddition;
		if(type == FUNC || type == PROC){
			if(type==FUNC){
				funcAddition << " func(";
			} else {
				funcAddition <<" param(";
			}
			if(params != nullptr){
				for(Param *p: *params){
					funcAddition << p->toString();
				}
			}
			funcAddition << ")";
		}


		description << '[' << id << ": " << typeName << funcAddition.str() << ']';
		return description.str(); 
	}
};
