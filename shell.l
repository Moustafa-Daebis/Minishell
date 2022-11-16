
/*
 *
 * CS-413 Spring 98
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include "y.tab.h"

%}

%%
"exit" { 
	return EXIT;
	}
\n 	{
		return NEWLINE;
	}

[ \t] 	{
		/* Discard spaces and tabs */
     	}	

">" 	{
		return GREAT;
	}
"|"     {      return BAR;
        }
">>"    {     return GREATER;
        }
"<"     {      return LESS;
        }
"&"      {return BACKGROUND;}
"&>>"   {return ERROR;}

[^\>\&\<\| \t\n][^\>\&\<\| \t\n]*  {
		/* Assume that file names have only alpha chars */
		yylval.string_val = strdup(yytext);
		return WORD;
	}

.	{
		/* Invalid character in input */
		return NOTOKEN;
	}

%%
	
