/*   This file, along with the lang.y, are extensions of the TP's files found at https://lmf.cnrs.fr/IsaVialard/LangagesFormels. */
%option noyywrap
%option yylineno

%%

"var"	{ return VAR; }

"proc"	{ return PROC; }
"end"	{ return END; }

"if"	{ return IF; }
"fi"	{ return FI; }
"do"	{ return DO; }
"od"	{ return OD; }
"::"	{ return COND; }
"->"	{ return THEN; }

"skip"	{ return SKIP; }
"else"	{ return ELSE; }
"break"	{ return BREAK; }

"reach"	{ return REACH; }

":="	{ return ASSIGN; }
"||"	{ return OR; }
"&&"	{ return AND; }
"=="	{ return EQ; }
"!"	    { return NOT; }

\/\/.*\n	{ }

[0-9]+	{ yylval.i = atol(yytext); return INT; }
[a-z_][a-z0-9_]* { yylval.c = strdup(yytext); return IDENT; }

[	 \n]   { /* ignore */ }

.	{ return *yytext; }

%%

