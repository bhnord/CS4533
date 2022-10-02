#pragma once
#include "Symbol.h"
#include "antlr4-runtime.h"



class PropertyManager : antlr4::tree::ParseTreeProperty<Symbol*>
{
	public:
		// Get the Symbol associated with this node
		Symbol* getBinding(antlr4::ParserRuleContext *ctx) {
			return get(ctx);
		}
		// Bind the symbol to the node
		void bind(antlr4::ParserRuleContext *ctx, Symbol* symbol) {
			put(ctx, symbol);
		}
		std::string toString() {
			std::ostringstream os;
			for (auto b : _annotations) {
				antlr4::tree::ParseTree *pt = b.first;
				Symbol* s = b.second;
				antlr4::Token* token = nullptr;
				if (pt->getTreeType() ==
						antlr4::tree::ParseTreeType::TERMINAL) {
					token = ((antlr4::tree::TerminalNode*)pt) ->getSymbol();
					os << "(" << token->getLine() << "," <<
						token->getCharPositionInLine()
						<< "): " << s->toString() << std::endl;
				} else {
					token = ((antlr4::ParserRuleContext*)pt) ->
						start;
					os << "(" << token->getLine() << "," <<
						token->getCharPositionInLine()
						<< "): " << s->toString() << std::endl;
				}
			}
			return os.str();
		}
	private:
		antlr4::tree::ParseTreeProperty<Symbol*> bindings;
};
