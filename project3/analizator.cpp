#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <ctype.h>
#include <cstdlib>
#include <stack>
#include <algorithm>

using namespace std;

enum type_of_lex 
{
	LEX_NULL,  //0
	LEX_BOOLEAN, LEX_DO, LEX_ELSE, LEX_IF, LEX_FALSE, LEX_NUMBER, LEX_STRING, LEX_THEN, LEX_TRUE, LEX_VAR, LEX_WHILE, LEX_BREAK, LEX_CASE, LEX_CONTINUE, LEX_GOTO, LEX_FOR, LEX_FUNCTION, LEX_IN, LEX_INSTANCE, LEX_NAN, LEX_NILL, LEX_RETURN, LEX_TYPEOF, LEX_VOID, LEX_WITH, LEX_OBJECT, LEX_UNDEFINED,
	LEX_FIN, //28
	LEX_AMP, LEX_DAMP, LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS, LEX_GRT, LEX_PLUS, LEX_PLEQ, /*41 */ LEX_MINUS, LES_MINEQ, LEX_PIPE, LEX_DPIPE, LEX_REM, LEX_REMEQ, LEX_SLASH, LEX_SLEQ, LEX_TIMES, LEX_TIMESEQ, LEX_SLASHTIMES, LEX_TIMESLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ, LEX_INC, LEX_DEC, LEX_POINT, LEX_LBRACK, LEX_RBRACK, LEX_STEQ, LEX_STNEQ, LEX_BEGIN, LEX_END, LEX_NOT, LEX_DSLASH,
	LEX_NUM, //68
	LEX_CONSTR, //69
  	LEX_ID, 
  	POLIZ_LABEL,
  	POLIZ_ADDRESS, 
  	POLIZ_GO, 
	POLIZ_FGO  
}; 


class Lex {
	type_of_lex t_lex;
	int v_lex;
	string s_lex;
public:
	Lex ( type_of_lex t = LEX_UNDEFINED, int v = 0, string s = " " ) 
	{
		t_lex = t;
		v_lex = v;
		s_lex = s;
	}

	type_of_lex get_type () const 
	{
		return t_lex;
	}

	int get_value () const 
	{
		return v_lex;
	}
	
	string get_string () const 
	{
		return s_lex;
	}

	friend ostream & operator << ( ostream & s, Lex l ); 
};

class Ident {
	string name;
	bool declare;
	type_of_lex type;
	bool assign;
	int value;
public:
	Ident () 
	{ 
		declare = false; 
		assign = false; 
	}
	
	Ident ( const string n ) 
	{
		name = n; 
		declare = false; 
		assign = false; 
	}
	
	bool operator== (const string& s) const 
	{
		return name == s; 
	}
	
	string get_name () 
	{ 
		return name; 
	} 
	
	bool get_declare () 
	{ 
		return declare; 
	}
	
	void put_declare () 
	{ 
		declare = true; 
	}
	
	type_of_lex get_type () 
	{ 
		return type; 
	}
	
	void put_type ( type_of_lex t ) 
	{ 
		type = t; 
	}
	
	bool get_assign () 
	{ 
		return assign; 
	}
	
	void put_assign () 
	{ 
		assign = true; 
	}
	
	int get_value () 
	{ 
		return value; 
	}
	
	void put_value ( int v ) 
	{ 
		value = v; 
	}
};

vector <Ident> TID;

int put (const string & buf) {
	vector <Ident>::iterator k;
	if ( (k = find (TID.begin(), TID.end(), buf)) != TID.end() ) 
		return k - TID.begin();
	TID.push_back ( Ident (buf) );
	return TID.size() - 1;
}

class Scanner {
	FILE *fp;
	int c;
	int look (const string& buf, const char **list) 
	{
		int i = 0;
		while (list [i]) 
		{
			if ( buf == list [i] )
				return i;
			i++; 
		}
		return 0; 
	}
	void gc () 
	{ 
		c = fgetc (fp); 
	}
public:
	static const char *TW [ ], *TD [ ];
	Scanner (const char *program) 
	{
		fp = fopen ( program, "r" );
	}
	Lex get_lex ();
};

const char * 
Scanner::TW [] = { "", "Boolean", "do", "else", "if", "false", "Number", "String", "then", "true", "var", "while", "break", "case", "continue", "goto", "for",	"function",	"in", "instanceof", "NaN", "null",	"return", "typeof", "void", "with", "Object", "undefined", NULL};

const  char * 
Scanner::TD [] = { "@", "&", "&&", ";", ",", ":", "=", "(", ")", "==", "<", ">", "+", "+=", "-", "-=", "|", "||", "%", "%=", "/", "/=", "*", "*=", "/*", "*/", "<=", "!=", ">=", "++", "--", ".", "[", "]", "===", "!==", "{", "}", "!", "//", NULL};


Lex Scanner::get_lex () 
{ 
	enum state { H, IDENT, NUMB, SLASH, ALE, ALE2, PLUS, MINUS, AMP, PIPE, QUOTE, COM, COM2, COM3, NEQ };
	state CS = H; 
	string buf; 
	int d, j;
	do { 
		gc (); 
		switch(CS) {
			case H: 
				if ( c == ' ' || c == '\n' || c == '\r' || c == '\t' );
				else if (isalpha(c)) 
				{ 
					buf.push_back(c); 
					CS = IDENT; 
				}
				else if (isdigit (c)) 
				{ 
					d = c - '0'; 
					CS = NUMB; 
				}
				else if ( c == '/' ) 
				{ 
					CS = SLASH; 
				}
				else if ( c == '!' || c == '=') 
				{
					buf.push_back(c); 
					CS = ALE; 
				}
				else if ( c == '*' || c == '<' || c == '>' || c == '=' ) 
				{
					buf.push_back(c); 
					CS = ALE2;
				}
				else if ( c == '+' )
				{
				    buf.push_back(c);
				    CS = PLUS;
				}
				else if ( c == '-' )
				{
				    buf.push_back(c);
				    CS = MINUS;
				}
				else if ( c == '&' )
				{
				    buf.push_back(c);
				    CS = AMP;
				}
				else if ( c == '|' )
				{
				    buf.push_back(c);
				    CS = PIPE;
				}
				else if ( c == '"')
				{
				    CS = QUOTE;
				}
				else if ( c == EOF ) 
					return Lex (LEX_FIN);
				else 
				{ 
					buf.push_back(c);
					if ((j = look ( buf, TD)))
						return Lex ( (type_of_lex) (j + (int) LEX_FIN), j );
					else throw c;
				}
				break;
			case IDENT:
				if (isalpha(c) || isdigit(c)) 
				{
					buf.push_back(c); 
				}
				else 
				{ 
					ungetc(c, fp);
					if (j = look (buf, TW))
						return Lex ((type_of_lex)j , j);
					else 
					{
						j = put (buf);
						return Lex (LEX_ID, j);
					}
				};
				break;
			case NUMB:
				if (isdigit (c)) 
				{
					d = d * 10 + (c - '0'); 
				}
				else 
				{ 
					ungetc(c, fp);
					return Lex(LEX_NUM, d); 
				}
				break;
			case SLASH:
			    if (c == '*')
			    {
			        buf.pop_back();
			        CS = COM;
			    }
			    else if (c == '=')
			    {
			        buf.push_back(c);
			        j = look ( buf, TD );
		            return Lex( (type_of_lex) ( j + (int)LEX_FIN ), j);
			    }
			    else if (c == '/')
			    {
			        buf.pop_back();
			        CS = COM3;
			    }
			    else 
			    {
			        ungetc ( c, fp );
		            j = look ( buf, TD );
		            return Lex( (type_of_lex) ( j + (int)LEX_FIN ), j);
			    }
			case COM:
				if (c == '*')
				{
				    CS = COM2;
				}
				else if ( c == EOF ) throw c;
				break;
			case COM2:
			    if (c == '/')
			    {
			        CS = H;
			    }
			    else if ( c == EOF )
			    {
			        throw c;
			    }
			    else CS = COM;
			    break;
			case COM3:
			    if (c == '\n')
			    {
			        CS = H;
			    }
			    else if ( c == EOF )
			    {
			        throw c;
			    }
			    break;
			case ALE:
				if ( c == '=' ) 
				{
					buf.push_back(c);
					CS = ALE2;
				}
				else 
				{
					ungetc(c, fp);
					j = look (buf, TD);
					return Lex((type_of_lex) ( j + (int) LEX_FIN), j );
				}
				break;
			case ALE2:
			    if ( c == '=' ) 
				{
					buf.push_back(c);
					j = look (buf, TD);
					return Lex((type_of_lex) ( j + (int) LEX_FIN), j );
				}
				else 
				{
					ungetc(c, fp);
					j = look (buf, TD);
					return Lex((type_of_lex) ( j + (int) LEX_FIN), j );
				}
				break; 
			case PLUS:
			    if ( c == '+' || c == '=' ) 
			    {
			        buf.push_back(c);
					j = look (buf, TD);
					return Lex((type_of_lex) ( j + (int) LEX_FIN), j );
				}
				else 
				{
					ungetc(c, fp);
					j = look (buf, TD);
					return Lex((type_of_lex) ( j + (int) LEX_FIN), j );
				}
				break; 
			case MINUS:
			    if ( c == '-' || c == '=' ) 
			    {
			        buf.push_back(c);
					j = look (buf, TD);
					return Lex((type_of_lex) ( j + (int) LEX_FIN), j );
				}
				else 
				{
					ungetc(c, fp);
					j = look (buf, TD);
					return Lex((type_of_lex) ( j + (int) LEX_FIN), j );
				}
				break; 
			case AMP:
			    if ( c == '&' ) 
			    {
			        buf.push_back(c);
					j = look (buf, TD);
					return Lex((type_of_lex) ( j + (int) LEX_FIN), j );
				}
				else 
				{
					ungetc(c, fp);
					j = look (buf, TD);
					return Lex((type_of_lex) ( j + (int) LEX_FIN), j );
				}
				break; 
			case PIPE:
			    if ( c == '|' ) 
			    {
			        buf.push_back(c);
					j = look (buf, TD);
					return Lex((type_of_lex) ( j + (int) LEX_FIN), j );
				}
				else 
				{
					ungetc(c, fp);
					j = look (buf, TD);
					return Lex((type_of_lex) ( j + (int) LEX_FIN), j );
				}
				break; 
			case QUOTE:
	            if (c=='"')
		        {
		          string st = "";
		          for (int i = 0; i < buf.size(); i++)
		          {
	                st += buf[i];
		          }
		          return Lex(LEX_CONSTR, 0, st);
		        }
		        buf.push_back(c);
		        break;
		} 
	} while (true);
};

ostream & operator<< ( ostream &s, Lex l ) {
    string t;
    if ( l.t_lex <= 27 )
        t = Scanner::TW[l.t_lex];
    else if (l.t_lex >= 28 && l.t_lex <= 67)
    t = Scanner::TD[l.t_lex-28];
    else if (l.t_lex == 68)
    t = "NUMB";
    else if (l.t_lex == 69)
    {	  
        t = "STRING CONST";
        s << '(' << t << ',' << l.s_lex << ");" << endl;
        return s;
    }
    else if (l.t_lex == 70)
        t = TID[l.v_lex].get_name();
    else if (l.t_lex == 71)
        t = "Label";
    else if(l.t_lex == 72)
        t = "Addr";
    else if (l.t_lex == 73)
        t = "!";
    else if (l.t_lex == 74) 
        t = "!F";
    else
        throw l;
    s << '(' << t << ',' << l.v_lex << ");" << endl;
    return s;
}

int main () {
    Scanner scan("program.txt");
    Lex l;
    try
    {
        while(1)
        {
	        l = scan.get_lex();
	        if (l.get_type() == LEX_FIN)
	        {
		        break;
	        }
            cout << l;
        }
    }
    catch (Lex l)
    {
        cout << "operator << error" << endl;
    }
    catch (char c)
    {
        cout << "get_lex() error" << endl;
    }
}



