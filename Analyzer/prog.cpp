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
// Parser //

void from_st(stack<type_of_lex> & st, type_of_lex & i ) {
    i = st.top(); st.pop();
}

class Parser{
    Lex curr_lex;
    type_of_lex c_type;
    int c_val;
    string c_str;
    Scanner scan;
    bool flag, flagLbl;
    stack<type_of_lex> st_lex;
    vector<int> Lbl_vector;
    vector<int> goto_vector;
    void Program();
    void Descriptions();
    void Description();
    void Type();
    void Variable(type_of_lex t_lex);
    void Constant(type_of_lex _t_lex);
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
    void check_id();
    void check_op ();
    void check_not();
    void check_unary(); 
    void eq_type();
    void eq_boolean();
    void check_id_in_read();
    void check_Lbl();
    void  gl(){
        curr_lex  = scan.get_lex ();
        c_type = curr_lex.get_type ();
        c_val = curr_lex.get_value_int();
        c_str = curr_lex.get_value_string();
        //cout << curr_lex;
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
    check_Lbl();
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
    type_of_lex tp = curr_lex.get_type();
    Type();
    Variable(tp);
    while(c_type == LEX_COMMA){
        gl();
        Variable(tp);
    }
}

void Parser::Type(){
    if((c_type == LEX_INT) || (c_type == LEX_STRING) || (c_type == LEX_BOOLEAN))
        gl();
    else throw curr_lex; 
}

void Parser::Variable(type_of_lex t_lex){
    if(c_type == LEX_ID){
        if(TID[curr_lex.get_value_int()].get_declare()) throw "twice";
        else{
            TID[curr_lex.get_value_int()].put_declare();
            TID[curr_lex.get_value_int()].put_type(t_lex);
        } 
        gl();
    }
    else throw curr_lex;
    if(c_type == LEX_EQ){
        gl();
        Constant(t_lex);
    };
}

void Parser::Constant(type_of_lex _t_lex){
    if(c_type == LEX_STR && _t_lex == LEX_STRING) StringConst();
    else if(((c_type == LEX_TRUE) || (c_type == LEX_FALSE)) && (_t_lex == LEX_BOOLEAN)) BoolConst();
    else if(_t_lex == LEX_INT) Integer();
    else throw "wrong types are in initialization";
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
        eq_boolean();
        st_lex.pop();
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
        eq_boolean();
        st_lex.pop();
        if(c_type == LEX_RPAREN) gl();
        else throw curr_lex;
        Operator();
    }
    else if(c_type == LEX_READ){
        gl();
        if(c_type == LEX_LPAREN) gl();
        else throw curr_lex;
        if(c_type == LEX_ID){
            check_id_in_read();
            gl();
        }
        else throw curr_lex;
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
        if(c_type == LEX_ID){
            goto_vector.push_back(c_val);
            gl();
        }
        else throw curr_lex;
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
        if(c_type == LEX_SEMICOLON){ 
            gl(); 
            st_lex.pop();
        }
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
        st_lex.push(c_type);
        gl(); 
        E(); 
        eq_type();
    }
}

void Parser::E(){
    A();
    while(c_type == LEX_OR){
        st_lex.push(c_type);
        gl(); 
        A(); 
        check_op();
    }
}

void Parser::A(){
    C();
    while((c_type == LEX_AND)){
        st_lex.push(c_type);
        gl(); 
        C(); 
        check_op();
    }
}

void Parser::C(){
    PM();
    while((LEX_LSS <= c_type) || (c_type >= LEX_DEQ)){
        st_lex.push(c_type);
        gl(); 
        PM(); 
        check_op();
    }
}

void Parser::PM(){
    TS();
    while((c_type == LEX_PLUS) || (c_type == LEX_MINUS)){
        st_lex.push(c_type);
        gl(); 
        TS(); 
        check_op();
    }
}

void Parser::TS(){
    F();
    while((c_type == LEX_TIMES) || (c_type == LEX_SLASH)){
        st_lex.push ( c_type );
        gl(); 
        F(); 
        check_op();
    }
}

void Parser::F(){ 
    if(c_type == LEX_NOT){
        gl();
        F();
        eq_boolean();
    }
    else if((c_type == LEX_PLUS) || (c_type == LEX_MINUS)){
        gl();
        F();
        check_unary();
    } 
    else if(c_type == LEX_ID){
        if(TID[c_val].get_declare()){
            st_lex.push(TID[c_val].get_type());
            //poliz.push_back ( Lex ( LEX_ID, c_val ) );
            gl();
        }
        else{
            Lbl_vector.push_back(c_val);
            gl();
            if(!flag || c_type != LEX_COLON) throw "not declared";
            flagLbl = true;
        }        
    }
    else if(c_type == LEX_NUM){
        st_lex.push(LEX_INT);
        //poliz.push_back ( curr_lex );
        gl ();
    }
    else if(c_type == LEX_STR){
        st_lex.push(LEX_STRING);
        //poliz.push_back ( curr_lex );
        gl ();
    }
    else if(c_type == LEX_TRUE){
        st_lex.push(LEX_BOOLEAN);
        //poliz.push_back ( Lex (LEX_TRUE, 1) );
        gl ();
    }
    else if(c_type == LEX_FALSE){
        st_lex.push(LEX_BOOLEAN);
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
    flag = false; 
}

void Parser::check_id(){
    if(TID[c_val].get_declare())
        st_lex.push(TID[c_val].get_type());
    else 
        throw "not declared";
}

void Parser::check_op(){
    type_of_lex t1, t2, op, r = LEX_NULL;
    from_st(st_lex, t2);
    from_st(st_lex, op);
    from_st(st_lex, t1);
    if(t1 == LEX_INT && t2 == LEX_INT){
        if(op == LEX_PLUS || op == LEX_MINUS || op == LEX_TIMES || op == LEX_SLASH)
            r = LEX_INT;
        if(LEX_LSS <= op || op <= LEX_EQ)
            r = LEX_BOOLEAN;
    }
    if(t1 == LEX_BOOLEAN && t2 == LEX_BOOLEAN){
        if (op == LEX_OR || op == LEX_AND)
            r = LEX_BOOLEAN;
    }
    if(t1 == LEX_STRING && t2 == LEX_STRING){
        if(op == LEX_PLUS)
            r = LEX_STRING;
        if(LEX_LSS <= op || op <= LEX_EQ)
            r = LEX_BOOLEAN;
    }
    if(r == LEX_NULL) throw "wrong types are in operation";
    cout << t1 << ' ' << t2 << ' ' << r << endl;
    st_lex.push(r);
    //poliz.push_back (Lex (op) );
}

void Parser::check_not(){
    if(st_lex.top() != LEX_BOOLEAN)
        throw "wrong type is in not";
   /* else  
        poliz.push_back ( Lex (LEX_NOT) );*/
}

void Parser::check_unary(){
    if(st_lex.top() != LEX_INT)
        throw "wrong type is in unary";
    //plus or minus?  - пометка на будущее
}

void Parser::eq_type(){
    type_of_lex t;
    from_st(st_lex, t);
    st_lex.pop(); // del =
    if(t != st_lex.top())
        throw "wrong types are in =";
}

void Parser::eq_boolean(){
    if(st_lex.top() != LEX_BOOLEAN)
        throw "expression is not boolean";
}

void Parser::check_id_in_read(){
    if(!TID[c_val].get_declare())
        throw "not declared";
    if(TID[c_val].get_type() == LEX_BOOLEAN)
        throw "Boolean in read";
}

void Parser::check_Lbl(){
    for(int i : Lbl_vector){
        cout << "LABEL:" << endl;
        cout << TID[i].get_name() << endl;
        if(TID[i].get_declare())
            throw "twice";
        else TID[i].put_declare();
    };
    vector<int>::iterator it;
    for(int j : goto_vector){
        cout << "GOTO:" << endl;
        cout << TID[j].get_name() << endl;
        it = find(Lbl_vector.begin(), Lbl_vector.end(), j);
        if(it == Lbl_vector.end())
            throw "not declared";
    }
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
        catch(const char * str){
            cout << str << endl;
        }
    }
};