
// Generated from WPL.g4 by ANTLR 4.10.1

#pragma once


#include "antlr4-runtime.h"
#include "WPLBaseVisitor.h"

#include "PropertyManager.h"
#include "ErrorHandler.h"
#include "SemanticVisitor.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/NoFolder.h"


/**
 * This class provides an empty implementation of WPLVisitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
using namespace llvm;
class  CodegenVisitor: public WPLBaseVisitor {

	public:

		CodegenVisitor(PropertyManager *pm, std::string moduleName)
		{
			props = pm;
			context = new LLVMContext();
			module = new Module(moduleName, *context);
			// Use the NoFolder to turn off constant folding
			builder = new IRBuilder<NoFolder>(module->getContext());

			// cached types
			VoidTy = Type::getVoidTy(module->getContext());
			Int32Ty = Type::getInt32Ty(module->getContext());
			Int1Ty = Type::getInt1Ty(module->getContext());
			Int8Ty = Type::getInt8Ty(module->getContext());
			Int32Zero = ConstantInt::get(Int32Ty, 0, true);
			Int32One = ConstantInt::get(Int32Ty, 1, true);
			i8p = Type::getInt8PtrTy(module->getContext());
			Int8PtrPtrTy = i8p->getPointerTo();
		}
		std::any visitCompilationUnit(WPLParser::CompilationUnitContext *ctx) override;

		//  std::any visitCuComponent(WPLParser::CuComponentContext *ctx) override;

		//  std::any visitVarDeclaration(WPLParser::VarDeclarationContext *ctx) override;

		std::any visitScalarDeclaration(WPLParser::ScalarDeclarationContext *ctx) override;

		//  std::any visitScalar(WPLParser::ScalarContext *ctx) override;

		//  std::any visitArrayDeclaration(WPLParser::ArrayDeclarationContext *ctx) override;

		//  std::any visitType(WPLParser::TypeContext *ctx) override;

		//  std::any visitVarInitializer(WPLParser::VarInitializerContext *ctx) override;

		//  std::any visitExternDeclaration(WPLParser::ExternDeclarationContext *ctx) override;

		//  std::any visitProcedure(WPLParser::ProcedureContext *ctx) override;

		//  std::any visitProcHeader(WPLParser::ProcHeaderContext *ctx) override;

		//  std::any visitExternProcHeader(WPLParser::ExternProcHeaderContext *ctx) override;

		//  std::any visitFunction(WPLParser::FunctionContext *ctx) override;

		//  std::any visitFuncHeader(WPLParser::FuncHeaderContext *ctx) override;

		//  std::any visitExternFuncHeader(WPLParser::ExternFuncHeaderContext *ctx) override;

		//  std::any visitParams(WPLParser::ParamsContext *ctx) override;

		//  std::any visitParam(WPLParser::ParamContext *ctx) override;

		//  std::any visitBlock(WPLParser::BlockContext *ctx) override;

		//  std::any visitStatement(WPLParser::StatementContext *ctx) override;

		//  std::any visitLoop(WPLParser::LoopContext *ctx) override;

		//  std::any visitConditional(WPLParser::ConditionalContext *ctx) override;

		//  std::any visitSelect(WPLParser::SelectContext *ctx) override;

		//  std::any visitSelectAlt(WPLParser::SelectAltContext *ctx) override;

		//  std::any visitCall(WPLParser::CallContext *ctx) override;

		//  std::any visitArguments(WPLParser::ArgumentsContext *ctx) override;

		//  std::any visitArg(WPLParser::ArgContext *ctx) override;

		//  std::any visitReturn(WPLParser::ReturnContext *ctx) override;

		std::any visitConstant(WPLParser::ConstantContext *ctx) override;

		//  std::any visitAssignment(WPLParser::AssignmentContext *ctx) override;

		//  std::any visitArrayIndex(WPLParser::ArrayIndexContext *ctx) override;

		//  std::any visitAndExpr(WPLParser::AndExprContext *ctx) override;

		std::any visitIDExpr(WPLParser::IDExprContext *ctx) override;

		//std::any visitConstExpr(WPLParser::ConstExprContext *ctx) override;

		//  std::any visitSubscriptExpr(WPLParser::SubscriptExprContext *ctx) override;

		std::any visitRelExpr(WPLParser::RelExprContext *ctx) override;

		//  std::any visitMultExpr(WPLParser::MultExprContext *ctx) override;

		std::any visitUnaryNotExpr(WPLParser::UnaryNotExprContext *ctx) override;

		//  std::any visitAddExpr(WPLParser::AddExprContext *ctx) override;

		//  std::any visitArrayLengthExpr(WPLParser::ArrayLengthExprContext *ctx) override;

		//  std::any visitOrExpr(WPLParser::OrExprContext *ctx) override;

		std::any visitEqExpr(WPLParser::EqExprContext *ctx) override;

		//  std::any visitParenExpr(WPLParser::ParenExprContext *ctx) override;

		//  std::any visitFuncCallExpr(WPLParser::FuncCallExprContext *ctx) override;

		std::any visitUnaryMinusExpr(WPLParser::UnaryMinusExprContext *ctx) override;

		std::string getErrors() { return errors.errorList(); }
		// STManager *getSTManager() { return stmgr; }
		PropertyManager *getProperties() { return props; }
		bool hasErrors() { return errors.hasErrors(); }
		llvm::Module *getModule() { return module; }
		void modPrint() { module -> print(llvm::outs(), nullptr); }
	private:
		PropertyManager *props;
		ErrorHandler errors;

		LLVMContext *context;
		Module *module;
		IRBuilder<NoFolder> *builder;

		// Cache commonly used types
		Type *VoidTy;
		Type *Int1Ty;
		Type *Int8Ty;
		Type *Int32Ty;
		Type * i8p;
		Type *Int8PtrPtrTy;
		Constant *Int32Zero;
		Constant *Int32One;

};
