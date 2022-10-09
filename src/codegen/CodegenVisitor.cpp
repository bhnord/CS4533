#include "CodegenVisitor.h"


std::any CodegenVisitor::visitCompilationUnit(WPLParser::CompilationUnitContext *ctx) {
  // 1. Declare external functions
  auto printf_prototype = FunctionType::get(i8p, true);
  auto printf_fn = Function::Create(printf_prototype, Function::ExternalLinkage, "printf", module);
  FunctionCallee printExpr(printf_prototype, printf_fn);

  // 2. Define the main program.
  FunctionType *mainFuncType = FunctionType::get(Int32Ty, {Int32Ty, Int8PtrPtrTy}, false);
  Function *mainFunc = Function::Create(mainFuncType,     GlobalValue::ExternalLinkage,
    "main", module);

 // 3. Create a basic block and attach it to the builder.
  BasicBlock *bBlock = BasicBlock::Create(module->getContext(), "entry", mainFunc);
  builder->SetInsertPoint(bBlock);




    // 4. Generate code for all expressions in the block.
  // COMING SOON to a code generator near you!
 for(auto e: ctx-> e){
	 //eval and print e
	 Value *exprResult = std::any_cast<Value *>(e->accept(this));
	 auto et = e->getText(); 
	 StringRef formatRef = "Expression %s evaluates to %d\n";
	 auto gFormat = builder->CreateGlobalStringPtr(formatRef, "fmtStr");
	 StringRef exprRef = et;
	 auto exFormat = builder->CreateGlobalStringPtr(exprRef, "exprStr");
	 builder->CreateCall(printf_fn, {gFormat, exFormat, exprResult});
	 
 }




  builder->CreateRet(Int32Zero);
  return nullptr;
}


std::any CodegenVisitor::visitConstant(WPLParser::ConstantContext *ctx){
	Value *v = nullptr;
	if(ctx->i != nullptr){
	int i = std::stoi(ctx->i->getText());
	v = builder->getInt32(i);
	} else if(ctx->i != nullptr){
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


std::any CodegenVisitor::visitScalarDeclaration(WPLParser::ScalarDeclarationContext *ctx){
	Value *exVal = nullptr;
	//eventually fix below!!! ----------------------------------
	Symbol *varSymbol = props->getBinding(ctx->scalars[0]);
	Value *v = nullptr;
	if (!(varSymbol->defined)) {
//		// Define the symbol and allocate memory.
		v = builder->CreateAlloca(CodegenVisitor::Int32Ty, 0, varSymbol->id);
//		varSymbol->defined = true;
//		varSymbol->val = v;
	} else {
		v = varSymbol->val;
	}

	builder->CreateStore(exVal, v);
	return exVal;
}