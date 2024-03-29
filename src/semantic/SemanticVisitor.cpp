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
		//VAR ASSIGNMENT
	}

	//get the variable name(s)
	for (auto s: ctx->scalars){
		id = s -> id -> getText();


		//CHECK TYPE IF THERE IS DECLARATION
		SymBaseType sType = UNDEFINED;
		bool isDef = s->v !=nullptr;
		if(isDef){
			sType = std::any_cast<SymBaseType>(s->v->accept(this));
				
			if(type != UNDEFINED && sType != type){
				errors.addSemanticError(s->getStart(), "Incorrect type: " + id + " " + Symbol::getSymBaseTypeName(sType) + ", Expected " + Symbol::getSymBaseTypeName(type));
			}
		} 
		Symbol *sym = nullptr;
		if(type == UNDEFINED)
			sym = new Symbol(id, sType);
		else
			sym = new Symbol(id, type);
		Symbol *symbol = stmgr->addSymbol(sym);
		if(symbol == nullptr){
			errors.addSemanticError(s->getStart(), "Duplicate variable: " + id);
		} else {
			//bindings
			bindings->bind(s, symbol);
			sym->defined = isDef;
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
	Scope *sc = stmgr->getCurrentScope2();
	this ->visitChildren(ctx);
	stmgr-> exitScope();
	return sc;
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
				symbol->defined = true;
			}	
		}
	}
	return type;
}

std::any SemanticVisitor::visitFunction(WPLParser::FunctionContext *ctx){
	ctx->fh->accept(this);
	Scope* currScope = stmgr->getCurrentScope2();
	Scope* blkScope = std::any_cast<Scope*>(ctx->b->accept(this));

	stmgr->setCurrentScope(blkScope); //SET SCOPE INSIDE BLOCK
			    

	SymBaseType fType = std::any_cast<SymBaseType>(ctx->fh->t->accept(this));
	for(WPLParser::StatementContext *st : ctx->b->s){
		if(st->r != nullptr){
			if(st->r->ex != nullptr){
				SymBaseType retTy = std::any_cast<SymBaseType>(st->r->ex->accept(this));
				if(retTy != fType){
					errors.addSemanticError(ctx->getStart(), "Incorrect return type");
				} 
			} else {
				errors.addSemanticError(ctx->getStart(), "Incorrect return type:  ");
			}
		}
	}

	//check last return in function
	if(ctx->b->s.size() != 0){
		WPLParser::StatementContext *s = ctx->b->s[ctx->b->s.size()-1];

		if(s == nullptr || s->r == nullptr){
			errors.addSemanticError(ctx -> getStart(), "No return as last in function: ");
		}
	} else {
		errors.addSemanticError(ctx->getStart(), "No return in function: ");
	}
	stmgr->setCurrentScope(currScope); //SET SCOPE BACK 
	stmgr->exitScope(); // entered in the header
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
                                symbol->defined = true;
                        }
		}
	}
	return nullptr;
}

std::any SemanticVisitor::visitProcedure(WPLParser::ProcedureContext *ctx){
	this->visitChildren(ctx);
	SymBaseType pType = SymBaseType::UNDEFINED; 
	for(WPLParser::StatementContext *st : ctx->b->s){
		if(st->r != nullptr){
			if(st->r->ex != nullptr){
				SymBaseType rType = std::any_cast<SymBaseType>(st->r->ex->accept(this));
				if(rType != pType)
					errors.addSemanticError(ctx -> getStart(), "Incorrect return type");
			} 
		}
	}

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
	bindings->bind(ctx, sym);
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
		std::vector<Param*> *params = s->params;
		//CHECK IF THEY HAVE EQUAL TYPES
		if(ctx->a != nullptr && params != nullptr){
			unsigned int size = params->size();
			std::vector<SymBaseType> *args = std::any_cast<std::vector<SymBaseType>*>(ctx->a->accept(this));
			if(args->size() != size){
				errors.addSemanticError(ctx->getStart(), "Invalid number of parameters: " + id);
			} else if(ctx->a != nullptr){
				for(unsigned int i = 0; i < size; i++){
					if((*params)[i]->baseType != (*args)[i]){
						errors.addSemanticError(ctx->getStart(), "Invalid parameter type: " + Symbol::getSymBaseTypeName((*args)[i]) + ", Expected: " + Symbol::getSymBaseTypeName((*params)[i]->baseType) + " for parameter " + (*params)[i]->id);
					}
				}

			}
		} else if(!(ctx->a == nullptr && params == nullptr)){
			//one has 0 the other does not
			errors.addSemanticError(ctx->getStart(), "Invalid number of parameters: " + id);
		}
	}
	return nullptr;
}

std::any SemanticVisitor::visitFuncCallExpr(WPLParser::FuncCallExprContext *ctx){
	std::string id = ctx->fname->getText();
	Symbol *s = stmgr->findSymbol(id);
	SymBaseType type = SymBaseType::UNDEFINED;
	//not function
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

		if(s->params != nullptr){
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
		} else if(!(ctx->args.size() ==0  && s->params == nullptr)){
			//not both no params
			errors.addSemanticError(ctx->getStart(), "Invalid number of parameters: " + id);
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
			errors.addSemanticError(ctx -> getStart(), "Use of undefined variable:    " + id);
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

	if(len <= 0){
		errors.addSemanticError(ctx->getStart(), "Invalid array length: " + id + ", " + ctx->i->getText());
	} else {
		Symbol *sym = new Symbol(id, t, len);
		Symbol *symbol = stmgr->addSymbol(sym);
		
		if(symbol == nullptr){
			errors.addSemanticError(ctx->getStart(), "Duplicate variable: " + id);
		} else {
			bindings->bind(ctx, sym);
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
		if(i < 0){
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
		errors.addSemanticError(ctx -> getStart(), "Use of undefined variable     : " + id);
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
		errors.addSemanticError(ctx -> getStart(), "Use of undefined variable:    " + id);
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
	if(symbol != nullptr && symbol->defined){
		bindings -> bind(ctx, symbol);
		t = symbol->baseType;
	} else {
		errors.addSemanticError(ctx -> getStart(), "Use of undefined variable:    " + id);
	}
	return t; 
}

//assignment with left and right: var1 <- var2
std::any SemanticVisitor::visitAssignment(WPLParser::AssignmentContext *ctx){
	SymBaseType right = std::any_cast<SymBaseType>(ctx->e->accept(this));

	SymBaseType left = SymBaseType::UNDEFINED;
	std::string id = "";
	if(ctx->target != nullptr)
		id = ctx -> target -> getText();
	else
		id = ctx->arr->id->getText();
	Symbol* symbol = stmgr-> findSymbol(id);
	if(symbol != nullptr){
		bindings -> bind(ctx, symbol);
		symbol->defined = true;
		left = symbol->baseType;

		if(left == SymBaseType::UNDEFINED){
			//VAR DECLARATION
			symbol->baseType = right;
		} else if(left != right){
			errors.addSemanticError(ctx -> getStart(), "Type mismatch on assignment: " + Symbol::getSymBaseTypeName(left) + ", " + Symbol::getSymBaseTypeName(right));
		}


	} else {
		errors.addSemanticError(ctx -> getStart(), "Use of undefined variable: " + id);
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

