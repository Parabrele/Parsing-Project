//   This file, along with the langlex.c and lang.y, are extensions of the TP's files.
%option noyywrap
%option yylineno

%%

"var"	{ return VAR; }
"proc"	{ return PROC; }
"end"	{ return END; }
"do"	{ return DO; }
"od"	{ return OD; }
"if"	{ return IF; }
"fi"	{ return FI; }
"else"	{ return ELSE; }
"break"	{ return BREAK; }
"reach"	{ return REACH; }
"skip"	{ return SKIP; }

":="	{ return ASSIGN; }
"||"	{ return OR; }
"&&"	{ return AND; }
"=="	{ return EQ; }
"!"	{ return NOT; }
"::"	{ return COLONS; }
"->"	{ return ARROW; }

\/\/.*\n	{ }

[0-9]+	{ yylval.i = atol(yytext); return INT; }
[a-z_][a-z0-9_]* { yylval.c = strdup(yytext); return IDENT; }

[	 \n]   { /* ignore */ }

.	{ return *yytext; }

%%

