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
    LEX_BOOLEAN, LEX_DO, LEX_ELSE, LEX_IF, LEX_FALSE, LEX_NUMBER, LEX_STRING, LEX_TRUE, LEX_VAR, LEX_WHILE, LEX_BREAK, LEX_CASE, LEX_CONTINUE, LEX_GOTO, LEX_FOR, LEX_FUNCTION, LEX_IN, LEX_INSTANCE, LEX_NAN, LEX_NILL, LEX_RETURN, LEX_TYPEOF, LEX_VOID, LEX_WITH, LEX_OBJECT, LEX_UNDEFINED, POLIZ_WRITE, POLIZ_GETENV,
    LEX_FIN, //29
    LEX_AMP, LEX_DAMP, LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS, LEX_GRT, LEX_PLUS, LEX_PLEQ, /*42 */ LEX_MINUS, LEX_MINEQ, LEX_PIPE, LEX_DPIPE, LEX_REM, LEX_REMEQ, LEX_SLASH, LEX_SLEQ, LEX_TIMES, LEX_TIMESEQ, LEX_SLASHTIMES, LEX_TIMESLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ, LEX_INC, LEX_DEC, LEX_POINT, LEX_LBRACK, LEX_RBRACK, LEX_STEQ, LEX_STNEQ, LEX_BEGIN, LEX_END, LEX_NOT, LEX_DSLASH,
    LEX_CONSTR, //69
    LEX_ID, 
    POLIZ_LABEL,
    POLIZ_ADDRESS, 
    POLIZ_GO, 
    POLIZ_FGO,
    POLIZ_LINC,
    POLIZ_RINC,
    POLIZ_LDEC,
    POLIZ_RDEC,
}; 

/////////////////////////  Класс Lex  //////////////////////////

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
    
    void put_type (type_of_lex t) 
    {
        t_lex = t;
    }

    int get_value () const 
    {
        return v_lex;
    }
    
    void put_value (int v) 
    {
        v_lex = v;
    }
    
    string get_string () const 
    {
        return s_lex;
    }
    
    void put_string (string s) 
    {
        s_lex = s;
    }

    friend ostream & operator << ( ostream & s, Lex l ); 
};


/////////////////////  Класс Ident  ////////////////////////////


class Ident {
    string name;
    string str;
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
    
    string get_string () 
    { 
        return str; 
    } 
    
    void put_string (string stri) 
    { 
        str = stri; 
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

//////////////////////  TID  ///////////////////////

vector <Ident> TID;

int put (const string & buf) {
    vector <Ident>::iterator k;
    if ( (k = find (TID.begin(), TID.end(), buf)) != TID.end() ) 
        return k - TID.begin();
    TID.push_back ( Ident (buf) );
    return TID.size() - 1;
}

/////////////////////////////////////////////////////////////////

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
Scanner::TW [] = { "", "Boolean", "do", "else", "if", "false", "Number", "String", "true", "var", "while", "break", "case", "continue", "goto", "for",    "function",    "in", "instanceof", "NaN", "null",    "return", "typeof", "void", "with", "Object", "undefined", "write", "getenv", NULL};

const  char * 
Scanner::TD [] = { "@", "&", "&&", ";", ",", ":", "=", "(", ")", "==", "<", ">", "+", "+=", "-", "-=", "|", "||", "%", "%=", "/", "/=", "*", "*=", "/*", "*/", "<=", "!=", ">=", "++", "--", ".", "[", "]", "===", "!==", "{", "}", "!", "//", NULL};


Lex Scanner::get_lex () 
{ 
    enum state { H, IDENT, NUMB, SLASH, ALE, ALE2, PLUS, MINUS, AMP, PIPE, QUOTE, COM, COM2, COM3 };
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
                    buf.push_back(c);
                    CS = SLASH; 
                }
                else if ( c == '#' ) 
                { 
                    CS = COM3; 
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
                    if ((j = look (buf, TW)))
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
                    return Lex(LEX_NUMBER, d); 
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
                  for (unsigned int i = 0; i < buf.size(); i++)
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
    if ( l.t_lex < 29 )
        t = Scanner::TW[l.t_lex];
    else if (l.t_lex >= 29 && l.t_lex <= 68)
    t = Scanner::TD[l.t_lex-29];
    else if (l.t_lex == 6)
    t = "NUMBER";
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
    else if (l.t_lex == 75)
        t = "+#";
    else if (l.t_lex == 76)
        t = "#+";
    else if (l.t_lex == 77)
        t = "-#";
    else if (l.t_lex == 78)
        t = "#-";
    else
        throw l;
    s << '(' << t << ',' << l.v_lex << ");" << endl;
    return s;
}

//////////////////////////  Класс Parser  /////////////////////////////////

template <class T, class T_EL>

void from_st(T & st, T_EL& i)
{
    i = st.top();
    st.pop();
}

class Parser
{
    Lex curr_lex;
    type_of_lex c_type;
    int c_val;
    Scanner scan;
    stack<int> st_int;
    stack<type_of_lex> st_lex;
    stack <int> labels_for_con;
    stack <int> labels_for_break;
    void S();
    void B();
    void D();
    void E();
    void E1();
    void T();
    void F();
    void WR();
    void FOR_PARAMETERS();
    void dec(type_of_lex type, int i);
    void check_id();
    void eq_type(type_of_lex& new_val);
    void gl()
    {
        curr_lex = scan.get_lex();
        c_type = curr_lex.get_type();
        c_val = curr_lex.get_value();
        cout << curr_lex;
    }

public:
    vector<Lex> poliz;
    Parser(const char* program): scan(program) {}
    void analyze();
};

void Parser::analyze()
{
    int num = 0;
    cout << "\nLex for self-checking:\n\n";
    gl();
    S();
    if (c_type != LEX_FIN)
        throw curr_lex;
    cout << "\n\nEverything is OK. Let's look at POLIZ:\n\n";
    for (Lex l : poliz)
    {
        cout << num <<' '<< l;
        num++;
    }
}

void Parser::S()
{
    int pl0, pl1, pl2, pl3;
    if (c_type == LEX_IF)
    {
        gl();
        if (c_type != LEX_LPAREN)
            throw curr_lex;
        else
        {
            gl();
            E();
            pl2 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_FGO));
            if (c_type == LEX_RPAREN)
            {
                gl();
                B();
                pl3 = poliz.size();
                poliz.push_back(Lex());
                poliz.push_back(Lex(POLIZ_GO));
                if (c_type == LEX_ELSE)
                {
                    poliz[pl2] = Lex(POLIZ_LABEL, poliz.size());
                    gl();
                    B();
                    poliz[pl3] = Lex(POLIZ_LABEL, poliz.size());
                }
                else
                {
                    poliz.pop_back();
                    poliz.pop_back();
                    poliz[pl2] = Lex(POLIZ_LABEL, poliz.size());
                }
            }
            else
                throw curr_lex;
        }
    } // end if
    else if (c_type == LEX_WHILE)
    {
        pl0 = poliz.size();
        labels_for_con.push(poliz.size());
        gl();
        if (c_type != LEX_LPAREN)
            throw curr_lex;
        else
        {
            gl();
            E();
            pl1 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_FGO));
            if (c_type == LEX_RPAREN)
            {
                gl();
                B();
                poliz.push_back(Lex(POLIZ_LABEL, pl0));
                poliz.push_back(Lex(POLIZ_GO));
                poliz[pl1] = Lex(POLIZ_LABEL, poliz.size());
            }
            else
                throw curr_lex;
        }
        if (!labels_for_break.empty())
        {
            poliz[labels_for_break.top()] = Lex(POLIZ_LABEL, poliz.size());
            labels_for_break.pop();
        }
    } // end while
    else if (c_type == LEX_DO) 
    {
        pl0 = poliz.size();
        gl();
        B();
        labels_for_con.push(poliz.size());
        if (c_type == LEX_WHILE) 
        {
            gl();
            if (c_type == LEX_LPAREN) 
            {
                gl();
                E();
                pl1 = poliz.size() + 4;
                poliz.push_back(Lex(POLIZ_LABEL, pl1)); 
                poliz.push_back(Lex(POLIZ_FGO));
                poliz.push_back(Lex(POLIZ_LABEL, pl0));
                poliz.push_back(Lex(POLIZ_GO));
                if (c_type != LEX_RPAREN)
                    throw curr_lex;
                gl();
                gl();
            }
            else
                throw curr_lex;
        }
        else
            throw curr_lex;
        if (!labels_for_break.empty())
        {
            poliz[labels_for_break.top()] = Lex(POLIZ_LABEL, poliz.size());
            labels_for_break.pop();
        }
    } //end do while
    else if (c_type == LEX_FOR) 
    {
        gl();
        if (c_type == LEX_LPAREN) 
        {
            gl();
            FOR_PARAMETERS();
            pl0 = poliz.size();
            if (c_type == LEX_SEMICOLON) {
                gl();
                E();
                poliz.push_back(Lex());
                poliz.push_back(Lex(POLIZ_FGO));
                poliz.push_back(Lex());
                poliz.push_back(Lex(POLIZ_GO));
                pl1 = poliz.size();
                labels_for_con.push(poliz.size());
                if (c_type == LEX_SEMICOLON) {
                    gl();
                    FOR_PARAMETERS();
                    poliz.push_back(Lex(POLIZ_LABEL, pl0));
                    poliz.push_back(Lex(POLIZ_GO));
                    poliz[pl1-2] = Lex(POLIZ_LABEL, poliz.size());
                    if (c_type == LEX_RPAREN) {
                        gl();
                        B();
                        poliz.push_back(Lex(POLIZ_LABEL, pl1));
                        poliz.push_back(Lex(POLIZ_GO));
                        poliz[pl1-4] = Lex(POLIZ_LABEL, poliz.size());
                    }
                    else
                        throw curr_lex;
                }
                else
                    throw curr_lex;
            }
            if (!labels_for_break.empty())
            {
                poliz[labels_for_break.top()] = Lex(POLIZ_LABEL, poliz.size());
                labels_for_break.pop();
            }
        }
        else
            throw curr_lex;
    } //end for
    else if (c_type == LEX_ID)
    {
        int l_v_index = curr_lex.get_value();
        type_of_lex new_val;
        check_id();
        poliz.push_back(Lex(POLIZ_ADDRESS, c_val));
        gl();
        if (c_type == LEX_ASSIGN || c_type == LEX_MINEQ || c_type == LEX_PLEQ || c_type == LEX_TIMESEQ || c_type == LEX_REMEQ || c_type == LEX_SLEQ)
        {
            type_of_lex tmp = c_type;
            gl();
            E();
            eq_type(new_val);
            TID[l_v_index].put_type(new_val);
            poliz.push_back(Lex(tmp));
            if (c_type == LEX_SEMICOLON)
            {
                gl();
            }
            else if (c_type == LEX_FIN)
                return;
            else
                throw curr_lex;
        }
        else if (c_type == LEX_INC)
        {
            poliz.push_back(Lex(POLIZ_RINC));
            gl();
            if (c_type != LEX_SEMICOLON && c_type != LEX_FIN)
                throw curr_lex;
            else if (c_type == LEX_SEMICOLON)
                gl();
        }
        else if (c_type == LEX_DEC)
        {
            poliz.push_back(Lex(POLIZ_RDEC));
            gl();
            if (c_type != LEX_SEMICOLON && c_type != LEX_FIN)
                throw curr_lex;
            else if (c_type == LEX_SEMICOLON)
                gl();
        }
        else
            throw curr_lex;
    } // assign-end
    else if (c_type == LEX_CONTINUE)
    {
        if (labels_for_con.empty())
        {
            throw curr_lex;
        }
        int lab = labels_for_con.top();
        labels_for_con.pop();
        poliz.push_back(Lex(POLIZ_LABEL, lab));
        poliz.push_back(Lex(POLIZ_GO));
        gl();
        if (c_type != LEX_SEMICOLON)
        {
            throw curr_lex;
        }
        gl();
    }
    else if (c_type == LEX_BREAK)
    {
        labels_for_break.push(poliz.size());
        poliz.push_back(Lex());
        poliz.push_back(Lex(POLIZ_GO));
        gl();
        if (c_type != LEX_SEMICOLON)
        {
            throw curr_lex;
        }
        gl();
    }
    else if (c_type == LEX_VAR)
    {
        gl();
        D();
    } // var-end
    else if (c_type == LEX_FIN)
    {
        return;
    }
    else if (c_type == POLIZ_WRITE || c_type == POLIZ_GETENV)
    {
        WR();
        gl();
    }
    else if (c_type == LEX_END)
    {
        return;
    }
    if (c_type == LEX_INC || c_type == LEX_DEC)
    {
        E();
        gl();
    }
    S();
}

void Parser::WR()
{
    type_of_lex new_type = c_type;
    gl();
    if (c_type != LEX_LPAREN)
    {
        throw curr_lex;
    }
    else
    {
        gl();
        if (c_type == LEX_ID)
        {
            check_id();
            poliz.push_back(Lex(POLIZ_ADDRESS, c_val));
            gl();
        }
        else
        {
            E();
        }
        if (c_type != LEX_RPAREN)
        {
            throw curr_lex;
        }
        else
        {
            gl();
            poliz.push_back(Lex(new_type));
        }
    }
}

void Parser::B()
{
    if (c_type == LEX_BEGIN)
    {
        gl();
        S();
        if (c_type == LEX_END)
        {
            gl();
        }
        else
        {
            throw curr_lex;
        }
    }
    else
        throw curr_lex;
}


void Parser::D()
{
    if (c_type != LEX_ID)
        throw curr_lex;
    else
    {
        st_int.push ( c_val );
        int l_v_index = c_val;
        gl();
        
        if (c_type == LEX_ASSIGN)
        {
            poliz.push_back(Lex(POLIZ_ADDRESS, l_v_index));
            type_of_lex i;
            type_of_lex tmp = c_type;
            gl();
            E();
            from_st(st_lex, i);
            dec(i, l_v_index);
            poliz.push_back(Lex(tmp));
        }
        else
        {
            dec(LEX_UNDEFINED, l_v_index);
        } 
        
        while (c_type == LEX_COMMA)
        {
            gl();
            if (c_type != LEX_ID)
                throw curr_lex;
            else
            {
                st_int.push ( c_val );
                int l_v_index = c_val;
                gl();
                
                if (c_type == LEX_ASSIGN)
                {
                    poliz.push_back(Lex(POLIZ_ADDRESS, l_v_index));
                    type_of_lex i;
                    type_of_lex tmp = c_type;
                    gl();
                    E();
                    from_st(st_lex, i);
                    dec(i, l_v_index);
                    poliz.push_back(Lex(tmp));
                }
                else
                {
                    dec(LEX_UNDEFINED, l_v_index);
                } 
            }
        }
        if (c_type != LEX_SEMICOLON && c_type != LEX_FIN)
            throw curr_lex;
        else if (c_type == LEX_SEMICOLON)
        {
            gl();
        }
    }
}


void Parser::E()
{
    if (c_type == LEX_INC)
    {
        gl();
        int l_v_index = c_val;
        if (c_type == LEX_ID)
        {
            check_id();
            poliz.push_back(Lex(POLIZ_ADDRESS, l_v_index));
        }
        else
        {
            throw curr_lex;
        }
        poliz.push_back(Lex(POLIZ_LINC));
        gl();
    }
    else if (c_type == LEX_DEC)
    {
        gl();
        int l_v_index = c_val;
        if (c_type == LEX_ID)
        {
            check_id();
            poliz.push_back(Lex(POLIZ_ADDRESS, l_v_index));
        }
        else
        {
            throw curr_lex;
        }
        poliz.push_back(Lex(POLIZ_LDEC));
        gl();
    }
    else
    {
        E1();
        if (c_type == LEX_EQ || c_type == LEX_LSS || c_type == LEX_GRT || c_type == LEX_LEQ || c_type == LEX_GEQ || c_type == LEX_NEQ)
        {
            type_of_lex op = c_type;
            st_lex.push(c_type);
            if (!(c_type == LEX_DEC || c_type ==  LEX_INC))
            {
                gl();
                E1();
                poliz.push_back (Lex (op) );
            }
            else 
            {
                if (c_type == LEX_DEC)
                {
                    poliz[poliz.size() - 1].put_type(POLIZ_ADDRESS);
                    gl();
                    poliz.push_back (Lex (POLIZ_RDEC) );
                }
                if (c_type == LEX_INC)
                {
                    poliz[poliz.size() - 1].put_type(POLIZ_ADDRESS);
                    gl();
                    poliz.push_back (Lex (POLIZ_RINC) );
                }
            }
        }
    }
}

void Parser::E1()
{
    T();
    while (c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_DPIPE)
    {
        type_of_lex op = c_type;
        st_lex.push(c_type);
        gl();
        T();
        poliz.push_back (Lex (op) );
    }
}

void Parser::T()
{
    F();
    while (c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_DAMP || c_type == LEX_REM)
    {
        type_of_lex op = c_type;
        st_lex.push(c_type);
        gl();
        F();
        poliz.push_back (Lex (op) );
    }
}

void Parser::F()
{
    if (c_type == LEX_ID)
    {
        check_id();
        poliz.push_back(Lex(LEX_ID, c_val));
        gl();
    }
    else if (c_type == LEX_NUMBER)
    {
        st_lex.push(LEX_NUMBER);
        poliz.push_back(curr_lex);
        gl();
    }
    else if (c_type == LEX_TRUE)
    {
        st_lex.push(LEX_BOOLEAN);
        poliz.push_back(Lex(LEX_TRUE, 1));
        gl();
    }
    else if (c_type == LEX_FALSE)
    {
        st_lex.push(LEX_BOOLEAN);
        poliz.push_back(Lex(LEX_FALSE, 0));
        gl();
    }
    else if (c_type == LEX_CONSTR)
    {
        st_lex.push(LEX_CONSTR);
        poliz.push_back(Lex(curr_lex));
        gl();
    }
    else if (c_type == LEX_NOT)
    {
        gl();
        F();
        poliz.push_back (Lex (LEX_NOT));
    }
    else if (c_type == LEX_LPAREN)
    {
        gl();
        E();
        if (c_type == LEX_RPAREN)
            gl();
        else
            throw curr_lex;
    }
    else
        throw curr_lex;
}

void Parser::FOR_PARAMETERS()
{
    int l_v_index = curr_lex.get_value();
    type_of_lex new_val;
    check_id();
    poliz.push_back(Lex(POLIZ_ADDRESS, c_val));
    gl();
    if (c_type == LEX_ASSIGN || c_type == LEX_MINEQ || c_type == LEX_PLEQ ||
        c_type == LEX_TIMESEQ || c_type == LEX_REMEQ || c_type == LEX_SLEQ)
    {
        gl();
        E();
        eq_type(new_val);
        TID[l_v_index].put_type(new_val);
        poliz.push_back(Lex(LEX_ASSIGN));
    }
    else if (c_type == LEX_INC)
    {
        poliz.push_back(Lex(POLIZ_RINC));
        gl();
        if (c_type!=LEX_SEMICOLON && c_type!=LEX_RPAREN)
            throw curr_lex;
        if (c_type == LEX_SEMICOLON)
            gl();
    }
    else if (c_type == LEX_DEC)
    {
        poliz.push_back(Lex(POLIZ_RDEC));
        gl();
        if (c_type != LEX_SEMICOLON && c_type!=LEX_RPAREN)
            throw curr_lex;
        if (c_type == LEX_SEMICOLON)
            gl();
    }
    else
        throw curr_lex;
}

////////////////////////////////////////////////////////////////

void Parser::dec ( type_of_lex type, int i ) 
{
    if ( TID[i].get_declare() ) 
        throw "twice";
    else 
    {
        TID[i].put_declare();
        TID[i].put_type(type);
    }
}
 
void Parser::check_id () 
{
    if ( TID[c_val].get_declare() )
        st_lex.push ( TID[c_val].get_type() );
    else 
        throw "not declared";
}
 
void Parser::eq_type (type_of_lex& new_val) 
{
    from_st(st_lex, new_val);
    if (new_val == LEX_UNDEFINED)
    {
        throw "wrong types are in =";
    }
}

////////////////////////////////////////////////////////////////

class Executer {
public:
    void execute ( vector<Lex> & poliz );
};
 
void Executer::execute ( vector<Lex> & poliz ) {
    Lex pc_el;
    stack < Lex > args;
    int i, index = 0, size = poliz.size(), j;
    Lex one, two;
    stack <int> inc_st;
    stack <int> dec_st;
    int inc_dec_tmp;
    while ( index < size ) 
    {
        pc_el = poliz [ index ];
        switch ( pc_el.get_type () ) 
        {
            case LEX_TRUE: case LEX_FALSE: case LEX_NUMBER: case POLIZ_ADDRESS: case POLIZ_LABEL: case LEX_CONSTR:
                args.push ( pc_el );
                break;
 
            case LEX_ID:
                i = pc_el.get_value ();
                if ( TID[i].get_assign () ) 
                {
                    args.push (Lex(TID[i].get_type(), TID[i].get_value(), TID[i].get_string()));
                    break;
                }
                else
                    throw "POLIZ: indefinite identifier";
 
            case LEX_NOT:
                from_st(args,one);
                if (one.get_type() == LEX_CONSTR)
                    throw "I can't make not with string";
                else if (one.get_type() == LEX_NUMBER)
                {
                    int a = one.get_value();
                    if (a == 0)
                        one.put_type(LEX_TRUE);
                    else one.put_type(LEX_FALSE);
                }
                else if (one.get_type() == POLIZ_ADDRESS || one.get_type() == POLIZ_LABEL)
                    throw "NOT error";
                else 
                {
                    if (one.get_type() == LEX_TRUE)
                        one.put_type(LEX_FALSE);
                    if (one.get_type() == LEX_FALSE)
                        one.put_type(LEX_TRUE);
                }
                args.push( one );
                break;
 
            case LEX_DPIPE:
                from_st(args,one); 
                from_st(args,two);
                if (one.get_type() == LEX_NUMBER)
                {
                    if (one.get_value() > 0) j = 1;
                    else j = 0;
                }
                else if (one.get_type() == LEX_TRUE) j = 1;
                else if (one.get_type() == LEX_FALSE) j = 0;
                else 
                    throw "I can't make OR with this types";
                if (two.get_type() == LEX_NUMBER)
                {
                    if (two.get_value() > 0) j++;
                }
                else if (two.get_type() == LEX_TRUE) j++;
                else if (two.get_type() == LEX_FALSE);
                else 
                    throw "I can't make OR with this types";
                if (j == 0)
                    one.put_type(LEX_FALSE);
                else one.put_type(LEX_TRUE);
                args.push( one );
                break;
 
            case LEX_DAMP:
                from_st(args,one); 
                from_st(args,two);
                if (one.get_type() == LEX_NUMBER)
                {
                    if (one.get_value() > 0) j = 1;
                    else j = 0;
                }
                else if (one.get_type() == LEX_TRUE) j = 1;
                else if (one.get_type() == LEX_FALSE) j = 0;
                else 
                    throw "I can't make AND with this types";
                if (two.get_type() == LEX_NUMBER)
                {
                    if (two.get_value() == 0) j = 0;
                }
                else if (two.get_type() == LEX_TRUE);
                else if (two.get_type() == LEX_TRUE) j = 0;
                else 
                    throw "I can't make AND with this types";
                if (j == 0)
                    one.put_type(LEX_FALSE);
                else one.put_type(LEX_TRUE);
                args.push( one );
                break;
 
            case POLIZ_GO:
                from_st(args,one);
                index = one.get_value() - 1;
                break;
 
            case POLIZ_FGO:
                from_st(args,one);
                from_st(args,two);
                if (two.get_type() != LEX_TRUE && two.get_type() != LEX_FALSE)
                    throw "FGO error";
                if ( two.get_type() == LEX_FALSE ) index = one.get_value() - 1;
                break;
 
            case LEX_PLUS:
                from_st(args,one);
                from_st(args,two);
                if (one.get_type() == LEX_TRUE)
                {
                    one.put_value(1);
                    one.put_type(LEX_NUMBER);
                }
                else if (one.get_type() == LEX_FALSE)
                {
                    one.put_value(0);
                    one.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_TRUE)
                {
                    two.put_value(1);
                    two.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_FALSE)
                {
                    two.put_value(0);
                    two.put_type(LEX_NUMBER);
                }
                if (one.get_type() == LEX_CONSTR && two.get_type() == LEX_CONSTR)
                {
                    string str = one.get_string() + two.get_string();
                    one.put_string(str);
                }
                else if (one.get_type() == LEX_NUMBER && two.get_type() == LEX_NUMBER)
                {
                    j = one.get_value() + two.get_value();
                    one.put_value(j);
                }
                else 
                {
                    cout << one;
                    cout << two;
                    throw "PLUS error";
                }
                args.push ( one );
                break;

            case LEX_PLEQ:
                from_st(args,two);
                from_st(args,one);
                if (one.get_type() != POLIZ_ADDRESS)
                {
                    throw "PLEQ error";
                }
                j = one.get_value();
                if ( TID[j].get_assign() )
                { 
                    if (TID[j].get_type() == LEX_TRUE)
                    {
                        TID[j].put_value(1);
                        TID[j].put_type(LEX_NUMBER);
                    }
                    else if (TID[j].get_type() == LEX_FALSE)
                    {
                        TID[j].put_value(0);
                        TID[j].put_type(LEX_NUMBER);
                    }
                    else if (two.get_type() == LEX_TRUE)
                    {
                        two.put_value(1);
                        two.put_type(LEX_NUMBER);
                    }
                    else if (two.get_type() == LEX_FALSE)
                    {
                        two.put_value(0);
                        two.put_type(LEX_NUMBER);
                    }
                    if (TID[j].get_type() == LEX_CONSTR && two.get_type() == LEX_CONSTR)
                    {
                        TID[j].put_string(TID[j].get_string() + two.get_string());
                    }
                    else if (one.get_type() == LEX_NUMBER && two.get_type() == LEX_NUMBER)
                    {
                        TID[j].put_value(TID[j].get_value() + two.get_value());
                    }
                    else throw "PLEQ error 1";
                }
                else throw "PLEQ error 2";
                while (!inc_st.empty())
                {
                    inc_dec_tmp = inc_st.top();
                    inc_st.pop();
                    TID[inc_dec_tmp].put_value(TID[inc_dec_tmp].get_value() + 1);
                }
                while (!dec_st.empty())
                {
                    inc_dec_tmp = dec_st.top();
                    dec_st.pop();
                    TID[inc_dec_tmp].put_value(TID[inc_dec_tmp].get_value() - 1);
                }
                break;
 
            case LEX_TIMES:
                from_st(args,one);
                from_st(args,two);
                if (one.get_type() == LEX_TRUE)
                {
                    one.put_value(1);
                    one.put_type(LEX_NUMBER);
                }
                else if (one.get_type() == LEX_FALSE)
                {
                    one.put_value(0);
                    one.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_TRUE)
                {
                    two.put_value(1);
                    two.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_FALSE)
                {
                    two.put_value(0);
                    two.put_type(LEX_NUMBER);
                }
                if (one.get_type() == LEX_NUMBER && two.get_type() == LEX_NUMBER)
                {
                    int j = one.get_value() * two.get_value();
                    one.put_value(j);
                }
                else 
                    throw "TIMES error";
                args.push ( one );
                break;
                
            case LEX_TIMESEQ:
                from_st(args,two);
                from_st(args,one);
                if (one.get_type() != POLIZ_ADDRESS)
                {
                    throw "TIMESEQ error";
                }
                j = one.get_value();
                if ( TID[j].get_assign() )
                { 
                    if (TID[j].get_type() == LEX_TRUE)
                    {
                        TID[j].put_value(1);
                        TID[j].put_type(LEX_NUMBER);
                    }
                    else if (TID[j].get_type() == LEX_FALSE)
                    {
                        TID[j].put_value(0);
                        TID[j].put_type(LEX_NUMBER);
                    }
                    else if (two.get_type() == LEX_TRUE)
                    {
                        two.put_value(1);
                        two.put_type(LEX_NUMBER);
                    }
                    else if (two.get_type() == LEX_FALSE)
                    {
                        two.put_value(0);
                        two.put_type(LEX_NUMBER);
                    }
                    else if (one.get_type() == LEX_NUMBER && two.get_type() == LEX_NUMBER)
                    {
                        TID[j].put_value(TID[j].get_value() * two.get_value());
                    }
                    else throw "TIMEQ error 1";
                }
                else throw "TIMEQ error 2";
                while (!inc_st.empty())
                {
                    inc_dec_tmp = inc_st.top();
                    inc_st.pop();
                    TID[inc_dec_tmp].put_value(TID[inc_dec_tmp].get_value() + 1);
                }
                while (!dec_st.empty())
                {
                    inc_dec_tmp=dec_st.top();
                    dec_st.pop();
                    TID[inc_dec_tmp].put_value(TID[inc_dec_tmp].get_value() - 1);
                }
                break;
 
            case LEX_MINUS:
                from_st(args,one);
                from_st(args,two);
                if (one.get_type() == LEX_TRUE)
                {
                    one.put_value(1);
                    one.put_type(LEX_NUMBER);
                }
                else if (one.get_type() == LEX_FALSE)
                {
                    one.put_value(0);
                    one.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_TRUE)
                {
                    two.put_value(1);
                    two.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_FALSE)
                {
                    two.put_value(0);
                    two.put_type(LEX_NUMBER);
                }
                if (one.get_type() == LEX_NUMBER && two.get_type() == LEX_NUMBER)
                {
                    int j = two.get_value() - one.get_value();
                    one.put_value(j);
                }
                else 
                    throw "MINUS error";
                args.push ( one );
                break;
                
            case LEX_MINEQ:
                from_st(args,two);
                from_st(args,one);
                if (one.get_type() != POLIZ_ADDRESS)
                {
                    throw "MINEQ error";
                }
                j = one.get_value();
                if ( TID[j].get_assign() )
                { 
                    if (TID[j].get_type() == LEX_TRUE)
                    {
                        TID[j].put_value(1);
                        TID[j].put_type(LEX_NUMBER);
                    }
                    else if (TID[j].get_type() == LEX_FALSE)
                    {
                        TID[j].put_value(0);
                        TID[j].put_type(LEX_NUMBER);
                    }
                    else if (two.get_type() == LEX_TRUE)
                    {
                        two.put_value(1);
                        two.put_type(LEX_NUMBER);
                    }
                    else if (two.get_type() == LEX_FALSE)
                    {
                        two.put_value(0);
                        two.put_type(LEX_NUMBER);
                    }
                    else if (one.get_type() == LEX_NUMBER && two.get_type() == LEX_NUMBER)
                    {
                        TID[j].put_value(TID[j].get_value() - two.get_value());
                    }
                    else throw "MINEQ error 1";
                }
                else throw "MINEQ error 2";
                while (!inc_st.empty())
                {
                    inc_dec_tmp=inc_st.top();
                    inc_st.pop();
                    TID[inc_dec_tmp].put_value(TID[inc_dec_tmp].get_value() + 1);
                }
                while (!dec_st.empty())
                {
                    inc_dec_tmp=dec_st.top();
                    dec_st.pop();
                    TID[inc_dec_tmp].put_value(TID[inc_dec_tmp].get_value() - 1);
                }
                break;
 
            case LEX_SLASH:
                from_st(args,two);
                from_st(args,one);
                if (one.get_type() == LEX_TRUE)
                {
                    one.put_value(1);
                    one.put_type(LEX_NUMBER);
                }
                else if (one.get_type() == LEX_FALSE)
                {
                    one.put_value(0);
                    one.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_TRUE)
                {
                    two.put_value(1);
                    two.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_FALSE)
                {
                    two.put_value(0);
                    two.put_type(LEX_NUMBER);
                }
                if (one.get_type() == LEX_NUMBER && two.get_type() == LEX_NUMBER)
                {
                    if (!two.get_type())
                    {
                        int j = one.get_value() / two.get_value();
                        one.put_value(j);
                    }
                    else throw "POLIZ:divide by zero";
                }
                else 
                    throw "SLASH error";
                args.push ( one );
                break;
            
            case LEX_SLEQ:
                from_st(args,two);
                from_st(args,one);
                if (one.get_type() != POLIZ_ADDRESS)
                {
                    throw "SLEQ error";
                }
                j = one.get_value();
                if ( TID[j].get_assign() )
                { 
                    if (TID[j].get_type() == LEX_TRUE)
                    {
                        TID[j].put_value(1);
                        TID[j].put_type(LEX_NUMBER);
                    }
                    else if (TID[j].get_type() == LEX_FALSE)
                    {
                        TID[j].put_value(0);
                        TID[j].put_type(LEX_NUMBER);
                    }
                    else if (two.get_type() == LEX_TRUE)
                    {
                        two.put_value(1);
                        two.put_type(LEX_NUMBER);
                    }
                    else if (two.get_type() == LEX_FALSE)
                    {
                        two.put_value(0);
                        two.put_type(LEX_NUMBER);
                    }
                    else if (one.get_type() == LEX_NUMBER && two.get_type() == LEX_NUMBER)
                    {
                        if (!one.get_value())
                            TID[j].put_value(TID[j].get_value() / two.get_value());
                        else throw "POLIZ:divide by zero";
                    }
                    else throw "SLEQ error 1";
                }
                else throw "SLEQ error 2";
                while (!inc_st.empty())
                {
                    inc_dec_tmp = inc_st.top();
                    inc_st.pop();
                    TID[inc_dec_tmp].put_value(TID[inc_dec_tmp].get_value() + 1);
                }
                while (!dec_st.empty())
                {
                    inc_dec_tmp = dec_st.top();
                    dec_st.pop();
                    TID[inc_dec_tmp].put_value(TID[inc_dec_tmp].get_value() - 1);
                }
                break;
                
            case LEX_REM:
                from_st(args,two);
                from_st(args,one);
                if (one.get_type() == LEX_TRUE)
                {
                    one.put_value(1);
                    one.put_type(LEX_NUMBER);
                }
                else if (one.get_type() == LEX_FALSE)
                {
                    one.put_value(0);
                    one.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_TRUE)
                {
                    two.put_value(1);
                    two.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_FALSE)
                {
                    two.put_value(0);
                    two.put_type(LEX_NUMBER);
                }
                if (one.get_type() == LEX_NUMBER && two.get_type() == LEX_NUMBER)
                {
                    if (!two.get_type())
                    {
                        int j = one.get_value() % two.get_value();
                        one.put_value(j);
                    }
                    else throw "POLIZ:divide by zero";
                }
                else 
                    throw "REM error";
                args.push ( one );
                break;
                
            case LEX_REMEQ:
                from_st(args,two);
                from_st(args,one);
                if (one.get_type() != POLIZ_ADDRESS)
                {
                    throw "REMEQ error";
                }
                j = one.get_value();
                if ( TID[j].get_assign() )
                { 
                    if (TID[j].get_type() == LEX_TRUE)
                    {
                        TID[j].put_value(1);
                        TID[j].put_type(LEX_NUMBER);
                    }
                    else if (TID[j].get_type() == LEX_FALSE)
                    {
                        TID[j].put_value(0);
                        TID[j].put_type(LEX_NUMBER);
                    }
                    else if (two.get_type() == LEX_TRUE)
                    {
                        two.put_value(1);
                        two.put_type(LEX_NUMBER);
                    }
                    else if (two.get_type() == LEX_FALSE)
                    {
                        two.put_value(0);
                        two.put_type(LEX_NUMBER);
                    }
                    else if (one.get_type() == LEX_NUMBER && two.get_type() == LEX_NUMBER)
                    {
                        if (!one.get_value())
                            TID[j].put_value(TID[j].get_value() % two.get_value());
                        else throw "POLIZ:divide by zero";
                    }
                    else throw "REMEQ error 1";
                }
                else throw "REMEQ error 2";
                while (!inc_st.empty())
                {
                    inc_dec_tmp = inc_st.top();
                    inc_st.pop();
                    TID[inc_dec_tmp].put_value(TID[inc_dec_tmp].get_value() + 1);
                }
                while (!dec_st.empty())
                {
                    inc_dec_tmp = dec_st.top();
                    dec_st.pop();
                    TID[inc_dec_tmp].put_value(TID[inc_dec_tmp].get_value() - 1);
                }
                break;
 
            case LEX_EQ:
                from_st(args,two);
                from_st(args,one);
                if (one.get_type() == LEX_TRUE)
                {
                    one.put_value(1);
                    one.put_type(LEX_NUMBER);
                }
                else if (one.get_type() == LEX_FALSE)
                {
                    one.put_value(0);
                    one.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_TRUE)
                {
                    two.put_value(1);
                    two.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_FALSE)
                {
                    two.put_value(0);
                    two.put_type(LEX_NUMBER);
                }
                if (one.get_type() == LEX_CONSTR && two.get_type() == LEX_CONSTR)
                {
                    if (one.get_string() == two.get_string())
                        one.put_type(LEX_TRUE);
                    else one.put_type(LEX_FALSE);
                }
                else if (one.get_type() == LEX_NUMBER && two.get_type() == LEX_NUMBER)
                {
                    if( one.get_value() == two.get_value())
                        one.put_type(LEX_TRUE);
                    else one.put_type(LEX_FALSE);
                }
                else 
                    throw "EQ error";
                args.push ( one );
                break;
 
            case LEX_LSS:
                from_st(args,two);
                from_st(args,one);
                if (one.get_type() == LEX_TRUE)
                {
                    one.put_value(1);
                    one.put_type(LEX_NUMBER);
                }
                else if (one.get_type() == LEX_FALSE)
                {
                    one.put_value(0);
                    one.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_TRUE)
                {
                    two.put_value(1);
                    two.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_FALSE)
                {
                    two.put_value(0);
                    two.put_type(LEX_NUMBER);
                }
                if (one.get_type() == LEX_CONSTR && two.get_type() == LEX_CONSTR)
                {
                    if (one.get_string() < two.get_string())
                        one.put_type(LEX_TRUE);
                    else one.put_type(LEX_FALSE);
                }
                else if (one.get_type() == LEX_NUMBER && two.get_type() == LEX_NUMBER)
                {
                    if( one.get_value() < two.get_value())
                        one.put_type(LEX_TRUE);
                    else one.put_type(LEX_FALSE);
                }
                else 
                    throw "LSS error";
                args.push ( one );
                break;
 
            case LEX_GRT:
                from_st(args,two);
                from_st(args,one);
                if (one.get_type() == LEX_TRUE)
                {
                    one.put_value(1);
                    one.put_type(LEX_NUMBER);
                }
                else if (one.get_type() == LEX_FALSE)
                {
                    one.put_value(0);
                    one.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_TRUE)
                {
                    two.put_value(1);
                    two.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_FALSE)
                {
                    two.put_value(0);
                    two.put_type(LEX_NUMBER);
                }
                if (one.get_type() == LEX_CONSTR && two.get_type() == LEX_CONSTR)
                {
                    if (one.get_string() > two.get_string())
                        one.put_type(LEX_TRUE);
                    else one.put_type(LEX_FALSE);
                }
                else if (one.get_type() == LEX_NUMBER && two.get_type() == LEX_NUMBER)
                {
                    if( one.get_value() > two.get_value())
                        one.put_type(LEX_TRUE);
                    else one.put_type(LEX_FALSE);
                }
                else 
                    throw "GTR error";
                args.push ( one );
                break;
 
            case LEX_LEQ:
                from_st(args,two);
                from_st(args,one);
                if (one.get_type() == LEX_TRUE)
                {
                    one.put_value(1);
                    one.put_type(LEX_NUMBER);
                }
                else if (one.get_type() == LEX_FALSE)
                {
                    one.put_value(0);
                    one.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_TRUE)
                {
                    two.put_value(1);
                    two.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_FALSE)
                {
                    two.put_value(0);
                    two.put_type(LEX_NUMBER);
                }
                if (one.get_type() == LEX_CONSTR && two.get_type() == LEX_CONSTR)
                {
                    if (one.get_string() <= two.get_string())
                        one.put_type(LEX_TRUE);
                    else one.put_type(LEX_FALSE);
                }
                else if (one.get_type() == LEX_NUMBER && two.get_type() == LEX_NUMBER)
                {
                    if( one.get_value() <= two.get_value())
                        one.put_type(LEX_TRUE);
                    else one.put_type(LEX_FALSE);
                }
                else 
                    throw "LEQ error";
                args.push ( one );
                break;
 
            case LEX_GEQ:
                from_st(args,two);
                from_st(args,one);
                if (one.get_type() == LEX_TRUE)
                {
                    one.put_value(1);
                    one.put_type(LEX_NUMBER);
                }
                else if (one.get_type() == LEX_FALSE)
                {
                    one.put_value(0);
                    one.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_TRUE)
                {
                    two.put_value(1);
                    two.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_FALSE)
                {
                    two.put_value(0);
                    two.put_type(LEX_NUMBER);
                }
                if (one.get_type() == LEX_CONSTR && two.get_type() == LEX_CONSTR)
                {
                    if (one.get_string() >= two.get_string())
                        one.put_type(LEX_TRUE);
                    else one.put_type(LEX_FALSE);
                }
                else if (one.get_type() == LEX_NUMBER && two.get_type() == LEX_NUMBER)
                {
                    if( one.get_value() >= two.get_value())
                        one.put_type(LEX_TRUE);
                    else one.put_type(LEX_FALSE);
                }
                else 
                    throw "GEQ error";
                args.push ( one );
                break;
 
            case LEX_NEQ:
                from_st(args,two);
                from_st(args,one);
                if (one.get_type() == LEX_TRUE)
                {
                    one.put_value(1);
                    one.put_type(LEX_NUMBER);
                }
                else if (one.get_type() == LEX_FALSE)
                {
                    one.put_value(0);
                    one.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_TRUE)
                {
                    two.put_value(1);
                    two.put_type(LEX_NUMBER);
                }
                else if (two.get_type() == LEX_FALSE)
                {
                    two.put_value(0);
                    two.put_type(LEX_NUMBER);
                }
                if (one.get_type() == LEX_CONSTR && two.get_type() == LEX_CONSTR)
                {
                    if (one.get_string() != two.get_string())
                        one.put_type(LEX_TRUE);
                    else one.put_type(LEX_FALSE);
                }
                else if (one.get_type() == LEX_NUMBER && two.get_type() == LEX_NUMBER)
                {
                    if( one.get_value() != two.get_value())
                        one.put_type(LEX_TRUE);
                    else one.put_type(LEX_FALSE);
                }
                else 
                    throw "NEQ error";
                args.push ( one );
                break;
 
            case LEX_ASSIGN:
                from_st(args,one);
                from_st(args,two);
                j = two.get_value();
                if (one.get_type() == LEX_CONSTR)
                {
                    TID[j].put_string(one.get_string());
                    TID[j].put_type(LEX_CONSTR);
                }
                else if (one.get_type() == LEX_NUMBER)
                {
                    TID[j].put_value(one.get_value());
                    TID[j].put_type(LEX_NUMBER);
                }
                else if (one.get_type() == LEX_TRUE)
                    TID[j].put_type(LEX_TRUE);
                else if (one.get_type() == LEX_FALSE)
                    TID[j].put_type(LEX_FALSE);
                TID[j].put_assign(); 
                while (!inc_st.empty())
                {
                    inc_dec_tmp = inc_st.top();
                    inc_st.pop();
                    TID[inc_dec_tmp].put_value(TID[inc_dec_tmp].get_value() + 1);
                }
                while (!dec_st.empty())
                {
                    inc_dec_tmp = dec_st.top();
                    dec_st.pop();
                    TID[inc_dec_tmp].put_value(TID[inc_dec_tmp].get_value() - 1);
                }
                break;
                
            case POLIZ_LINC:
                from_st(args,one);
                if (one.get_type() != POLIZ_ADDRESS)
                    throw "LINK error";
                j = one.get_value();
                if ( TID[j].get_assign () )
                {
                    if (TID[j].get_type() == LEX_NUMBER)
                    {
                        int k = TID[j].get_value();
                        TID[j].put_value(k + 1);
                        args.push (Lex(LEX_NUMBER, k + 1 , pc_el.get_string()));
                        break;
                    }
                    else
                    {
                        throw "LINC error 2";
                    }
                }
                else throw "LINC error 3";
                break;
            
            case POLIZ_RINC:
                from_st(args,one);
                if (one.get_type() != POLIZ_ADDRESS)
                {
                    throw "RINC error 1";
                }
                j = one.get_value();
                if ( TID[j].get_assign ())
                {
                    if (TID[j].get_type() == LEX_NUMBER)
                    {
                        int k;
                        args.push (Lex(LEX_NUMBER, k = TID[j].get_value(), pc_el.get_string()));
                        inc_st.push(j);
                    }
                    else
                    {
                        throw "RINC error 2";
                    }
                }
                else throw "RINC error 3";
                break;
            
            case POLIZ_LDEC:
                from_st(args,one);
                j = one.get_value();
                if ( TID[j].get_assign () )
                {
                    if (TID[j].get_type() == LEX_NUMBER)
                    {
                        int k = TID[j].get_value();
                        TID[j].put_value(k - 1);
                        args.push (Lex(LEX_NUMBER, k - 1 , pc_el.get_string()));
                        break;
                    }
                    else
                    {
                        throw "LDEC error 2";
                    }
                }
                else throw "LDEC error 3";
                break;
            
            case POLIZ_RDEC:
                from_st(args,one);
                if (one.get_type() != POLIZ_ADDRESS)
                {
                    throw "RDEC error 1";
                }
                j = one.get_value();
                if ( TID[j].get_assign ())
                {
                    if (TID[j].get_type() == LEX_NUMBER)
                    {
                        int k;
                        args.push (Lex(LEX_NUMBER, k = TID[j].get_value(), pc_el.get_string()));
                        dec_st.push(j);
                    }
                    else
                    {
                        throw "RDEC error 2";
                    }
                }
                else throw "RDEC error 3";
                break;
                
            case POLIZ_WRITE:
                from_st(args, one);
                if (one.get_type() == POLIZ_ADDRESS)
                    if (TID[one.get_value()].get_string() != "")
                        cout << TID[one.get_value()].get_string() << endl;
                    else
                        cout << TID[one.get_value()].get_value() << endl;
                else if (one.get_type() == LEX_CONSTR)
                    cout << one.get_string() << endl;
                else 
                    cout << one.get_value() << endl;
                break;
                
            case POLIZ_GETENV:
                from_st(args,one);
                if (one.get_type() == POLIZ_ADDRESS)
                    if (TID[one.get_value()].get_string() != "")
                        cout << getenv(TID[one.get_value()].get_string().c_str()) << endl;
                    else
                        throw "Impossible to apply";
                else if (one.get_type() == LEX_CONSTR)
                {
                    try
                    {
                        string str = getenv(one.get_string().c_str());
                        cout << str << endl;
                    }
                    catch(...)
                    {
                        throw "Environment error";
                    }
                }
                else 
                    throw "Impossible to apply";
                break;
 
            default:
                throw "POLIZ: unexpected elem";
        }//end of switch
        ++index;
    };//end of while
    /*unsigned int s = 0;
    while (s < TID.size())
    {
        cout << TID[s].get_name() << '_' << TID[s].get_value() << endl;
        s++;
    }*/
    cout << "Finish of executing!!!" << endl;
}
 
class Interpretator {
    Parser   pars;
    Executer E;
public:
    Interpretator  (const char* program): pars (program) {}
    void interpretation ();
};
 
void Interpretator::interpretation () 
{
    pars.analyze ();
    E.execute ( pars.poliz );
} 

int main () {
    try
    {
        Interpretator I ("program.txt");
        I.interpretation ();
        /*Parser pars("program.txt");
        pars.analyze();*/
        return 0;
    }
    
    catch (char c) 
    {
        cout << "unexpected symbol " << c << endl;
        return 1;
    }
    
    catch (const char* str)
    {
        cout << str << endl;
        return 1;
    }
    
    catch (Lex l)
    {
        cout << "\nerror\n";
        cout << l << endl;
        return 1;
    }
    
    return 0;
}

// string
