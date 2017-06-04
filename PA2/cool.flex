/*
 *  The scanner definition for COOL.
 */

/*
 *  Stuff enclosed in %{ %} in the first section is copied verbatim to the
 *  output, so headers and global definitions are placed here to be visible
 * to the code in the file.  Don't remove anything that was here initially
 */

%{
#include <cool-parse.h>
#include <stringtab.h>
#include <utilities.h>

/* The compiler assumes these identifiers. */
#define yylval cool_yylval
#define yylex  cool_yylex

/* Max size of string constants */
#define MAX_STR_CONST 1025
#define YY_NO_UNPUT   /* keep g++ happy */

extern FILE *fin; /* we read from this file */

/* define YY_INPUT so we read from the FILE fin:
 * This change makes it possible to use this scanner in
 * the Cool compiler.
 */
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) \
  if ( (result = fread( (char*)buf, sizeof(char), max_size, fin)) < 0) \
    YY_FATAL_ERROR( "read() in flex scanner failed");

char string_buf[MAX_STR_CONST]; /* to assemble string constants */
unsigned str_const_len = 0;

extern int curr_lineno;
extern int verbose_flag;

extern YYSTYPE cool_yylval;

/*
 *  Add Your own definitions here
 */
/* handle locations */
int yycolumn = 1;

unsigned int comment_level = 0;
bool string_parsing_error = false;

int accumulate_string_const(const char* added_str_, unsigned int len_);
%}

/*
 * Define names for regular expressions here.
 */
DIGIT           [0-9]
SIGN            [+-]

SINGLECHOPS     [+-/*~=*.(){}<:;@]
ASSIGN          <-
DARROW          =>
LE              <=
CLASS           [cC][lL][aA][sS][sS]
ELSE            [eE][lL][sS][eE]
FALSE           f[aA][lL][sS][eE]
IF              [iI][fF]
FI              [fF][iI]
IN              [iI][nN]
INHERITS        [iI][nN][hH][eE][rR][iI][tT][sS]
ISVOID          [iI][sS][vV][oO][iI][dD]
LET             [lL][eE][tT]
LOOP            [lL][oO][oO][pP]
POOL            [pP][oO][oO][lL]
THEN            [tT][hH][eE][nN]
WHILE           [wW][hH][iI][lL][eE]
CASE            [cC][aA][sS][eE]
ESAC            [eE][sS][aA][cC]
NEW             [nN][eE][wW]
OF              [oO][fF]
NOT             [nN][oO][tT]
TRUE            t[rR][uU][eE]
WHITESPACE      [ \f\r\t\v]+
NEW_LINE        \n
INT_CONST       ({DIGIT})+
OBJECTID        [a-z][A-Za-z0-9_]*
TYPEID          [A-Z][A-Za-z0-9_]*

BEGIN_LINE_COMMENT --
BEGIN_COMMENT   \(\*
END_COMMENT     \*\)
/*
 * Declare start states
 */
%X COMMENT
%X LINE_COMMENT
%X STRING_CONST


%%

{WHITESPACE} ;

{SINGLECHOPS} {return yytext[0];}

 /*
  *  Nested comment
  */
{BEGIN_LINE_COMMENT} {
  BEGIN LINE_COMMENT;
}

<LINE_COMMENT>\n {
  BEGIN INITIAL;
  ++curr_lineno;
}
<LINE_COMMENT><<EOF>>         {
  BEGIN INITIAL;
}
<LINE_COMMENT>. {
;
}

{BEGIN_COMMENT} {
    ++comment_level; 
    BEGIN COMMENT;
}

{END_COMMENT} {
    cool_yylval.error_msg = "Unmatched *)";
    return (ERROR);
}
<COMMENT>{BEGIN_COMMENT} {
  ++comment_level; 
}

<COMMENT>{END_COMMENT} {

  if(comment_level > 0) {
    --comment_level;
    if(comment_level == 0) {
      BEGIN INITIAL;
    }
    yyless(yyleng);
  } else {
    BEGIN INITIAL;
    cool_yylval.error_msg = "Unmatched *)";
    return (ERROR);
  }
}

<COMMENT>\n {
  ++curr_lineno;
}

<COMMENT><<EOF>> {
  cool_yylval.error_msg = "EOF in comment";
  str_const_len = 0;
  BEGIN INITIAL;
  return (ERROR);
}

<COMMENT>. {
;
}

 /*
  * Keywords are case-insensitive except for the values true and false,
  * which must begin with a lower-case letter.
  */

{CLASS}       { return (CLASS); }
{ELSE}        { return (ELSE); }
{IF}          { return (IF); }
{FI}          { return (FI); }
{IN}          { return (IN); }
{INHERITS}    { return (INHERITS); }
{ISVOID}      { return (ISVOID); }
{LET}         { return (LET);}
{LOOP}        { return (LOOP); }
{POOL}        { return (POOL); }
{THEN}        { return (THEN); }
{WHILE}       { return (WHILE); }
{CASE}        { return (CASE); }
{ESAC}        { return (ESAC); }
{NEW}         { return (NEW); }
{OF}          { return (OF); }
{NOT}         { return (NOT); }
{TRUE}        {
  cool_yylval.boolean = 1;
  return (BOOL_CONST);
}

{FALSE}       {
  cool_yylval.boolean = 0;
  return (BOOL_CONST);
}
 /*
  *  The multiple-character operators.
  */
{DARROW} {
  return (DARROW);
}
{ASSIGN} {
  return (ASSIGN);
}
{LE} {
  return (LE);
}

{INT_CONST} {
  cool_yylval.symbol = inttable.add_string(yytext);
  return INT_CONST;
     }

{OBJECTID} {
  cool_yylval.symbol = idtable.add_string(yytext);
  return OBJECTID;
     }

{TYPEID} {
  cool_yylval.symbol = idtable.add_string(yytext); //not sure is enough
  return TYPEID;
     }

 /*
  *  String constants (C syntax)
  *  Escape sequence \c is accepted for all characters c. Except for
  *  \n \t \b \f, the result is c.
  *
  */

\" {

  //printf("\nBEGIN Comment");
  string_parsing_error = false;
  str_const_len = 0;
  string_buf[str_const_len] = '\0';
  BEGIN STRING_CONST;
}

<STRING_CONST>{
  \" {
    //printf("\nEND Comment");
    if(string_parsing_error == false) {
      string_buf[str_const_len] = '\0';
      cool_yylval.symbol = stringtable.add_string(string_buf, str_const_len);
      str_const_len = 0;
      BEGIN INITIAL;
      return (STR_CONST);
    }else {
      string_parsing_error = false;
      str_const_len = 0;
      BEGIN INITIAL;
    }
  }
  \n {
  ++curr_lineno;
  if(string_parsing_error == false) {
    cool_yylval.error_msg = "Unterminated string constant";
    str_const_len = 0;
    BEGIN INITIAL;
    return (ERROR);
  } else {
    str_const_len = 0;
    BEGIN INITIAL;
    string_parsing_error = false; //resume string parsing
  }
}
  \\\n {
    ++curr_lineno;
    int err = accumulate_string_const(yytext+1, 1);
    if(err == ERROR) {
      return err;
    }
  }
  \\n {
    int err = accumulate_string_const("\n", 1);
    if(err == ERROR) {
      return err;
    }
  }
  \\t {
    int err = accumulate_string_const("\t", 1);
    if(err == ERROR) {
      return err;
    }
  }
  \\b {
    int err = accumulate_string_const("\b", 1);
    if(err == ERROR) {
      return err;
    }
  }
  \\f {
      int err = accumulate_string_const("\f", 1);
    if(err == ERROR) {
      return err;
    }
  }
  \\. {
    int err = accumulate_string_const(&yytext[1], 1);
    if(err == ERROR) {
      return err;
    }
  }
  [^\\\n\"]+ {
    int err = accumulate_string_const(yytext, yyleng);
    if(err == ERROR) {
      return err;
    }
  }
  <<EOF>> {
    cool_yylval.error_msg = "EOF in string constant";
    string_parsing_error = true;
    str_const_len = 0;
    BEGIN INITIAL;
    return (ERROR);
  }
}

{NEW_LINE} {
  ++curr_lineno;
}

. {
    cool_yylval.error_msg = yytext;
    return (ERROR);
}

%%
int accumulate_string_const(const char* added_str_, unsigned int len_) 
{

  if(str_const_len + len_ < MAX_STR_CONST) {

    for(unsigned int idx = 0; idx < len_; ++idx) {
      if(added_str_[idx] != '\0') {
        string_buf[str_const_len + idx] = added_str_[idx];
      } else {
        str_const_len = 0;
        cool_yylval.error_msg = "String constant may not contain null character";
        string_parsing_error = true;
        return (ERROR);
      }

    }
    str_const_len += len_;
    return 0;
  } else {
    str_const_len = MAX_STR_CONST;
    cool_yylval.error_msg = "String constant too long";
    string_parsing_error = true;
    return (ERROR);
  }

}
