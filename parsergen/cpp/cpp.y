%{

  /* This part goes to CppParser.h */

#include "ParserTree.h"
#include "CppSymbol.h"
#include "CppLex.h"

typedef SyntaxNode *stype;

class CppParser : public LRparser, public ParserTree {
public:
  CppParser(CppLex *lex = NULL) : LRparser(*CppTables,lex) {}
  SyntaxNode *newNode( int token, ... );
  void  appendError(const char *format, ...);
  void	verror(const SourcePosition &pos, const char *format,va_list argptr);
private:
  stype m_dollardollar,*m_stacktop,m_userstack[256];
  int	reduceAction(unsigned int prod);
//void	traceStack(unsigned int prod);
  stype getStackTop(int fromtop)          { return m_stacktop[-fromtop];	  }
  void	initUserStack()                   { m_stacktop = m_userstack;         }
  void	shiftSymbol(unsigned int symbol)  { m_stacktop++;				      } // push 1 element (garbage) on userstack
  void	popSymbols(unsigned int count)    { m_stacktop -= count;	          } // pop count symbols from userstack
  void	shiftDollarDollar()               { *(++m_stacktop) = m_dollardollar; } // push($$) on userstack
  void	defaultReduce(unsigned int prod)  { m_dollardollar  = *m_stacktop;    } // $$ = $1
};

%}

%term	STRING					/* String constant									*/
%term	ICON					/* Integer or long constant including '\t', etc.	*/
%term	FCON					/* Floating-point constant.							*/
%term	TYPE					/* int char long float double signed unsigned short */
								/* const volatile void								*/
%term	STRUCT					/* struct union	class	     						*/
%term	ENUM					/* enum				     							*/
%term   DEFINE ENDIF IFDEF IFNDEF INCLUDE UNDEF
%term	EXTERN STATIC VIRTUAL REGISTER AUTO OPERATOR SIGNED
%term   PRIVATE PUBLIC PROTECTED
%term   TEMPLATE
%term	RETURN GOTO
%term	IF
%term	SWITCH CASE DEFAULT
%term	BREAK CONTINUE
%term	WHILE DO FOR
%term   TRYSTMT CATCH THROW
%term	LC RC					/* 	{ }	*/
%term	SEMI					/* 	 ;	*/
%term	ELLIPSIS				/*	...	*/

/* The attributes used below tend to be the sensible thing. For example, the
 * ASSIGNOP attribute is the operator component of the lexeme; most other
 * attributes are the first character of the lexeme. Exceptions are as follows:
 *	token	  attribute
 *	RELOP >	    '>'
 *	RELOP <     '<'
 *	RELOP >=    'G'
 *	RELOP <=    'L'
 */

%left	COMMA					/*	,			     					*/
%right	EQUAL ASSIGNOP 			/* =   *= /= %= += -= <<= >>= &= |= ^=	*/
%right	QUEST COLON				/* 	? :									*/
%left	OROR					/*	||									*/
%left	ANDAND					/*	&&									*/
%left	OR						/*	|									*/
%left   XOR						/*	^									*/
%left	AND						/*	&									*/
%left	EQUOP					/*	==  !=								*/
%left	RELOP					/*	<=  >= <  >							*/
%left	SHIFTOP					/*	>> <<								*/
%left	PLUS  MINUS				/* + -									*/
%left	STAR  DIVOP				/* *  /   %								*/
%right	SIZEOF UNOP INCOP		/*        sizeof     ! ~     ++ --      */
%left	LB RB LP RP STRUCTOP	/*	[ ] ( )  . ->						*/
%left   COLONCOLON
%right  NEWOP DELETEOP

								/* These attributes are shifted by the scanner.   */
%term TTYPE						/* Name of a type created with a previous typedef.*/
								/* Attribute is a pointer to the symbol table     */
								/* entry for that typedef.						  */
%nonassoc CLASS					/* extern register auto static typedef. Attribute */
								/* is the first character of the lexeme.		  */
%nonassoc NAME					/* Identifier or typedef name. Attribute is NULL  */
								/* if the symbol doesn't exist, a pointer to the  */
								/* associated "symbol" structure, otherwise.	  */

%nonassoc ELSE

%{

  /* This part goes to the first part of cppaction.cpp */

#include "stdafx.h"
#include <MyUtil.h>
#include "CppParser.h"

%}
%%

program		: ext_def_list
			;

ext_def_list: /* eps */
			| ext_def_list ext_def
			;

ext_def		: opt_specifiers ext_decl_list SEMI
			| opt_specifiers SEMI
			| opt_specifiers funct_decl compound_stmt
			;

opt_specifiers:
			  /* eps */							%prec COMMA
			| CLASS TTYPE 
			| TTYPE	      
			| specifiers
			;

specifiers	: type_or_class
			| specifiers type_or_class 
			;

type_or_class
			: type_specifier
			| CLASS 	   	
			;

type_specifier
			: TYPE		   	
			| enum_specifier
			| struct_specifier
			;

ext_decl_list
			: ext_decl
			| ext_decl_list COMMA ext_decl
			;

ext_decl	: var_decl
			| var_decl EQUAL initializer
			| funct_decl
			;

var_decl	: name									%prec COMMA  /* This production is done first. */
			| var_decl LP RP	    
			| var_decl LP var_list RP
			| var_decl LB RB
			| var_decl LB const_expr RB
			| STAR var_decl							%prec UNOP
			| LP var_decl RP 
			;

funct_decl	: STAR funct_decl	      
			| funct_decl LB RB	      
			| funct_decl LB const_expr RB 
			| LP funct_decl RP	      
			| funct_decl LP RP	      
			| name LP RP	      
			| name LP name_list RP
			| name LP var_list RP
			;

name_list	: name
			| name_list COMMA name 
			;

var_list	: param_declaration			
			| var_list COMMA param_declaration	
			;

param_declaration
			: type var_decl    	
			| abstract_decl		
			| ELLIPSIS		
			;

type		: type_specifier
			| type type_specifier	
			;

abstract_decl
			: type abs_decl   
			| TTYPE	abs_decl   
			;

abs_decl	: /* eps */	
			| LP abs_decl RP LP RP	
			| STAR abs_decl		    
			| abs_decl LB RB
			| abs_decl LB const_expr RB
			| LP abs_decl RP 	    
			;

struct_specifier
			: STRUCT opt_tag LC def_list RC
			| STRUCT tag		 
			;

opt_tag		: /* eps */	
			| tag
			;

tag			: NAME
			;


def_list	: /* eps */
			| def_list def		
			;			

def			: specifiers decl_list SEMI
			| specifiers SEMI 	
			;

decl_list	: decl 			
			| decl_list COMMA decl
			;

decl		: funct_decl
			| var_decl
			| var_decl EQUAL initializer 
			| var_decl COLON const_expr   			%prec COMMA
			| COLON const_expr		    			%prec COMMA
			;

enum_specifier
			: enum name opt_enum_list    
			| enum LC enumerator_list RC
			;

opt_enum_list:
			  /* eps */
			| LC enumerator_list RC
			;


enum		: ENUM   
			;

enumerator_list
			: enumerator
			| enumerator_list COMMA enumerator
			;

enumerator	: name			
			| name EQUAL const_expr 
			;


compound_stmt: LC stmt_list RC
			;

stmt_list	: /* eps */
			| stmt_list statement
			| stmt_list def
			;

/*----------------------------------------------------------------------
 * Statements
 */
statement	: SEMI
			| compound_stmt
			| expr SEMI  		
			| RETURN SEMI 	
			| RETURN expr SEMI
			| GOTO target SEMI	
			| target COLON	statement
			| IF LP test RP statement	
			| IF LP test RP statement ELSE  statement			
			| WHILE LP test RP	statement		
			| DO statement WHILE LP test RP SEMI	
			| FOR LP opt_expr SEMI test SEMI opt_expr RP statement
			| BREAK SEMI		
			| CONTINUE SEMI		
			| SWITCH LP expr RP compound_stmt
			| CASE const_expr COLON
			| DEFAULT COLON
			| THROW expr SEMI
			| TRYSTMT compound_stmt handler_list
			;

handler_list: exception_handler
			| handler_list exception_handler
			;

exception_handler:
			  CATCH LP catch_type RP compound_stmt
			;

catch_type	: ELLIPSIS
			| type var_decl
			| type
			;

target		: NAME	
			;

test		: /* eps */	    
			| expr      
			;

unary		: LP expr RP
			| FCON	  	
			| ICON	  	
			| NAME 		
			| string_const 							%prec COMMA
			| SIZEOF LP string_const RP				%prec SIZEOF
			| SIZEOF LP expr RP	     				%prec SIZEOF
			| SIZEOF LP abstract_decl RP			%prec SIZEOF
			| LP abstract_decl RP unary    		    %prec UNOP
			| MINUS	unary							%prec UNOP
			| UNOP unary		
			| unary INCOP		
			| INCOP	unary		
			| AND unary 							%prec UNOP
			| STAR unary							%prec UNOP
			| unary LB expr RB						%prec UNOP
			| unary STRUCTOP NAME					%prec STRUCTOP
			| unary LP args RP  	
			| unary LP RP  	
			;

args		: non_comma_expr						%prec COMMA   
			| non_comma_expr COMMA args
			;

expr		: expr COMMA non_comma_expr  
			| non_comma_expr
			;


non_comma_expr
			: non_comma_expr QUEST non_comma_expr COLON non_comma_expr	
			| non_comma_expr ASSIGNOP non_comma_expr 
			| non_comma_expr EQUAL non_comma_expr 
			| or_expr
			;

or_expr		: or_list         
			;

or_list		: or_list OROR  and_expr        
			| and_expr        
			;

and_expr	: and_list        
			;

and_list	: and_list ANDAND binary          
			| binary          
			;

binary		: binary RELOP	  binary	
			| binary EQUOP	  binary	
			| binary STAR 	  binary	
			| binary DIVOP 	  binary	
			| binary SHIFTOP  binary	
			| binary AND	  binary	
			| binary XOR	  binary	
			| binary OR		  binary	
			| binary PLUS 	  binary	
			| binary MINUS 	  binary	
			| unary
			;

opt_expr	: /* eps */
			| expr 		  
			;

const_expr	: expr					%prec COMMA
			;

initializer : expr					%prec COMMA
		    | LC initializer_list RC	
		    ;


initializer_list
 		    : initializer
		    | initializer_list COMMA initializer
		    ;

string_const: STRING
			| string_const STRING
			;
				
name		: NAME	
			;

%%

void CppParser::verror(const SourcePosition &pos, const char *format, va_list argptr) {
  char tmp[256],tmp2[256];
  vsprintf(tmp,format,argptr);
  sprintf(tmp2,"Error in line %d: %s", pos.getLineNumber(), tmp );
  strReplace(tmp2,'\n',' ');
  appendError("%s",tmp2);
}

void CppParser::appendError(const char *format,...) {
  va_list argptr;
  va_start(argptr,format);
  vprintf(format,argptr);
  va_end(argptr);
}
