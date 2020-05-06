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
    LEX_BOOLEAN, LEX_DO, LEX_ELSE, LEX_IF, LEX_FALSE, LEX_NUMBER, LEX_STRING, LEX_TRUE, LEX_VAR, LEX_WHILE, LEX_BREAK, LEX_CASE, LEX_CONTINUE, LEX_GOTO, LEX_FOR, LEX_FUNCTION, LEX_IN, LEX_INSTANCE, LEX_NAN, LEX_NILL, LEX_RETURN, LEX_TYPEOF, LEX_VOID, LEX_WITH, LEX_OBJECT, LEX_UNDEFINED,
    LEX_FIN, //27
    LEX_AMP, LEX_DAMP, LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_ASSIGN, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS, LEX_GRT, LEX_PLUS, LEX_PLEQ, /*40 */ LEX_MINUS, LEX_MINEQ, LEX_PIPE, LEX_DPIPE, LEX_REM, LEX_REMEQ, LEX_SLASH, LEX_SLEQ, LEX_TIMES, LEX_TIMESEQ, LEX_SLASHTIMES, LEX_TIMESLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ, LEX_INC, LEX_DEC, LEX_POINT, LEX_LBRACK, LEX_RBRACK, LEX_STEQ, LEX_STNEQ, LEX_BEGIN, LEX_END, LEX_NOT, LEX_DSLASH,
    LEX_CONSTR, //67
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


/////////////////////  Класс Ident  ////////////////////////////


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
Scanner::TW [] = { "", "Boolean", "do", "else", "if", "false", "Number", "String", "true", "var", "while", "break", "case", "continue", "goto", "for",    "function",    "in", "instanceof", "NaN", "null",    "return", "typeof", "void", "with", "Object", "undefined", NULL};

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
    if ( l.t_lex < 27 )
        t = Scanner::TW[l.t_lex];
    else if (l.t_lex >= 27 && l.t_lex <= 66)
    t = Scanner::TD[l.t_lex-27];
    else if (l.t_lex == 6)
    t = "NUMBER";
    else if (l.t_lex == 67)
    {      
        t = "STRING CONST";
        s << '(' << t << ',' << l.s_lex << ");" << endl;
        return s;
    }
    else if (l.t_lex == 68)
        t = TID[l.v_lex].get_name();
    else if (l.t_lex == 69)
        t = "Label";
    else if(l.t_lex == 70)
        t = "Addr";
    else if (l.t_lex == 71)
        t = "!";
    else if (l.t_lex == 72) 
        t = "!F";
    else if (l.t_lex == 73)
        t = "+#";
    else if (l.t_lex == 74)
        t = "#+";
    else if (l.t_lex == 75)
        t = "-#";
    else if (l.t_lex == 76)
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
    void FOR_PARAMETERS();
    void dec(type_of_lex type, int i);
    void check_id();
    void check_not();
    void eq_type(type_of_lex &);
    void eq_bool();
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
            eq_bool();
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
            eq_bool();
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
                eq_bool();
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
        F();
        poliz.push_back(Lex(POLIZ_LINC));
    }
    else if (c_type == LEX_DEC)
    {
        gl();
        F();
        poliz.push_back(Lex(POLIZ_LDEC));
    }
    else
    {
        E1();
        if (c_type == LEX_EQ || c_type == LEX_LSS || c_type == LEX_GRT || c_type == LEX_LEQ || c_type == LEX_GEQ || c_type == LEX_NEQ)
        {
            st_lex.push(c_type);
            gl();
            E1();
        }
    }
}

void Parser::E1()
{
    T();
    while (c_type == LEX_PLUS || c_type == LEX_MINUS || c_type == LEX_DPIPE)
    {
        st_lex.push(c_type);
        gl();
        T();
    }
}

void Parser::T()
{
    F();
    while (c_type == LEX_TIMES || c_type == LEX_SLASH || c_type == LEX_DAMP || c_type == LEX_REM)
    {
        st_lex.push(c_type);
        gl();
        F();
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
        st_lex.push(LEX_STRING);
        poliz.push_back(Lex(curr_lex));
        gl();
    }
    else if (c_type == LEX_NOT)
    {
        gl();
        F();
        check_not();
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
 
void Parser::check_not () 
{
    if (st_lex.top() != LEX_BOOLEAN)
        throw "wrong type is in not";
    else  
        poliz.push_back (Lex (LEX_NOT));
}
 
void Parser::eq_type (type_of_lex & t1) 
{
    type_of_lex t2;
    from_st(st_lex, t2);
    from_st(st_lex, t1);
    if (t1 == LEX_UNDEFINED)
    {
        t1 = t2;
    }
    else if ( t1 != t2)
    {
        cout << t1 << ' ' << t2 << endl;
        throw "wrong types are in =";
    }
}
 
void Parser::eq_bool () 
{
    if ( st_lex.top() != LEX_BOOLEAN )
        throw "expression is not boolean";
    st_lex.pop();
}
////////////////////////////////////////////////////////////////

int main () {
    try
    {
        Parser pars("program.txt");
        pars.analyze();
    }
    
    catch (const char* str)
    {
        cout << str << endl;
        return 1;
    }
    
    catch (Lex l)
    {
        cout << "error\n";
        cout << l << endl;
        return 1;
    }
    
    return 0;
}
