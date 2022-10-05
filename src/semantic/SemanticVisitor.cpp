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
	}

	//get the variable name(s)
	for (auto s: ctx->scalars){
		id = s -> id -> getText();
		Symbol *sym = new Symbol(id, type );
		Symbol *symbol = stmgr->addSymbol(sym);
		if(symbol == nullptr){
			errors.addSemanticError(ctx->getStart(), "Duplicate variable: " + id);
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

std::any SemanticVisitor::visitFunction(WPLParser::FunctionContext *ctx){
	this->visitChildren(ctx);
	stmgr->exitScope(); // entered in the header
	return nullptr;
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
	Symbol *sym = new Symbol(id, t, 5); //stoi(ctx-> i -> getText()));
	Symbol *symbol = stmgr->addSymbol(sym);
	if(symbol == nullptr){
		errors.addSemanticError(ctx->getStart(), "Duplicate variable: " + id);
	}

	return nullptr;
}



//returns type of array
std::any SemanticVisitor::visitArrayIndex(WPLParser::ArrayIndexContext *ctx){
	SymBaseType e = std::any_cast<SymBaseType>(ctx -> ex -> accept(this));
	if(e != SymBaseType::INT){
		errors.addSemanticError(ctx->getStart(), "INT expression expected, but was " + Symbol::getSymBaseTypeName(e));
		return nullptr;
	}
	//bind array
	std::string id = ctx -> id -> getText();
	Symbol* symbol = stmgr-> findSymbol(id);
	if(symbol != nullptr){
		bindings -> bind(ctx, symbol);
	} else {
		errors.addSemanticError(ctx -> getStart(), "Use of undefined variable    : " + id);
	}
	return symbol->baseType;
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
	std::string id = ctx -> id -> getText();
	Symbol* symbol = stmgr-> findSymbol(id);
	if(symbol != nullptr){
		bindings -> bind(ctx, symbol);
	} else {
		errors.addSemanticError(ctx -> getStart(), "Use of undefined variable: " + id);
		return SymBaseType::UNDEFINED;
	}
	return symbol->baseType;
}

std::any SemanticVisitor::visitAssignment(WPLParser::AssignmentContext *ctx){
	std::string id = ctx -> target -> getText();
	Symbol* symbol = stmgr-> findSymbol(id);
	if(symbol != nullptr){
		bindings -> bind(ctx, symbol);
	} else {
		errors.addSemanticError(ctx -> getStart(), "Use of undefined variable: " + id);
		return nullptr;
	}

	SymBaseType left = symbol->baseType;
	SymBaseType right = std::any_cast<SymBaseType>(ctx->e->accept(this));
	if(left != right){
		errors.addSemanticError(ctx -> getStart(), "Type mismatch on assignment: " + Symbol::getSymBaseTypeName(left) + ", " + Symbol::getSymBaseTypeName(right));
	}
	return nullptr;
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
	result = std::any_cast<SymBaseType>(ctx -> right ->accept(this));
	auto left = std::any_cast<SymBaseType>(ctx -> left ->accept(this));
	if (result != left) {
		errors.addSemanticError(ctx->getStart(), "Both sides of '=' must have the same type");
		result = SymBaseType::UNDEFINED;
	}
	return result;
}


/**
 * @brief ParenExpr.type = ex.type
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
