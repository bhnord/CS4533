/*
 * Grammar file for WPI Programming Language (WPL).
 */
grammar WPL;

// Parser rules
compilationUnit   : e+=cuComponent+ EOF;

cuComponent       : varDeclaration | procedure | function | externDeclaration ;
varDeclaration    : scalarDeclaration | arrayDeclaration ;
scalarDeclaration : (t=type| VAR) scalars+=scalar (',' scalars+=scalar)* ';' ;
scalar            : id=ID varInitializer? ;
arrayDeclaration  : t=type '[' i=INTEGER ']' id=ID ';' ;       // No dynamic arrays, type not inferred
type              : b=BOOL | i=INT | s=STR ;  
varInitializer    : '<-' c=constant ;
externDeclaration : 'extern' (externProcHeader | externFuncHeader) ';';

procedure         : procHeader block ;
procHeader        : 'proc' id=ID '(' p=params? ')' ;
externProcHeader  : 'proc' id=ID '(' ((params ',' ELLIPSIS) | params? | ELLIPSIS?) ')' ;
function          : fh=funcHeader block  ;
funcHeader        : t=type 'func' id=ID '(' p=params? ')' ;
externFuncHeader  : t=type 'func' id=ID '(' ((params ',' ELLIPSIS) | params? | ELLIPSIS?) ')' ;

params            : (p+=param (',' p+=param)*) ;
param		  : t=type id=ID;

///
block            : '{' (statement | varDeclaration | block)+ '}' ;   // Change to expr ???

statement         : assignment
                  | loop
                  | select
                  | conditional
                  | call
                  | block
                  | return
                  ;

loop              : 'while' e=expr 'do' block ;
conditional       : 'if' e=expr 'then'? block ('else' block)? ;
select            : 'select' '{' selectAlt+ '}' ;
selectAlt         : e=expr ':' s=statement ;  
call              : id=ID '(' arguments? ')' ';' ;
arguments         : (arg (',' arg)*) ;  
arg               : (id=ID | c=constant) ; 
return            : 'return' expr? ';' ;

constant          : i=INTEGER | s=STRING | b=BOOLEAN ;
assignment        : target=ID '<-' e=expr ';' 
                  | arr=arrayIndex '<-' e=expr ';' ;
arrayIndex        : id=ID '[' ex =expr ']' ;

expr              : 
                  fname=ID '(' (args+=expr (',' args+=expr)*)? ')'       # FuncCallExpr
                  | arrayIndex                                            # SubscriptExpr
                  | <assoc=right> '-' ex=expr                                # UnaryMinusExpr
                  | <assoc=right> '~' ex=expr                                # UnaryNotExpr
                  | left=expr (MUL | DIV) right=expr                      # MultExpr
                  | left=expr (PLUS | MINUS) right=expr                   # AddExpr
                  | left=expr (LESS | LEQ | GTR | GEQ) right=expr         # RelExpr
                  | <assoc=right> left=expr (EQUAL | NEQ) right=expr      # EqExpr
                  | left=expr AND right=expr                              # AndExpr
                  | left=expr OR right=expr                               # OrExpr
                  | '(' ex = expr ')'                                          # ParenExpr
                  | arrayname=ID '.' 'length'                             # ArrayLengthExpr
                  | id=ID                                                    # IDExpr
                  | c=constant                                              # ConstExpr
                  ;

// Lexer rules
// Keywords
BOOL              : 'boolean' ;
INT               : 'int' ;
STR               : 'str' ;
VAR               : 'var' ;

PROC              : 'proc' ;
FUNC              : 'func' ;
EXTERN            : 'extern' ;
RETURN            : 'return' ;
WHILE             : 'while' ;
DO                : 'do' ;
SELECT            : 'select' ;
END               : 'end' ;
IF                : 'if' ;
THENN             : 'then' ;
ELSE              : 'else' ;
LENGTH            : 'length' ;

// Operators
AND               : '&' ;
ASSIGN            : '<-' ;
EQUAL					    :	'=' ;
GEQ               : '>=' ;
GTR               : '>' ;
LEQ               : '<=' ;
LESS					    :	'<' ;
MINUS					    :	'-' ;
NEQ               : '~=' ;
OR                : '|' ;
PLUS					    :	'+' ;
DIV 					    :	'/' ;
MUL 					    :	'*' ;
NOT 					    :	'~' ;

// Punctuation and delimiters
COMMA             : ',' ;
SEMICOLON         : ';' ;
ELLIPSIS          : '...' ;
DOT               : '.' ;

LBRACKET          : '[' ;
RBRACKET          : ']' ;
LPAR              : '(' ;
RPAR              : ')' ;
LBRACE					  : '{' ;
RBRACE					  :	'}' ;

// Literals, identifiers, and whitespace
WS 						    :	[ \t\r\n\f]+ -> skip ;
INTEGER					  :	'-'? DIGIT+ ;
BOOLEAN           : 'true' | 'false' ;
ID      				  : LETTER (LETTER|DIGIT|UNDERSCORE)* ;
STRING			      : '"' ('\\'. | ~[\n])*? '"' ;

// Comments
COMMENT				     : 	(INLINE_COMMENT|STD_COMMENT) -> skip ;
fragment INLINE_COMMENT  :	'#' .*? ('\n'|EOF);
fragment STD_COMMENT	 :	'(*' (STD_COMMENT | .)*? '*)' ;
fragment UNDERSCORE      :   '_' ;

// Fragments
fragment DIGIT			:	[0-9] ;
fragment UPPER          :   [A-Z] ;
fragment LOWER          :   [a-z] ;
fragment LETTER			:	UPPER | LOWER ;

