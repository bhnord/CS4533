#include "SemanticVisitor.h"
#include <any>


std::any SemanticVisitor::visitCompilationUnit(WPLParser::CompilationUnitContext*ctx) {
	stmgr->enterScope();
	for(auto ex: ctx->e){
		ex->accept(this);
	}
	return SymBaseType::UNDEFINED; //nullptr?
}
///NEW////////////////////////////////////////////
std::any SemanticVisitor::visitScalarDeclaration(WPLParser::ScalarDeclarationContext *ctx){
	SymBaseType type = UNDEFINED; //assume var
	std::string id;

	if(ctx->t != nullptr) {
		type = std::any_cast<SymBaseType>(ctx->t->accept(this));
	} else {
		type = UNDEFINED;
		//FIGURE OUT VAR ASSIGNMENT
	}

	//get the variable name(s)
	for (auto s: ctx->scalars){
		id = s -> id -> getText();


		//CHECK TYPE IF THERE IS DECLARATION
		if(s->v !=nullptr){
			SymBaseType sType = std::any_cast<SymBaseType>(s->v->accept(this));
			if(sType != type){
				errors.addSemanticError(s->getStart(), "Incorrect type: " + id + " " + Symbol::getSymBaseTypeName(sType) + ", Expected " + Symbol::getSymBaseTypeName(type));
			}
		}
		Symbol *sym = new Symbol(id, type);
		Symbol *symbol = stmgr->addSymbol(sym);
		if(symbol == nullptr){
			errors.addSemanticError(s->getStart(), "Duplicate variable: " + id);
		} else {
			///ADDED BINDING SHERE -------------------
			bindings->bind(s, symbol);
		}
	}
	return nullptr;
}	


std::any SemanticVisitor::visitType(WPLParser::TypeContext *ctx) {
	return ctx -> b != nullptr ? BOOL
		:ctx -> i != nullptr ? INT : STR;
};

std::any SemanticVisitor::visitBlock(WPLParser::BlockContext *ctx){
	stmgr -> enterScope();
	this ->visitChildren(ctx);
	stmgr-> exitScope();
	return nullptr;
}

std::any SemanticVisitor::visitFuncHeader(WPLParser::FuncHeaderContext *ctx) {
	SymBaseType type = std::any_cast<SymBaseType>(ctx -> t -> accept(this));
	std::string id = ctx -> id -> getText();
	std::vector<Param*> *params = nullptr;
	if (ctx -> p != nullptr) {
		params = std::any_cast<std::vector<Param*>*>(ctx -> p -> accept(this));
	}
	Symbol *sym = new Symbol(id, type, params);
	Symbol *symbol = stmgr->addSymbol(sym); // global scope
	bindings->bind(ctx, sym);
	if (symbol == nullptr) {
		errors.addSemanticError(ctx -> getStart(), "Duplicate variable: " + id);
	}
	stmgr->enterScope(); // scope for the parameters
	if (ctx -> p != nullptr) {
		unsigned int pnum =0;
		for (Param *p : *params) {
			Symbol *sym = new Symbol(p->id, p->baseType);
			Symbol *symbol = stmgr->addSymbol(sym);

			if (symbol == nullptr) {
				errors.addSemanticError(ctx -> getStart(), "Duplicate variable: " + id);
			} else {
				bindings->bind(ctx->p->p[pnum++], sym);
			}	
		}
	}
	return type;
}

std::any SemanticVisitor::visitFunction(WPLParser::FunctionContext *ctx){
	this->visitChildren(ctx);
	stmgr->exitScope(); // entered in the header

	if(ctx->b->s.size() != 0){
	WPLParser::StatementContext *s = ctx->b->s[ctx->b->s.size()-1];
	
	if(s != nullptr){
		if(s->r != nullptr){
			return nullptr;
		}
	}
	}
	errors.addSemanticError(ctx -> getStart(), "No return as last in function: ");
	return nullptr;
}

std::any SemanticVisitor::visitProcHeader(WPLParser::ProcHeaderContext *ctx){
	std::string id = ctx -> id -> getText();
	std::vector<Param*> *params = nullptr;
	if (ctx -> p != nullptr) {
		params = std::any_cast<std::vector<Param*>*>(ctx -> p -> accept(this));
	}
	Symbol *sym = new Symbol(id, params);
	Symbol *symbol = stmgr->addSymbol(sym); // global scope
	if (symbol == nullptr) {
		errors.addSemanticError(ctx -> getStart(), "Duplicate variable: " + id);
	}
	stmgr->enterScope(); // scope for the parameters
	if (ctx -> p != nullptr) {
		for (Param *p : *params) {
			Symbol *sym = new Symbol(p->id, p->baseType);
			Symbol *symbol = stmgr->addSymbol(sym);
			if (symbol == nullptr) {
				errors.addSemanticError(ctx -> getStart(), "Duplicate variable: " + id);
			}
		}
	}
	return nullptr;
}

std::any SemanticVisitor::visitProcedure(WPLParser::ProcedureContext *ctx){
	this->visitChildren(ctx);
	stmgr->exitScope();
	return nullptr;
}




///FIND OUT IF i AM SUPPOSED TO EXIT SCOPE HERE !!!!! -------------------------
std::any SemanticVisitor::visitExternProcHeader(WPLParser::ExternProcHeaderContext *ctx){
	std::string id = ctx -> id -> getText();
	std::vector<Param*> *params = nullptr;
	if (ctx -> p != nullptr) {
		params = std::any_cast<std::vector<Param*>*>(ctx -> p -> accept(this));
        }
        Symbol *sym = new Symbol(id, params);
        Symbol *symbol = stmgr->addSymbol(sym); // global scope
        if (symbol == nullptr) {
                errors.addSemanticError(ctx -> getStart(), "Duplicate variable: " + id);
        }
        stmgr->enterScope(); // scope for the parameters
        if (ctx -> p != nullptr) {
                for (Param *p : *params) {
                        Symbol *sym = new Symbol(p->id, p->baseType);
                        Symbol *symbol = stmgr->addSymbol(sym);
                        if (symbol == nullptr) {
                                errors.addSemanticError(ctx -> getStart(), "Duplicate variable: " + id);
                        }
                }
        }


	stmgr->exitScope();
        return nullptr;
}

std::any SemanticVisitor::visitExternFuncHeader(WPLParser::ExternFuncHeaderContext *ctx) {
        SymBaseType type = std::any_cast<SymBaseType>(ctx -> t -> accept(this));
        std::string id = ctx -> id -> getText();
        std::vector<Param*> *params = nullptr;
        if (ctx -> p != nullptr) {
                params = std::any_cast<std::vector<Param*>*>(ctx -> p -> accept(this));
        }
        Symbol *sym = new Symbol(id, type, params);
        Symbol *symbol = stmgr->addSymbol(sym); // global scope
        if (symbol == nullptr) {
                errors.addSemanticError(ctx -> getStart(), "Duplicate variable: " + id);
        }
        stmgr->enterScope(); // scope for the parameters
        if (ctx -> p != nullptr) {
                for (Param *p : *params) {
                        Symbol *sym = new Symbol(p->id, p->baseType);
                        Symbol *symbol = stmgr->addSymbol(sym);
                        if (symbol == nullptr) {
                                errors.addSemanticError(ctx -> getStart(), "Duplicate variable: " + id);
                        }
                }
        }


	stmgr->exitScope();
        return type;
}


std::any SemanticVisitor::visitCall(WPLParser::CallContext *ctx){
	std::string id = ctx->id->getText();
	Symbol *s = stmgr->findSymbol(id);
	if(s == nullptr || (s->type != SymType::FUNC && s->type != SymType::PROC)){
		errors.addSemanticError(ctx->getStart(), "Undefined call: " + id);
	} else {
		bindings->bind(ctx, s);	
		std::vector<SymBaseType> *args = std::any_cast<std::vector<SymBaseType>*>(ctx->a->accept(this));
		std::vector<Param*> *params = s->params;
		//CHECK IF THEY HAVE EQUAL TYPES
		unsigned int size = params->size();
		if(args->size() != size){
			errors.addSemanticError(ctx->getStart(), "Invalid number of parameters: " + id);
		} else {
			for(unsigned int i = 0; i < size; i++){
				if((*params)[i]->baseType != (*args)[i]){
					errors.addSemanticError(ctx->getStart(), "Invalid parameter type: " + Symbol::getSymBaseTypeName((*args)[i]) + ", Expected: " + Symbol::getSymBaseTypeName((*params)[i]->baseType) + " for parameter " + (*params)[i]->id);
				}
			}

		}
	}
	return nullptr;
}

std::any SemanticVisitor::visitFuncCallExpr(WPLParser::FuncCallExprContext *ctx){
	std::string id = ctx->fname->getText();
        Symbol *s = stmgr->findSymbol(id);
	SymBaseType type = SymBaseType::UNDEFINED;
        if(s == nullptr || s->type != SymType::FUNC){
		errors.addSemanticError(ctx->getStart(), "Undefined call: " + id);
	} else {
		type = s->baseType;
		bindings->bind(ctx, s);
		std::vector<SymBaseType> *args = new std::vector<SymBaseType>;

		for(WPLParser::ExprContext *e : ctx->args){
			SymBaseType type = std::any_cast<SymBaseType>(e->accept(this));
			args->push_back(type);
		}

		std::vector<Param*> *params = s->params;
		//CHECK IF THEY HAVE EQUAL TYPES
		unsigned int size = params->size();
		if(args->size() != size){
			errors.addSemanticError(ctx->getStart(), "Invalid number of parameters: " + id);
		} else {
			for(unsigned int i = 0; i < size; i++){
				if((*params)[i]->baseType != (*args)[i]){
					errors.addSemanticError(ctx->getStart(), "Invalid parameter type: " + Symbol::getSymBaseTypeName((*args)[i]) + ", Expected: " + Symbol::getSymBaseTypeName((*params)[i]->baseType) + " for parameter " + (*params)[i]->id);
				}
			}

		}
	}
	return type;
}


std::any SemanticVisitor::visitArguments(WPLParser::ArgumentsContext *ctx){
	std::vector<SymBaseType> *args = new std::vector<SymBaseType>;

	for(WPLParser::ArgContext *a : ctx->a){
		SymBaseType type = std::any_cast<SymBaseType>(a->accept(this));
		args->push_back(type);
	}
	return args;

}

std::any SemanticVisitor::visitArg(WPLParser::ArgContext *ctx){
	if(ctx->id == nullptr){
		return this->visitChildren(ctx);
	} else {
		SymBaseType t = SymBaseType::UNDEFINED;
		std::string id = ctx -> id -> getText();
		Symbol* symbol = stmgr-> findSymbol(id);
		if(symbol != nullptr){
			bindings -> bind(ctx, symbol);
			t = symbol->baseType;
		} else {
			errors.addSemanticError(ctx -> getStart(), "Use of undefined variable: " + id);
		}
		return t;
	}
}

//std::any SemanticVisitor::visitFucnCallExpr(WPLParser::FuncCallExprContext *ctx){
//	//find fname in linked
//	Symbol* symbol = stmgr->findSymbol(ctx->fname->getText());
//	//check variables with func definition
//
//}
std::any SemanticVisitor::visitArrayDeclaration(WPLParser::ArrayDeclarationContext *ctx){
	SymBaseType t = std::any_cast<SymBaseType>(ctx -> t -> accept(this));
	std::string id = ctx->id->getText();
	std::string i = ctx->i->getText();
	int len = std::stoi(ctx->i->getText());

	if(len < 0){
		errors.addSemanticError(ctx->getStart(), "Invalid array length: " + id + ", " + ctx->i->getText());
	} else {
		Symbol *sym = new Symbol(id, t, len);
		Symbol *symbol = stmgr->addSymbol(sym);
		if(symbol == nullptr){
			errors.addSemanticError(ctx->getStart(), "Duplicate variable: " + id);
		}
	}
	return nullptr;
}


// do I need length checking for arrays?
//returns type of array array[int]
std::any SemanticVisitor::visitArrayIndex(WPLParser::ArrayIndexContext *ctx){
	SymBaseType e = std::any_cast<SymBaseType>(ctx -> ex -> accept(this));
	std::string id = ctx -> id -> getText();

	if(e != SymBaseType::INT){
		errors.addSemanticError(ctx->getStart(), "INT expression expected, but was " + Symbol::getSymBaseTypeName(e));
	} else {
		int i = std::stoi(ctx->ex->getText());
		if(i <= 0){
			errors.addSemanticError(ctx->getStart(), "Invalid array length: " + id + ", " + ctx->ex->getText());
		}
	}
	//bind array
	Symbol* symbol = stmgr-> findSymbol(id);
	SymBaseType t = SymBaseType::UNDEFINED;
	if(symbol != nullptr){
		bindings -> bind(ctx, symbol);
		t = symbol->baseType;
	} else {
		errors.addSemanticError(ctx -> getStart(), "Use of undefined variable    : " + id);
	}
	return t;
}	

//array.length ----- do we need a binding here?
std::any SemanticVisitor::visitArrayLengthExpr(WPLParser::ArrayLengthExprContext *ctx){
	SymBaseType t = SymBaseType::UNDEFINED;
	std::string id = ctx -> arrayname -> getText();
	Symbol* symbol = stmgr-> findSymbol(id);
	if(symbol != nullptr){
		bindings -> bind(ctx, symbol);
		t = SymBaseType::INT;
	} else {
		errors.addSemanticError(ctx -> getStart(), "Use of undefined variable: " + id);
	}
	return t;
}


//creates all params for func
std::any SemanticVisitor::visitParams(WPLParser::ParamsContext *ctx){
	std::vector<Param*> *params = new std::vector<Param*>;

	for(WPLParser::ParamContext *param: ctx->p){
		std::string id = param->id->getText();
		SymBaseType type = std::any_cast<SymBaseType>(param->t->accept(this));
		Param *prm = new Param(id, type);
		params->push_back(prm);
	}
	return params;
}

//return param to add to func
std::any SemanticVisitor::visitParam(WPLParser::ParamContext *ctx) {
	SymBaseType type = std::any_cast<SymBaseType>(ctx -> t -> accept(this));
	std::string id = ctx->id->getText();
	Param *param = new Param(id, type);
	return param;
}


// return type to ensure semantic
std::any SemanticVisitor::visitIDExpr(WPLParser::IDExprContext *ctx) {
	SymBaseType t = SymBaseType::UNDEFINED;
	std::string id = ctx -> id -> getText();
	Symbol* symbol = stmgr-> findSymbol(id);
	if(symbol != nullptr){
		bindings -> bind(ctx, symbol);
		t = symbol->baseType;
	} else {
		errors.addSemanticError(ctx -> getStart(), "Use of undefined variable: " + id);
	}
	return t; 
}

//assignment with left and right: var1 <- var2
std::any SemanticVisitor::visitAssignment(WPLParser::AssignmentContext *ctx){
	SymBaseType right = std::any_cast<SymBaseType>(ctx->e->accept(this));

	SymBaseType left  = SymBaseType::UNDEFINED;
	if(ctx -> target != nullptr){
		std::string id = ctx -> target -> getText();
		Symbol* symbol = stmgr-> findSymbol(id);
		if(symbol != nullptr){
			bindings -> bind(ctx, symbol);
			left = symbol->baseType;
		} else {
			errors.addSemanticError(ctx -> getStart(), "Use of undefined variable: " + id);
		}
	} else {
		left = std::any_cast<SymBaseType>(ctx->arr->accept(this)); 
	}

	if(left != right){
		errors.addSemanticError(ctx -> getStart(), "Type mismatch on assignment: " + Symbol::getSymBaseTypeName(left) + ", " + Symbol::getSymBaseTypeName(right));
	}

	return nullptr;
}


std::any SemanticVisitor::visitAndExpr(WPLParser::AndExprContext *ctx){
	SymBaseType t = SymBaseType::UNDEFINED;
	SymBaseType left = std::any_cast<SymBaseType>( ctx->left->accept(this));
	SymBaseType right = std::any_cast<SymBaseType>(ctx->right->accept(this));
	if(left == SymBaseType::BOOL && left == right){
		t = SymBaseType::BOOL;
	} else {
		errors.addSemanticError(ctx->getStart(), "BOOL expression expected, but was: " + Symbol::getSymBaseTypeName(left) + ", " + Symbol::getSymBaseTypeName(right)); 
	}
	return t;
}

//or expr - boolean |
std::any SemanticVisitor::visitOrExpr(WPLParser::OrExprContext *ctx){
	SymBaseType t = SymBaseType::UNDEFINED;
	SymBaseType left = std::any_cast<SymBaseType>( ctx->left->accept(this));
	SymBaseType right = std::any_cast<SymBaseType>(ctx->right->accept(this));
	if(left == SymBaseType::BOOL && left == right){
		t = SymBaseType::BOOL;
	} else {
		errors.addSemanticError(ctx->getStart(), "BOOL expression expected, but was: " + Symbol::getSymBaseTypeName(left) + ", " + Symbol::getSymBaseTypeName(right));
	}
	return t;
}

std::any SemanticVisitor::visitLoop(WPLParser::LoopContext *ctx){
	SymBaseType t = std::any_cast<SymBaseType>(ctx->e->accept(this));
	if(t != SymBaseType::BOOL){
		errors.addSemanticError(ctx->getStart(), "BOOL expression expected, but was: " + Symbol::getSymBaseTypeName(t));
	}
	ctx->b->accept(this);
	return nullptr;
}


std::any SemanticVisitor::visitConditional(WPLParser::ConditionalContext *ctx){
	SymBaseType t = std::any_cast<SymBaseType>(ctx->e->accept(this));
	if(t != SymBaseType::BOOL){
		errors.addSemanticError(ctx->getStart(), "BOOL expression expected, but was: " + Symbol::getSymBaseTypeName(t));
	}
	for(WPLParser::BlockContext *b: ctx->b){
		b->accept(this);
	}
	return nullptr;
}	


///MAY WANT TO VISIT IN SELECT INSTEAD OF SELECTALT?
std::any SemanticVisitor::visitSelectAlt(WPLParser::SelectAltContext *ctx){
	SymBaseType t  = std::any_cast<SymBaseType>(ctx->e->accept(this));
	if(t != SymBaseType::BOOL){
		errors.addSemanticError(ctx->getStart(), "BOOL expression expected, but was: " + Symbol::getSymBaseTypeName(t));
	}
	//make sure statement is ok syntactically
	ctx->s->accept(this);
	return t;
}

////////////////////////////////////////////////


//std::any SemanticVisitor::visitAssignExpression(WPLParser::AssignExpressionContext *ctx) {
//  // Visit the expression
//  SymBaseType result = std::any_cast<SymBaseType>(ctx->ex->accept(this));
//  if (result == UNDEFINED) {
//    errors.addSemanticError(ctx->getStart(), "Expression evaluates to an UNDEFINED type");
//  }
//  std::string varId = ctx->v->getText();
//  Symbol *symbol = stmgr->findSymbol(varId);
//  if (symbol == nullptr) {
//    // Undefined: add it
//    symbol = stmgr->addSymbol(varId, result);
//  } else {
//    symbol->type = result;
//  }
//  // For the Calculator, we don't have to check the type of the variable since
//  // it would be redefined here.
//  return result;
//}
//
/**
 * @brief Constant.type = INT, STR, BOOL
 */
std::any SemanticVisitor::visitConstant(WPLParser::ConstantContext *ctx) {
	if(ctx->i != nullptr){
		return SymBaseType::INT;
	} else if(ctx->s != nullptr){
		return SymBaseType::STR;
	} else {
		return SymBaseType::BOOL;
	}
}

/**
 * @brief booleanConstant.type = BOOL
 */
//std::any SemanticVisitor::visitBoolean(WPLParser::BooleanContext *ctx) {
//  return SymBaseType::BOOL;
//}
//
/**
 * @brief UnaryMinusExpr.type = ex.type && ex.type == INT;
 * @return SymTyp::INT if no error, SymBaseType::UNDEFINED if an error.
 */
std::any SemanticVisitor::visitUnaryMinusExpr(WPLParser::UnaryMinusExprContext *ctx) {
	auto t = std::any_cast<SymBaseType>(ctx -> ex ->accept(this));
	if (t != SymBaseType::INT){ // Type mismatch
		errors.addSemanticError(ctx->getStart(), "INT expression expected, but was " + Symbol::getSymBaseTypeName(t));
		t = SymBaseType::UNDEFINED;
	}
	return t;
}
//
//
/**
 * @brief UnaryNotExpr.type = ex.type && ex.type == BOOL;
 * @return SymTyp::BOOL if no error, SymBaseType::UNDEFINED if an error.
 */
std::any SemanticVisitor::visitUnaryNotExpr(WPLParser::UnaryNotExprContext *ctx) {
	auto t = std::any_cast<SymBaseType>(ctx -> ex ->accept(this));
	if (t != SymBaseType::BOOL){ // Type mismatch
		errors.addSemanticError(ctx->getStart(), "BOOL expression expected, but was " + Symbol::getSymBaseTypeName(t));
		t = SymBaseType::UNDEFINED;
	}
	return t;
}
//
//
///**
// * @brief BinaryArithExpr.type = INT && left.type == INT && right.type == INT
// *
// * @return SymBaseType::INT if there are no errors or SymBaseType::UNDEFINED if there are errors.
// */
std::any SemanticVisitor::visitMultExpr(WPLParser::MultExprContext *ctx) {
	SymBaseType type = SymBaseType::INT;
	auto left = std::any_cast<SymBaseType>(ctx -> left ->accept(this));
	if (left != SymBaseType::INT){ // Type mismatch
		errors.addSemanticError(ctx->getStart(), "INT left expression expected, but was " + Symbol::getSymBaseTypeName(left));
		type = SymBaseType::UNDEFINED;;
	}
	auto right = std::any_cast<SymBaseType>(ctx -> right ->accept(this));
	if (right != SymBaseType::INT){ // Type mismatch
		errors.addSemanticError(ctx->getStart(), "INT right expression expected, but was " + Symbol::getSymBaseTypeName(right));
		type = SymBaseType::UNDEFINED;
	}
	return type;
}



std::any SemanticVisitor::visitAddExpr(WPLParser::AddExprContext *ctx) {
	SymBaseType type = INT;
	auto left = std::any_cast<SymBaseType>(ctx -> left ->accept(this));
	if (left != SymBaseType::INT){ // Type mismatch
		errors.addSemanticError(ctx->getStart(), "INT left expression expected, but was " + Symbol::getSymBaseTypeName(left));
		type = SymBaseType::UNDEFINED;;
	}
	auto right = std::any_cast<SymBaseType>(ctx -> right ->accept(this));
	if (right != SymBaseType::INT){ // Type mismatch
		errors.addSemanticError(ctx->getStart(), "INT right expression expected, but was " + Symbol::getSymBaseTypeName(right));
		type = SymBaseType::UNDEFINED;
	}
	return type;
}


std::any SemanticVisitor::visitRelExpr(WPLParser::RelExprContext *ctx) {
	SymBaseType type = SymBaseType::BOOL;
	auto left = std::any_cast<SymBaseType>(ctx -> left ->accept(this));
	if (left != SymBaseType::INT){ // Type mismatch
		errors.addSemanticError(ctx->getStart(), "INT left expression expected, but was " + Symbol::getSymBaseTypeName(left));
		type = SymBaseType::UNDEFINED;;
	}
	auto right = std::any_cast<SymBaseType>(ctx -> right ->accept(this));
	if (right != SymBaseType::INT){ // Type mismatch
		errors.addSemanticError(ctx->getStart(), "INT right expression expected, but was " + Symbol::getSymBaseTypeName(right));
		type = SymBaseType::UNDEFINED;
	}
	return type;
}


//
//
///**
// * @brief BinaryRelExpr.type = BOOL && left.type == INT && right.type == INT
// *
// * @return SymBaseType::BOOL if there are no errors or SymBaseType::UNDEFINED if there are errors.
// */
// std::any SemanticVisitor::visitBinaryRelExpr(WPLParser::BinaryRelExprContext *ctx) {
//   SymBaseType type = BOOL;
//   auto left = std::any_cast<SymBaseType>(ctx -> left ->accept(this));
//   if (left != SymBaseType::INT){ // Type mismatch
//     errors.addSemanticError(ctx->getStart(), "INT left expression expected, but was " + Symbol::getSymBaseTypeName(left));
//     type = SymBaseType::UNDEFINED;;
//   }
//   auto right = std::any_cast<SymBaseType>(ctx -> right ->accept(this));
//   if (right != SymBaseType::INT){ // Type mismatch
//     errors.addSemanticError(ctx->getStart(), "INT right expression expected, but was " + Symbol::getSymBaseTypeName(right));
//     type = SymBaseType::UNDEFINED;
//   }
//   return type;
// }

//
/**
 * @brief EqExpr.type = SymBaseType::BOOL && left.type == right.type
 *
 * @return SymBaseType::BOOL if there are no errors or SymBaseType::UNDEFINED if there are errors.
 */
std::any SemanticVisitor::visitEqExpr(WPLParser::EqExprContext *ctx) {
	SymBaseType result = SymBaseType::BOOL;
	SymBaseType left = std::any_cast<SymBaseType>(ctx -> left ->accept(this));
	SymBaseType right = std::any_cast<SymBaseType>(ctx -> right ->accept(this));
	if (left != right) {
		errors.addSemanticError(ctx->getStart(), "Both sides of '=' must have the same type");
		result = SymBaseType::UNDEFINED;
	}
	return result;
}


/**
 * @brief ParenExpr.type = ex.type
 * can probably get rid of this function
 */
std::any SemanticVisitor::visitParenExpr(WPLParser::ParenExprContext *ctx) {
	return ctx->ex->accept(this);
}
//
//
///**
// * @brief v.defined == TRUE && VariableExpr.type = v.type
// *
// * @return The type of the variable as found in the symbol table or UNDEFINED if
// *  it is not defined in the symbol table.
// */
//std::any SemanticVisitor::visitVariableExpr(WPLParser::VariableExprContext *ctx) {
//  SymBaseType result = UNDEFINED;
//  std::string varId = ctx->v->getText();
//  Symbol *symbol = stmgr->findSymbol(varId);
//  if (symbol == nullptr) {
//    // Undefined: error
//    errors.addSemanticError(ctx->getStart(), "Undefined variable in expression: " + varId);
//  } else {
//    // bind the symbol to this node
//    bindings->bind(ctx, symbol);
//    result = symbol->type;
//  }
//  return result;
//}
