#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <stack>
#include <algorithm>
using namespace std; 

enum type_of_lex {
    LEX_NULL,                                                                                   /* 0*/
    LEX_PROGRAM, LEX_STRING, LEX_INT, LEX_WHILE, LEX_READ, LEX_WRITE, LEX_GOTO, 
    LEX_IF, LEX_ELSE, LEX_BOOLEAN, LEX_TRUE, LEX_FALSE,         
    LEX_AND, LEX_NOT, LEX_OR,                                                              /*15*/
    LEX_FIN,                                                                                    /*16*/
    LEX_SEMICOLON, LEX_COMMA, LEX_COLON, LEX_LPAREN, LEX_RPAREN, LEX_EQ,
    LEX_PLUS, LEX_MINUS, LEX_TIMES, LEX_SLASH, LEX_LSS,               
    LEX_GTR,  LEX_LEQ, LEX_NEQ, LEX_GEQ,              
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
    "and", "not", "or", 
    NULL };

const char * TD[] = { 
    "", ";", ",", ":", "(", ")", 
    "=", "+", "-", "*", "/",
    "<", ">","<=", "!=", ">=", "==",  
    "%","*/", "/*", "{", "}",
    NULL 
};

class Lex{
    type_of_lex t_lex;
    int v_lex;
    string l_str;
public:
    Lex(type_of_lex t = LEX_NULL, int v = 0, string str = "") : t_lex(t), v_lex(v), l_str(str) {};
    Lex(const Lex & L) : t_lex(L.t_lex), v_lex(L.v_lex), l_str(L.l_str) {};
    Lex operator=(const Lex & L){ t_lex = L.t_lex; v_lex = L.v_lex; l_str = L.l_str; return *this; };
    type_of_lex get_type() const { return t_lex; };
    int get_value_int() const { return v_lex; };
    int get_value_string() const { return v_lex; };
    friend ostream & operator<<(ostream & os, const Lex L);
};

ostream & operator<<(ostream & os, const Lex L){
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

class Parser{
    Lex curr_lex;
    type_of_lex c_type;
    int c_val;
    string c_str;
    Scanner scan;
    bool flag, flagLbl;
//  stack<int> st_int;
//  stack<type_of_lex> st_lex;
    void Program();
    void Descriptions();
    void Description();
    void Type();
    void Variable();
    void Identifier();
    void Constant();
    void Integer();
    void Sign();
    void StringConst();
    void BoolConst();
    void Operators();
    void Operator();
    void LabelOp();
    void Expression();
    void CompoundOp();
    void ExpressionOp();
    void E();
    void A();
    void C();
    void PM();
    void TS();
    void N();
    void UPM();
    void F();
    void  gl () {
        curr_lex  = scan.get_lex ();
        c_type = curr_lex.get_type ();
        c_val = curr_lex.get_value_int();
        c_str = curr_lex.get_value_string();
        cout << curr_lex;
    }
public:
//  vector<Lex> poliz;
    Parser(const char * program) : scan(program){};
    void analyze();
};

void Parser::analyze(){
    gl();
    Program();
    if(c_type != LEX_FIN) 
        throw curr_lex;
    cout << endl << "Yes!!!" << endl;
}

void Parser::Program(){
    if(c_type == LEX_PROGRAM){ gl (); }
    else throw curr_lex;     
    if(c_type == LEX_BEGIN){ gl (); }
    else throw curr_lex;  
    Descriptions();
    Operators();
    if(c_type == LEX_END) gl();
    else throw curr_lex;
}

void Parser::Descriptions(){
    while((c_type == LEX_INT) || (c_type == LEX_STRING) || (c_type == LEX_BOOLEAN)){
        Description();
        if(c_type == LEX_SEMICOLON){ gl (); }
        else throw curr_lex; 
    }
}

void Parser::Description(){
    Type();
    Variable();
    while(c_type == LEX_COMMA){
        gl();
        Variable();
    }
}

void Parser::Type(){
    if((c_type == LEX_INT) || (c_type == LEX_STRING) || (c_type == LEX_BOOLEAN))
        gl();
    else throw curr_lex; 
}

void Parser::Variable(){
    Identifier();
    if(c_type == LEX_EQ){
        gl();
        Constant();
    };
}

void Parser::Identifier(){
    if(c_type == LEX_ID) gl();
    else throw curr_lex; 
}

void Parser::Constant(){
    if(c_type == LEX_STR) StringConst();
    else if((c_type == LEX_TRUE) || (c_type == LEX_FALSE)) BoolConst();
    else Integer();
}

void Parser::Integer(){
    if((c_type == LEX_PLUS) || (c_type == LEX_MINUS)) Sign();
    if(c_type == LEX_NUM) gl();
    else throw curr_lex;
}

void Parser::Sign(){
    if((c_type == LEX_PLUS) || (c_type == LEX_MINUS)) gl();
    else throw curr_lex;
}

void Parser::StringConst(){
    if(c_type == LEX_STR) gl();
    else throw curr_lex;
}

void Parser::BoolConst(){
    if((c_type == LEX_TRUE) || (c_type == LEX_FALSE)) gl();
    else throw curr_lex;
}

void Parser::Operators(){
    while(c_type != LEX_END) Operator();
}

void Parser::Operator(){
    flag = true;
    if(c_type == LEX_IF){
        gl();
        if(c_type == LEX_LPAREN) gl();
        else throw curr_lex;
        Expression();
        if(c_type == LEX_RPAREN) gl();
        else throw curr_lex;
        Operator();
        if(c_type == LEX_ELSE){
            gl();
            Operator();
        }
    }
    else if(c_type == LEX_WHILE){
        cout << "NICE" << endl;
        gl();
        if(c_type == LEX_LPAREN) gl();
        else throw curr_lex;
        Expression();
        if(c_type == LEX_RPAREN) gl();
        else throw curr_lex;
        Operator();
    }
    else if(c_type == LEX_READ){
        gl();
        if(c_type == LEX_LPAREN) gl();
        else throw curr_lex;
        Identifier();
        if(c_type == LEX_RPAREN) gl();
        else throw curr_lex;
        if(c_type == LEX_SEMICOLON) gl();
        else throw curr_lex;
    }
    else if(c_type == LEX_WRITE){
        gl();
        if(c_type == LEX_LPAREN) gl();
        else throw curr_lex;
        Expression();
        while(c_type == LEX_COMMA){
            gl();
            Expression();
        };
        if(c_type == LEX_RPAREN) gl();
        else throw curr_lex;
        if(c_type == LEX_SEMICOLON) gl();
        else throw curr_lex;
    }
    else if(c_type == LEX_BEGIN) CompoundOp();
    else if(c_type == LEX_GOTO){
        gl();
        Identifier();
        if(c_type == LEX_SEMICOLON) gl();
        else throw curr_lex;
    }
    else ExpressionOp();
}

void Parser::CompoundOp(){
    if(c_type == LEX_BEGIN) gl();
    else throw curr_lex;
    Operators();
    if(c_type == LEX_END) gl();
    else throw curr_lex;
}

void Parser::ExpressionOp(){
    flagLbl = false;
    Expression();
    if(!flagLbl){
        if(c_type == LEX_SEMICOLON) gl(); 
        else throw curr_lex;
    }
    else LabelOp();
}

void Parser::LabelOp(){
    if(c_type == LEX_COLON) gl();
    else throw curr_lex;
    Operator();
}

void Parser::Expression(){
    E();
    while(c_type == LEX_EQ){
        //st_lex.push ( c_type );
        gl(); 
        E(); 
        //check_op ();
    }
}

void Parser::E(){
    A();
    while(c_type == LEX_OR){
        //st_lex.push ( c_type );
        gl(); 
        A(); 
        //check_op ();
    }
}

void Parser::A(){
    C();
    while((c_type == LEX_AND)){
        //st_lex.push ( c_type );
        gl(); 
        C(); 
        //check_op ();
    }
}

void Parser::C(){
    PM();
    while((LEX_LSS <= c_type) || (c_type >= LEX_DEQ)){
        //st_lex.push ( c_type );
        gl(); 
        PM(); 
        //check_op ();
    }
}

void Parser::PM(){
    TS();
    while((c_type == LEX_PLUS) || (c_type == LEX_MINUS)){
        //st_lex.push ( c_type );
        gl(); 
        TS(); 
        //check_op ();
    }
}

void Parser::TS(){
    F();
    while((c_type == LEX_TIMES) || (c_type == LEX_SLASH)){
        //st_lex.push ( c_type );
        gl(); 
        F(); 
        //check_op ();
    }
}

void Parser::F () {
    //cout << "F();" << endl;
    if(c_type == LEX_NOT){
        gl();
        F();
    }
    else if((c_type == LEX_PLUS) || (c_type == LEX_MINUS)){
        gl();
        F();
    } 
    else if(c_type == LEX_ID){
        //check_id ();
        //poliz.push_back ( Lex ( LEX_ID, c_val ) );
        gl();
        if(flag){
            flag = false;
            if(c_type == LEX_COLON) flagLbl = true;
        }
    }
    else if(c_type == LEX_NUM){
        //st_lex.push ( LEX_INT );
        //poliz.push_back ( curr_lex );
        gl ();
    }
    else if(c_type == LEX_STR){
        //st_lex.push ( LEX_INT );
        //poliz.push_back ( curr_lex );
        gl ();
    }
    else if(c_type == LEX_TRUE){
        //st_lex.push ( LEX_BOOL );
        //poliz.push_back ( Lex (LEX_TRUE, 1) );
        gl ();
    }
    else if(c_type == LEX_FALSE){
        //st_lex.push ( LEX_BOOL );
        //poliz.push_back ( Lex (LEX_FALSE, 0) );
        gl ();
    }
    else if(c_type == LEX_LPAREN){
        gl(); 
        Expression();
        if(c_type == LEX_RPAREN)
            gl ();
        else 
            throw curr_lex;
    }
    else throw curr_lex;
}

int main(int argc, char *argv[]){
    if(argc < 2) cout << "FILE?" << endl;
    else{
        try{
            /*Lex L;
            Scanner F(argv[1]);
            while( (L = F.get_lex()).get_type() != LEX_FIN) cout << L;
            cout << "TID:" << endl;
            vector<Ident>::const_iterator i;
            for(i = TID.begin(); i != TID.end(); i++) cout << (*i).get_name() << endl;*/
            Parser P(argv[1]);
            P.analyze();
        }
        catch(const Lex L){
            cout << "error: " << L;
        }
        catch(const char c){
            cout << "error: " << c;
        }
    }
};