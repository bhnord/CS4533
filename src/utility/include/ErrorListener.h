class ErrorListener: public antlr4::BaseErrorListener {
static antlr4::ConsoleErrorListener INSTANCE;
  bool error = false;
  virtual void syntaxError(
      antlr4::Recognizer *recognizer,
      antlr4::Token *offendingSymbol,
      size_t line,
      size_t charPositionInLine,
      const std::string &msg,
      std::exception_ptr e) override {
	  //line <em>line</em>:<em>charPositionInLine</em> <em>msg</em>
      std::cerr<<"line " << line << ":" << charPositionInLine << " " << msg <<std::endl;
      error = true;
  }
	public:
  virtual bool hasErrors(){
	  return error;
  }
};

