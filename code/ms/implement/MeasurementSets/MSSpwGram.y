/*
    MSSPWGram.y: Parser for spw expressions
    Copyright (C) 2004
    Associated Universities, Inc. Washington DC, USA.

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; if not, write to the Free Software Foundation,
    Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.

    Correspondence concerning AIPS++ should be addressed as follows:
           Internet email: aips2-request@nrao.edu.
           Postal address: AIPS++ Project Office
                           National Radio Astronomy Observatory
                           520 Edgemont Road
                           Charlottesville, VA 22903-2475 USA

    $Id$
*/

%{
using namespace casa;
%}

%pure_parser                /* make parser re-entrant */

%union {
  const TableExprNode* node;
  Block<TableExprNode>* exprb;
  TableExprNodeSetElem* elem;
  TableExprNodeSet* settp;
  Int ival;
  char * str; 
  Double dval;
}

%token FREQUENCYUNIT
%token VELOCITYUNIT
%token <str> SPWNAME
%token <ival> INDEX
%token <dval> NUMBER
%token DASH
%token EQASS
%token SQUOTE

%token LBRACKET
%token LPAREN
%token RBRACKET
%token RPAREN
%token LBRACE
%token RBRACE
%token COLON

%type <node> spwstatement
%type <node> nameexpr
%type <node> combindexexpr
%type <node> pureindexexpr
%type <node> indexexpr

%left OR
%nonassoc EQ EQASS GT GE LT LE NE COMMA SLASH
%left PLUS MINUS
%left TIMES DIVIDE MODULO
%nonassoc UNARY
%nonassoc NOT
%right POWER

%{
int MSSpwGramlex (YYSTYPE*);
%}

%%
spwstatement: SQUOTE nameexpr SQUOTE {
                $$ = $2;}
            | combindexexpr
            ;

combindexexpr: pureindexexpr {
                 $$ = $1;}
             | indexexpr COMMA indexexpr {
                 $$ = new TableExprNode ($1 || $3);}
             ;
pureindexexpr: INDEX {
                 Vector<Int> spwids(1);
                 spwids[0] = $1;
                 $$ = MSSpwParse().selectSpwIds(spwids);
                 }
             | pureindexexpr COMMA INDEX {
                 Vector<Int> spwids(1);
                 spwids[0] = $3;
	         $$ = MSSpwParse().selectSpwIds(spwids);
                 }
             ;
indexexpr: INDEX COLON INDEX {
	     $$ = MSSpwParse().selectChaninASpw($1, $3);
	     }
         | INDEX COLON INDEX DASH INDEX {
             $$ = MSSpwParse().selectChanRangeinASpw($1, $3, $5);
	     }
         | INDEX COLON NUMBER DASH NUMBER VELOCITYUNIT {
             $$ = MSSpwParse().selectVelRangeinASpw($1, $3, $5);
	     }
         | INDEX COLON NUMBER DASH NUMBER FREQUENCYUNIT {
             $$ = MSSpwParse().selectFreRangeinASpw($1, $3, $5);
	     }
         ;
nameexpr:  SPWNAME {
             String name($1);
             $$ = MSSpwParse().selectSpwName(name);
           }
        |  nameexpr COMMA SPWNAME {
             $$ = new TableExprNode ($1 || $3);}
        ;

%%

