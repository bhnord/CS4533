#include "CodegenVisitor.h"


std::any CodegenVisitor::visitCompilationUnit(WPLParser::CompilationUnitContext *ctx) {
	// 1. Declare external functions
//	auto printf_prototype = FunctionType::get(i8p, true);
//	auto printf_fn = Function::Create(printf_prototype, Function::ExternalLinkage, "printf", module);
//	FunctionCallee printExpr(printf_prototype, printf_fn);

	// 2. Define the main program.
//	FunctionType *mainFuncType = FunctionType::get(CodegenVisitor::Int32Ty, {CodegenVisitor::Int32Ty, CodegenVisitor::Int8PtrPtrTy}, false);
//	Function *mainFunc = Function::Create(mainFuncType,     GlobalValue::ExternalLinkage,
//			"main", module);

	

//	//	// 3. Create a basic block and attach it to the builder.
//	BasicBlock *bBlock = BasicBlock::Create(module->getContext(), "entry", mainFunc);
//	builder->SetInsertPoint(bBlock);



	// 4. Generate code for all expressions in the block.
	// COMING SOON to a code generator near you!
	for(auto e: ctx-> e){
		e->accept(this);
		//	//eval and print e
		//        Value *exprResult = std::any_cast<Value *>(e->accept(this));
		//	auto et = e->getText(); 
		//	StringRef formatRef = "Expression %s evaluates to %d\n";
		//	auto gFormat = builder->CreateGlobalStringPtr(formatRef, "fmtStr");
		//	StringRef exprRef = et;
		//	auto exFormat = builder->CreateGlobalStringPtr(exprRef, "exprStr");
		//	builder->CreateCall(printf_fn, {gFormat, exFormat, exprResult});

	}




//	builder->CreateRet(CodegenVisitor::Int32Zero);
	return nullptr;
}


std::any CodegenVisitor::visitConstant(WPLParser::ConstantContext *ctx){
	Value *v = nullptr;
	if(ctx->i != nullptr){
		int i = std::stoi(ctx->i->getText());
		v = builder->getInt32(i);
	} else if(ctx->b != nullptr){
		if(ctx->b->getText() == "true"){
			v = builder->getInt32(1);
		} else {
			v = builder->getInt32(0);
		}
	} ///STILL NEED TO DO STRING!!
	return v;


}

std::any CodegenVisitor::visitIDExpr(WPLParser::IDExprContext *ctx){
	std::string varId = ctx->id->getText(); 
	Symbol *symbol = props->getBinding(ctx);
	Value *v = nullptr;
	// We made sure that the variable is defined in the semantic analysis phase
	if (!(symbol->defined)) {
		errors.addCodegenError(ctx->getStart(), "Undefined variable in expression: " + varId);
	} else {
		v = builder->CreateLoad(CodegenVisitor::Int32Ty, symbol->val, varId);
	}
	return v;
}


std::any CodegenVisitor::visitScalar(WPLParser::ScalarContext *ctx){
	Value *v = nullptr;
	Value *exVal = nullptr;

	//globals
	if(ctx->v != nullptr){

		bool inFunc = builder->GetInsertBlock() != nullptr;

		exVal =  std::any_cast<Value *>(ctx->v->accept(this));
		Symbol *varSymbol = props->getBinding(ctx);  // child variable symbol
		if (varSymbol == nullptr) {
			errors.addCodegenError(ctx->getStart(), "Undefined variable in expression: " + ctx->id->getText());
		}
		if (!(varSymbol->defined)) {
			// Define the symbol and allocate memory.
			if(!inFunc){
				module->getOrInsertGlobal(varSymbol->id, CodegenVisitor::Int32Ty);
				GlobalVariable *g = module->getNamedGlobal(varSymbol->id);
				g->setLinkage(GlobalValue::CommonLinkage);
				g->setInitializer(Int32Zero);
				g->setAlignment(Align(4));
				v = g;
			} else {
				v = builder->CreateAlloca(CodegenVisitor::Int32Ty, 0, varSymbol->id);
			}
			varSymbol->defined = true;
			varSymbol->val = v;
		} else {
			v = varSymbol->val;
		}
		if(inFunc)
			builder->CreateStore(exVal, v);
	}

	return exVal;
}

std::any CodegenVisitor::visitScalarDeclaration(WPLParser::ScalarDeclarationContext *ctx){
	return ctx->scalars[0]->accept(this);
}

//eventually fix below!!! (does not work1!!)----------------------------------
//for(WPLParser::ScalarContext *c: ctx->scalars){
//	Symbol *varSymbol = props->getBinding(c);
//	Value *v = nullptr;
//	if(varSymbol == nullptr){
//		errors.addCodegenError(c->getStart(), "semantic error");
//		return nullptr;
//	}
//	if (!(varSymbol->defined)) {
//		//		// Define the symbol and allocate memory.
//		v = builder->CreateAlloca(CodegenVisitor::Int32Ty, 0, varSymbol->id);
//		varSymbol->defined = true;
//		varSymbol->val = v;
//	} else {
//		v = varSymbol->val;
//	}

//	builder->CreateStore(exVal, v);
//}
//	return exVal;



//subs from zero for unary minus
std::any CodegenVisitor::visitUnaryMinusExpr(WPLParser::UnaryMinusExprContext *ctx){
	Value *exVal = std::any_cast<Value *>(ctx->ex->accept(this));
	Value *v = builder->CreateNSWSub(builder->getInt32(0), exVal);
	return v;
}


//relational expression
//LESS | LEQ | GTR | GEQ
std::any CodegenVisitor::visitRelExpr(WPLParser::RelExprContext *ctx){
	Value *lVal = std::any_cast<Value *>(ctx->left->accept(this));
	Value *rVal = std::any_cast<Value *>(ctx->right->accept(this));
	auto op = ctx->op->getType();
	Value *v1;
	if(op == WPLParser::LESS){
		v1 = builder->CreateICmpSLT(lVal, rVal);
	} else if(op == WPLParser::GTR){
		v1 = builder->CreateICmpSGT(lVal, rVal);
	} else if(op == WPLParser::LEQ){
		v1 = builder->CreateICmpSLE(lVal, rVal);
	} else{
		v1 = builder->CreateICmpSGE(lVal, rVal);
	}

	Value *v = builder->CreateZExtOrTrunc(v1, CodegenVisitor::Int32Ty);
	return v;
}

//unary not (t/f)
std::any CodegenVisitor::visitUnaryNotExpr(WPLParser::UnaryNotExprContext *ctx){
	Value *v = std::any_cast<Value *>(ctx->ex->accept(this));
	v = builder->CreateZExtOrTrunc(v, CodegenVisitor::Int1Ty);
	v = builder->CreateXor(v, Int32One);
	v = builder->CreateZExtOrTrunc(v, CodegenVisitor::Int32Ty);
	return v;
}


//equality expression (eq or neq)
std::any CodegenVisitor::visitEqExpr(WPLParser::EqExprContext *ctx){
	Value *v = nullptr;
	Value *lVal = std::any_cast<Value *>(ctx->left->accept(this));
	Value *rVal = std::any_cast<Value *> (ctx->right->accept(this));
	auto op = ctx->op->getType();
	Value *v1;
	if(op == WPLParser::EQUAL){
		v1 = builder->CreateICmpEQ(lVal, rVal);
	} else{ //neq 
		v1 = builder->CreateICmpNE(lVal, rVal);
	}

	v = builder->CreateZExtOrTrunc(v1, Int32Ty);
	return v;
}

//expr addition
std::any CodegenVisitor::visitAddExpr(WPLParser::AddExprContext *ctx){
	Value *v = nullptr;
	Value *lVal = std::any_cast<Value *>(ctx->left->accept(this));
	Value *rVal = std::any_cast<Value *>(ctx->right->accept(this));
	auto opType = ctx->op->getType();
	if(opType == WPLParser::PLUS){
		v = builder->CreateNSWAdd(lVal, rVal);
	} else {
		v = builder->CreateNSWSub(lVal, rVal);
	}
	return v;
}

//multiplication
std::any CodegenVisitor::visitMultExpr(WPLParser::MultExprContext *ctx){
	Value *v = nullptr;
	Value *lVal = std::any_cast<Value *>(ctx->left->accept(this));
	Value *rVal = std::any_cast<Value *>(ctx->right->accept(this));
	auto opType = ctx->op->getType();
	if(opType == WPLParser::MUL){
		v = builder->CreateNSWMul(lVal, rVal);
	} else {
		v = builder->CreateSDiv(lVal, rVal);
	}
	return v;
}


//and expression
std::any CodegenVisitor::visitAndExpr(WPLParser::AndExprContext *ctx){
	Value *v = nullptr;
	Value *lVal = std::any_cast<Value *>(ctx->left->accept(this));
	Value *rVal = std::any_cast<Value *>(ctx->right->accept(this));
	v = builder->CreateICmpEQ(lVal, rVal);
	return v;
}

//or expression ---> shortcut out if left is true 
std::any CodegenVisitor::visitOrExpr(WPLParser::OrExprContext *ctx){
	Value *lVal = std::any_cast<Value *>(ctx->left->accept(this));
	//
	//
	//	//blocks and setup
	Function *theFunc = builder->GetInsertBlock()->getParent();
	BasicBlock *currBlock = builder->GetInsertBlock();
	BasicBlock *endBlock = BasicBlock::Create(*context, "cont", theFunc);
	BasicBlock *rBlock = BasicBlock::Create(*context, "right", theFunc);

	Value *t = builder->CreateICmpEQ(lVal, builder->getInt32(1));
	builder->CreateCondBr(t, endBlock, rBlock);
	//	
	//	//rblock
	builder->SetInsertPoint(rBlock);
	Value *rVal = std::any_cast<Value *>(ctx->right->accept(this));
	builder->CreateBr(endBlock);
	//
	//	//endblock
	builder->SetInsertPoint(endBlock);
	PHINode *PN = builder->CreatePHI(Int32Ty, 2, "ortmp");
	PN->addIncoming(rVal, rBlock);
	PN->addIncoming(lVal, currBlock);
	Value * v = PN;
	return v;

}

/// test this -------------------------------------------------------------------------------------------------------
std::any CodegenVisitor::visitArrayLengthExpr(WPLParser::ArrayLengthExprContext *ctx){
	Value *v = nullptr;
	Symbol *symbol = props->getBinding(ctx);
	v = builder->getInt32(symbol->length);
	return v;
}

//create procedure
std::any CodegenVisitor::visitProcedure(WPLParser::ProcedureContext *ctx) {
	BasicBlock *main = builder->GetInsertBlock();
	Function *proc = std::any_cast<Function *>(ctx->ph->accept(this));
	BasicBlock *bBlock = BasicBlock::Create(*context, ctx->ph->id->getText() + "head", proc);


	builder->SetInsertPoint(bBlock);
	BasicBlock *b = std::any_cast<BasicBlock *>(ctx->b->accept(this));
	//ret can be created inside block
	builder->CreateRet(nullptr);

	builder->SetInsertPoint(bBlock);
	builder->CreateBr(b);
	////EVENTUALLY REMOVE THIS !!!!!!!!

	builder->SetInsertPoint(main);

	return nullptr;

}

//procedure header and func setup
std::any CodegenVisitor::visitProcHeader(WPLParser::ProcHeaderContext *ctx){
	Symbol * sym = props->getBinding(ctx);

	std::vector<Type *> *types = new std::vector<Type*>;

	std::vector<Param*> *params = nullptr;
	if(sym->params != nullptr){
		params = sym->params;
		for(Param *param: *params){
			if(SymBaseType::INT == param->baseType || SymBaseType::BOOL == param->baseType){
				types->push_back(CodegenVisitor::Int32Ty);
			} else {
				types->push_back(CodegenVisitor::Int8PtrPtrTy);
			}
		}
	}


	FunctionType *funcType = FunctionType::get(CodegenVisitor::VoidTy, *types,false);
	Function *func = Function::Create(funcType, Function::ExternalLinkage, sym->id, module);

	sym->func = func;
	unsigned Idx = 0;
	for (auto &Arg : func->args())
		Arg.setName(((*(sym->params))[Idx++])->id);



	return func;
}


//somehow set params in memory
std::any CodegenVisitor::visitFuncHeader(WPLParser::FuncHeaderContext *ctx){
	Symbol * sym = props->getBinding(ctx);
	FunctionType *funcType = nullptr;

	std::vector<Type *> *types = new std::vector<Type*>;

	std::vector<Param*> *params = nullptr;
	if(sym->params != nullptr){
		params = sym->params;
		for(Param *param: *params){
			if(SymBaseType::INT == param->baseType || SymBaseType::BOOL == param->baseType){
				types->push_back(CodegenVisitor::Int32Ty);
			} else {
				types->push_back(CodegenVisitor::Int8PtrPtrTy);
			}
		}
	}
	if(sym->baseType == INT || sym->baseType == BOOL){
		funcType = FunctionType::get(CodegenVisitor::Int32Ty, *types,false);
	}else{ //string
	       //	funcType = FunctionType::get(CodegenVisitor::Int8PtrPtrTy, {CodegenVisitor::Int32Ty, CodegenVisitor::Int8PtrPtrTy}, false); 
	}
	Function *func = Function::Create(funcType, Function::ExternalLinkage, sym->id, module);

	if(ctx->p != nullptr){
		std::vector<WPLParser::ParamContext *> p = ctx->p->p;
		sym->func = func;

		//name params
		unsigned Idx = 0;
		for (auto &Arg : func->args())
			Arg.setName(((*(sym->params))[Idx++])->id);


		//map params to args (intermediary -- will alloc params later)
		Function::arg_iterator args = func->arg_begin();
		for (WPLParser::ParamContext * px: p){
			Value *v = args++;

			Symbol *sym = props->getBinding(px);

			sym->val = v;
			sym->defined = true;

		}
	}

	return func;
}

//IS THIS CORRECT?? ---------------- YOU ARE IN CHARGE OF GOING BACK!!
std::any CodegenVisitor::visitBlock(WPLParser::BlockContext *ctx){
	Function *theFunc = builder->GetInsertBlock()->getParent();
	BasicBlock *back = builder->GetInsertBlock();
	BasicBlock *bBlock = BasicBlock::Create(*context, "block", theFunc);

	//PROBABLY TAKE OUT CREATEBR
	builder->SetInsertPoint(bBlock);
	this->visitChildren(ctx);
	//builder->CreateBr(back);

	//builder->SetInsertPoint(back);
	return bBlock;
}



//NEED TO ENSURE RETURN STATEMENT!!!!!!!!!!!!!
std::any CodegenVisitor::visitFunction(WPLParser::FunctionContext *ctx){
	BasicBlock *main = builder->GetInsertBlock();
	Function *func = std::any_cast<Function *>(ctx->fh->accept(this));
	BasicBlock *bBlock = BasicBlock::Create(*context, ctx->fh->id->getText() + "head", func);
	builder->SetInsertPoint(bBlock);

	//allocate all the params
	if(ctx->fh->p != nullptr)
		ctx->fh->p->accept(this); //visit all params


	BasicBlock *b = std::any_cast<BasicBlock *>(ctx->b->accept(this));
	//ret is created inside block

	builder->SetInsertPoint(bBlock);
	builder->CreateBr(b);

	builder->SetInsertPoint(main);

	return nullptr;
}

std::any CodegenVisitor::visitParam(WPLParser::ParamContext *ctx){
	Symbol *sym = props->getBinding(ctx);
	Value *val = sym->val;
	Value *alloc = builder->CreateAlloca(CodegenVisitor::Int32Ty, 0, sym->id);
 	builder->CreateStore(val, alloc);
	sym->val = alloc;
	sym->defined = true;

	return nullptr;

}

std::any CodegenVisitor::visitReturn(WPLParser::ReturnContext *ctx){
	Value *v = std::any_cast<Value *>(ctx->ex->accept(this));
	builder->CreateRet(v);
	return nullptr;
}




//call to function 
std::any CodegenVisitor::visitCall(WPLParser::CallContext *ctx){
	Symbol *s = props->getBinding(ctx);
	Function *func = s->func;
	std::vector<Value *> *args = new std::vector<Value *>;
	for(WPLParser::ArgContext *a : ctx->a->a){
		Value *v = std::any_cast<Value *>(a->accept(this));
		args->push_back(v);
	}
	builder->CreateCall(func, *args); 
	return nullptr;
}


std::any CodegenVisitor::visitConditional(WPLParser::ConditionalContext *ctx){
	Value *cond = std::any_cast<Value *>(ctx->e->accept(this));


	//blocks and setup
	Function *theFunc = builder->GetInsertBlock()->getParent();
	BasicBlock *currBlock = builder->GetInsertBlock();
	BasicBlock *tBlock = BasicBlock::Create(*context, "then", theFunc);
	BasicBlock *endBlock = BasicBlock::Create(*context, "cont", theFunc);
	BasicBlock *eBlock = nullptr;


	Value *t = builder->CreateICmpEQ(cond, builder->getInt32(1));
	if(ctx->b.size() > 1){
		BasicBlock *eBlock = BasicBlock::Create(*context, "else", theFunc);

		builder->CreateCondBr(t, tBlock, eBlock);

		//else block
		builder->SetInsertPoint(eBlock);
		BasicBlock *eb = std::any_cast<BasicBlock *>(ctx->b[1]->accept(this));
		builder->CreateBr(endBlock);
		builder->SetInsertPoint(eBlock);
		builder->CreateBr(eb);

	} else {
		builder->CreateCondBr(t, tBlock, endBlock);
	}

	//then block
	builder->SetInsertPoint(tBlock);
	BasicBlock *tb = std::any_cast<BasicBlock *>(ctx->b[0]->accept(this));
	builder->CreateBr(endBlock);
	builder->SetInsertPoint(tBlock);
	builder->CreateBr(tb);


	//      //endblock
	builder->SetInsertPoint(endBlock);


	return nullptr;

}


std::any CodegenVisitor::visitAssignment(WPLParser::AssignmentContext *ctx){

	Value *v = nullptr;
	Value *exVal = nullptr;
	exVal =  std::any_cast<Value *>(ctx->e->accept(this));
	Symbol *varSymbol = props->getBinding(ctx);  // child variable symbol
	if (varSymbol == nullptr) {
		errors.addCodegenError(ctx->getStart(), "Undefined variable in expression: " + ctx->target->getText());
	}
	v = varSymbol->val;
	builder->CreateStore(exVal, v);

	return exVal;

}


///all expr need return
std::any CodegenVisitor::visitParenExpr(WPLParser::ParenExprContext *ctx){
	return ctx->ex->accept(this);
}
