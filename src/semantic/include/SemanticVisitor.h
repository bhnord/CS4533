#pragma once
#include "antlr4-runtime.h"
#include "WPLBaseVisitor.h"
#include "STManager.h"
#include "PropertyManager.h"
#include "ErrorHandler.h"

class SemanticVisitor : WPLBaseVisitor {
  public :
    // Pass in the appropriate elements
    SemanticVisitor(STManager* stm, PropertyManager* pm) {
      stmgr = stm;
      bindings = pm;
    }

    std::any visitCompilationUnit(WPLParser::CompilationUnitContext*ctx) override;
    std::any vistScalarDeclaration(WPLParser::ScalarDeclarationContext *ctx);
    std::any visitType(WPLParser::TypeContext *ctx) override;
    std::any visitBlock(WPLParser::BlockContext *ctx) override;
    std::any visitFuncHeader(WPLParser::FuncHeaderContext *ctx) override;
    std::any visitParams(WPLParser::ParamsContext *ctx) override;
    std::any visitParam(WPLParser::ParamContext *ctx);



    //////////////////////////
    //std::any visitCompilationUnit(WPLParser::ProgramContext *ctx);
    //std::any visitBooleanConstant(WPLParser::BooleanConstantContext *ctx);
    //std::any visitIConstExpr(WPLParser::IConstExprContext *ctx);
    std::any visitParenExpr(WPLParser::ParenExprContext *ctx) override;
    std::any visitUnaryMinusExpr(WPLParser::UnaryMinusExprContext *ctx) override;
    std::any visitUnaryNotExpr(WPLParser::UnaryNotExprContext *ctx) override;
    //std::any visitBinaryArithExpr(WPLParser::BinaryArithExprContext *ctx);
    //std::any visitBinaryRelExpr(WPLParser::BinaryRelExprContext *ctx);
    std::any visitEqExpr(WPLParser::EqExprContext *ctx) override;
    //std::any visitAssignExpression(WPLParser::AssignExpressionContext *ctx);
    //std::any visitVariableExpr(WPLParser::VariableExprContext *ctx);

    std::string getErrors() { return errors.errorList(); }
    STManager* getSTManager() { return stmgr; }
    PropertyManager* getBindings() { return bindings; }
    bool hasErrors() { return errors.hasErrors(); }

  private: 
    STManager* stmgr;
    PropertyManager* bindings; 
    ErrorHandler errors;
};
