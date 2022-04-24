#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <algorithm>
using namespace std; 

enum type_of_lex {
    LEX_NULL,                                                                                   /* 0*/
    LEX_PROGRAM, LEX_STRING, LEX_INT, LEX_WHILE, LEX_READ, LEX_WRITE, LEX_GOTO, 
    LEX_IF, LEX_ELSE, LEX_BOOLEAN, LEX_TRUE, LEX_FALSE,         
    LEX_AND, LEX_NOT, LEX_OR,  LEX_CONTINU,                                                               /*15*/
    LEX_FIN,                                                                                    /*16*/
    LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_LPAREN, LEX_RPAREN, LEX_EQ, LEX_LSS,               
    LEX_GTR, LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LEQ, LEX_NEQ, LEX_GEQ,              
    LEX_DEQ, LEX_PER, LEX_TSLASH, LEX_SLASHT, LEX_BEGIN, LEX_END,                               /*37*/
    LEX_NUM,                                                                                    /*38*/
    LEX_STR,                                                                                    /*39*/
    LEX_ID,                                                                                     /*40*/
    POLIZ_LABEL,                                                                                /*41*/
    POLIZ_ADDRESS,                                                                              /*42*/
    POLIZ_GO,                                                                                   /*43*/
    POLIZ_FGO                                                                                   /*44*/
};

const char * TW[] = { 
    "", "program", "string", "int", 
    "while", "read", "write",
    "goto", "if", "else", 
    "boolean", "true", "false", 
    "and", "not", "or", "continue", 
    NULL };

const char * TD[] = { 
    "", ";", ",", ":", "(", ")", 
    "=", "<", ">", "+", "-", "*", 
    "/","<=", "!=", ">=", "==",  
    "%","*/", "/*", "{", "}",
    NULL 
};

class Lex {
    type_of_lex t_lex;
    int v_lex;
    string l_str;
public:
    Lex(type_of_lex t = LEX_NULL, int v = 0, string str = "") : t_lex(t), v_lex(v), l_str(str) {};
    Lex(const Lex & L) : t_lex(L.t_lex), v_lex(L.v_lex), l_str(L.l_str) {};
    Lex operator=(const Lex & L){ t_lex = L.t_lex; v_lex = L.v_lex; l_str = L.l_str; return *this; };
    type_of_lex get_type() const { return t_lex; };
    int get_value() const { return v_lex; };
    friend ostream & operator<<(ostream & os, Lex L);
};

ostream & operator<<(ostream & os, Lex L){
    cout << "(" << L.t_lex << "," << L.v_lex << "," << L.l_str << ")" << endl;
    return os;
};

class Ident{
    string name;
    bool declare;
    type_of_lex type;
    bool assign;
    int value_int;
    string value_string;
    bool value_boolean;
public:
    Ident() { 
        declare = false; 
        assign = false; 
    };
    bool operator==(const string & s) const{ return name == s; };
    Ident(const string s){
        name = s;
        declare = false; 
        assign  = false;
    };
    string get_name() const{ return name; };
    bool get_declare() const{ return declare; };
    void put_declare (){ declare = true; };
    type_of_lex get_type() const{ return type; };
    void put_type(type_of_lex t){ type = t; };
    bool get_assign () const { return assign; };
    void put_assign(){ assign = true; };
    int get_value_int() const{ return value_int; };
    void put_value_int(int v){ value_int = v; };
    string get_value_string() const{ return value_string; };
    void put_value_string(string v){ value_string = v; };
    bool get_value_boolean() const{ return value_boolean; };
    void put_value_boolean(bool v){ value_boolean = v; };
};

vector<Ident> TID;
int put (const string & buf){
    vector<Ident>::iterator k;
    if ((k = find(TID.begin(), TID.end(), buf)) != TID.end())
        return k - TID.begin();
    TID.push_back(Ident(buf));
    return TID.size() - 1;
};

 
class Scanner {
    FILE * fp;
    char c;
    int look(const string buf, const char ** list){
        int i = 0;
        while(list[i]){
            if(buf == list[i]) return i;
            ++i;
        };
        return 0;
    }
    void gc() { c = fgetc (fp); };
public:
    Scanner(const char * program){
        if(!(fp = fopen(program, "r"))) 
            throw  "can’t open file";
    };
    Lex get_lex ();
};

Lex Scanner::get_lex(){
    enum state{H, IDENT, NUMB, COM, ALE, NEQ, QUOTE, SLASH};
    int n, j;
    string buf;
    state CS = H;
    do {
        gc();
        switch(CS){
            case H:
                if(isspace(c)); 
                else if(isalpha(c)){
                    buf.push_back(c);
                    CS = IDENT;
                }
                else if(isdigit(c)){
                    n = c - '0';
                    CS  = NUMB;
                } 
                else if(c == '<' || c == '>' || c == '='){ 
                    buf.push_back(c);
                    CS  = ALE; 
                }
                else if(c == '/'){ 
                    buf.push_back(c);
                    CS  = SLASH; 
                }
                else if (c == EOF)
                    return Lex(LEX_FIN);
                else if(c == '"'){
                    CS = QUOTE;
                }
                else if(c == '!'){
                    buf.push_back (c);
                    CS  = NEQ;
                }
                else{
                    buf.push_back(c);
                    if((j = look(buf, TD))){
                        return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                    }
                    else throw c;
                }
                break;
            case IDENT:
                if(isalpha(c) || isdigit(c)){
                    buf.push_back(c); 
                }
                else{
                    ungetc(c, fp);
                    if((j = look(buf, TW)))
                        return Lex((type_of_lex) j, j);
                    else{
                        j = put(buf);
                        return Lex(LEX_ID, j);
                    }
                }
                break;
            case NUMB:
                if(isdigit(c)){
                    n = n * 10 + (c - '0');
                }
                else{
                    ungetc(c, fp);
                    return Lex(LEX_NUM, n);
                }
                break;
            case QUOTE:
                if(c != '"') buf.push_back(c); 
                else return Lex(LEX_STR, 0, buf);
                break;
            case SLASH:
                if(c != '*'){
                    ungetc(c, fp); 
                    j = look(buf, TD);
                    return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                }
                else{
                    CS = COM;
                    buf.pop_back();
                }
                break;
            case COM:
                if(c == '*'){
                    gc();
                    if(c == '/'){  CS = H; }
                    else ungetc(c, fp); 
                }
                break;
            case ALE:
                if(c == '=') buf.push_back(c); 
                else ungetc(c, fp); 
                j = look(buf, TD);
                return Lex((type_of_lex)(j + (int)LEX_FIN), j);
                break;
            case NEQ:
                if(c == '='){
                    buf.push_back(c);
                    j = look(buf, TD);
                    return Lex(LEX_NEQ, j);
                }
                else throw '!';
                break;
    } 
  } while (true);
}

int main(int argc, char *argv[]){
    if(argc < 2) cout << "FILE?" << endl;
    else{
        try{
            Lex L;
            Scanner F(argv[1]);
            while( (L = F.get_lex()).get_type() != LEX_FIN) cout << L;
            cout << "TID:" << endl;
            vector<Ident>::const_iterator i;
            for(i = TID.begin(); i != TID.end(); i++) cout << (*i).get_name() << endl;
        }
        catch(char c){
            cout << "error: " << c << endl;
        }
    }
};