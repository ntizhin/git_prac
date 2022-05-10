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
    POLIZ_FGO,                                                                                  /*44*/    
    LEX_UPLUS,                                                                                  /*45*/
    LEX_UMINUS                                                                                  /*46*/
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
    string get_value_string() const { return l_str; };
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

template <class T, class T_EL>
void from_st(stack<T> & st, T_EL & i ) {
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
    struct s_gt{
        int index;
        int ps;
    } _gt;
    vector<s_gt> goto_vector;
    int _lbl[2];
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
    }
public:
    vector<Lex> poliz;
    Parser(const char * program) : scan(program){};
    void analyze();
};

void Parser::analyze(){
    gl();
    Program();
    if(c_type != LEX_FIN) 
        throw curr_lex;
    check_Lbl();
    for(Lex l : poliz) 
        cout << l;
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
        TID[TID.size() - 1].put_assign();
    };
}

void Parser::Constant(type_of_lex _t_lex){
    if(c_type == LEX_STR && _t_lex == LEX_STRING) StringConst();
    else if(((c_type == LEX_TRUE) || (c_type == LEX_FALSE)) && (_t_lex == LEX_BOOLEAN)) BoolConst();
    else if(_t_lex == LEX_INT) Integer();
    else throw "wrong types are in initialization";
}

void Parser::Integer(){
    int sign = 1;
    if(c_type == LEX_PLUS) gl(); 
    else if(c_type == LEX_MINUS){ sign = -1; gl(); }
    if(c_type == LEX_NUM){ 
        TID[TID.size() - 1].put_value_int(c_val * sign);
        gl();
    }
    else throw curr_lex;
}

void Parser::StringConst(){
    if(c_type == LEX_STR){
        TID[TID.size() - 1].put_value_string(c_str);
        gl();
    }
    else throw curr_lex;
}

void Parser::BoolConst(){
    if(c_type == LEX_TRUE){
        TID[TID.size() - 1].put_value_int(1);
        gl();
    }
    else if(c_type == LEX_FALSE){
        TID[TID.size() - 1].put_value_int(0);
        gl();
    }
    else throw curr_lex;
}

void Parser::Operators(){
    while(c_type != LEX_END) Operator();
}

void Parser::Operator(){
    flag = true;
    int pl0, pl1, pl2, pl3;
    if(c_type == LEX_IF){
        gl();
        if(c_type == LEX_LPAREN) gl();
        else throw curr_lex;
        Expression();
        eq_boolean();
        pl0 = poliz.size();
        poliz.push_back(Lex());
        poliz.push_back(Lex(POLIZ_FGO));
        st_lex.pop();
        if(c_type == LEX_RPAREN) gl();
        else throw curr_lex;
        Operator();
        if(c_type == LEX_ELSE){
            pl1 = poliz.size();
            poliz.push_back(Lex());
            poliz.push_back(Lex(POLIZ_GO));
            poliz[pl0] = Lex(POLIZ_LABEL, poliz.size());
            gl();
            Operator();
            poliz[pl1] = Lex(POLIZ_LABEL, poliz.size());
        }
        else poliz[pl0] = Lex(POLIZ_LABEL, poliz.size());
    }
    else if(c_type == LEX_WHILE){
        gl();
        if(c_type == LEX_LPAREN) gl();
        else throw curr_lex;
        pl3 = poliz.size();
        Expression();
        eq_boolean();
        st_lex.pop();
        pl2 = poliz.size();
        poliz.push_back(Lex());
        poliz.push_back(Lex(POLIZ_FGO));
        if(c_type == LEX_RPAREN) gl();
        else throw curr_lex;
        Operator();
        poliz.push_back(Lex(POLIZ_LABEL, pl3));
        poliz.push_back(Lex(POLIZ_GO));
        poliz[pl2] = Lex(POLIZ_LABEL, poliz.size());
    }
    else if(c_type == LEX_READ){
        gl();
        if(c_type == LEX_LPAREN) gl();
        else throw curr_lex;
        if(c_type == LEX_ID){
            check_id_in_read();
            poliz.push_back(Lex(POLIZ_ADDRESS, c_val));
            gl();
        }
        else throw curr_lex;
        if(c_type == LEX_RPAREN) gl();
        else throw curr_lex;
        poliz.push_back(Lex(LEX_READ));
        if(c_type == LEX_SEMICOLON) gl();
        else throw curr_lex;
    }
    else if(c_type == LEX_WRITE){
        gl();
        if(c_type == LEX_LPAREN) gl();
        else throw curr_lex;
        Expression();
        poliz.push_back(Lex(LEX_WRITE));
        while(c_type == LEX_COMMA){
            gl();
            Expression();
            poliz.push_back(Lex(LEX_WRITE));
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
            _gt.index = c_val;
            _gt.ps = poliz.size();
            goto_vector.push_back(_gt);
            poliz.push_back(Lex());
            poliz.push_back(POLIZ_GO);
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
    if(c_type == LEX_COLON){
        gl();
        if(TID[_lbl[0]].get_declare())
            throw "twice";
        else{
            TID[_lbl[0]].put_declare();
            TID[_lbl[0]].put_value_int(_lbl[1]);
            Lbl_vector.push_back(_lbl[0]);
        }
    }
    else throw curr_lex;
    Operator();
}

void Parser::Expression(){
    int _id;
    E();
    if(c_type == LEX_EQ){
        st_lex.push(c_type);
        if(poliz.back().get_type() == LEX_ID) {
            _id = poliz.back().get_value_int();
            TID[_id].put_assign();
            poliz.pop_back();
            poliz.push_back(Lex(POLIZ_ADDRESS, _id));
        }
        else 
            throw "wrong types are in =";
        gl(); 
        Expression(); 
        eq_type();
        poliz.push_back(Lex(LEX_EQ));
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
    else if(c_type == LEX_PLUS){
        gl();
        F();
        check_unary();
    } 
    else if(c_type == LEX_MINUS){
        gl();
        F();
        check_unary();
        poliz.push_back(Lex(LEX_UMINUS));
    } 
    else if(c_type == LEX_ID){
        if(TID[c_val].get_declare()){
            st_lex.push(TID[c_val].get_type());
            poliz.push_back(curr_lex);
            gl();
        }
        else{
            _lbl[0] = c_val;
            _lbl[1] = poliz.size();
            gl();
            if(!flag || c_type != LEX_COLON) throw "not declared";
            flagLbl = true;
        }        
    }
    else if(c_type == LEX_NUM){
        st_lex.push(LEX_INT);
        poliz.push_back(curr_lex);
        gl ();
    }
    else if(c_type == LEX_STR){
        st_lex.push(LEX_STRING);
        poliz.push_back(curr_lex);
        gl ();
    }
    else if(c_type == LEX_TRUE){
        st_lex.push(LEX_BOOLEAN);
        poliz.push_back(Lex(LEX_TRUE, 1));
        gl();
    }
    else if(c_type == LEX_FALSE){
        st_lex.push(LEX_BOOLEAN);
        poliz.push_back(Lex(LEX_FALSE, 0));
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
    st_lex.push(r);
    poliz.push_back(Lex(op));
}

void Parser::check_not(){
    if(st_lex.top() != LEX_BOOLEAN)
        throw "wrong type is in not";
    else  
        poliz.push_back(Lex(LEX_NOT));
}

void Parser::check_unary(){
    if(st_lex.top() != LEX_INT)
        throw "wrong type is in unary";
}

void Parser::eq_type(){
    type_of_lex t;
    from_st(st_lex, t);
    st_lex.pop(); 
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
    int lbl_ps;
/*    cout << "LABEL:" << endl;
    for(int i : Lbl_vector){
        cout << TID[i].get_name() << endl;
    }; */
    vector<int>::iterator it;
//    cout << "GOTO:" << endl;
    for(s_gt j : goto_vector){
//        cout << TID[j.index].get_name() << endl;
        it = find(Lbl_vector.begin(), Lbl_vector.end(), j.index);
        if(it == Lbl_vector.end())
            throw "not declared";
        else{
            lbl_ps = TID[j.index].get_value_int();
            poliz[j.ps] = Lex(POLIZ_LABEL, lbl_ps);
        }
    }
}

/// Executer ///

class Executer {
public:
    void execute ( vector<Lex> & poliz );
};

void Executer::execute(vector<Lex> & poliz){
    Lex pc_el;
    stack<int> args_int;
    stack<string> args_str;
    int lst_stack; // 0 - int, 1 - string
    int i, j, index = 0, size = poliz.size();
    string i_str, j_str;
    while(index < size){
        pc_el = poliz[index];
        switch(pc_el.get_type()){
            case LEX_TRUE: case LEX_FALSE: case LEX_NUM: 
            case POLIZ_ADDRESS: case POLIZ_LABEL: 
                args_int.push(pc_el.get_value_int());
                lst_stack = 0;
                break;

            case LEX_STR:
                args_str.push(pc_el.get_value_string());
                lst_stack = 1;
                break;

            case LEX_ID:
                i = pc_el.get_value_int();
                if(TID[i].get_assign()){
                    if(TID[i].get_type() == LEX_STRING){
                        args_str.push(TID[i].get_value_string());
                        lst_stack = 1;
                    }
                    else{
                        args_int.push(TID[i].get_value_int());
                        lst_stack = 0;
                    }
                }
                else
                  throw "POLIZ: indefinite identifier";
                break;

            case LEX_NOT:
                from_st(args_int, i);
                args_int.push(!i);
                lst_stack = 0;
                break;

            case LEX_OR:
                from_st(args_int, i); 
                from_st(args_int, j);
                args_int.push(j || i);
                lst_stack = 0;
                break;
 
            case LEX_AND:
                from_st(args_int, i);
                from_st(args_int, j);
                args_int.push(j && i);
                lst_stack = 0;
                break;
 
            case POLIZ_GO:
                from_st(args_int, i);
                index = i - 1;
                break;
 
            case POLIZ_FGO:
                from_st(args_int, i);
                from_st(args_int, j);
                if(!j) index = i - 1;
                break;
 
            case LEX_WRITE:
                if(lst_stack){
                    from_st(args_str, i_str);
                    cout << i_str << endl;
                }
                else{
                    from_st(args_int, i);
                    cout << i << endl;
                }
                break;
 
            case LEX_READ:
                int k;
                from_st(args_int, i);
                if(TID[i].get_type () == LEX_INT){
                    cout << "Input int value for" << TID[i].get_name () << endl;
                    cin >> k;
                    TID[i].put_value_int(k);
                    TID[i].put_assign();
                }
                else if(TID[i].get_type() == LEX_BOOLEAN){
                    string s;
                    while (1) {
                        cout << "Input boolean value (true or false) for" << TID[i].get_name() << endl;
                        cin >> s;
                        if ( s != "true" && s != "false" ) {
                            cout << "Error in input:true/false" << endl;
                            continue;
                        }
                        k = ( s == "true" ) ? 1 : 0;
                        break;
                        TID[i].put_value_int(k);
                        TID[i].put_assign();
                    }
                }
                else{
                    string s;
                    cout << "Input string value for" << TID[i].get_name() << endl;
                    cin >> s;
                    TID[i].put_value_string(s);
                    TID[i].put_assign();
                }
                break;
 
            case LEX_PLUS:
                if(lst_stack){
                    from_st(args_str, i_str);
                    from_st(args_str, j_str);
                    args_str.push(j_str + i_str);
                }
                else{
                    from_st(args_int, i);
                    from_st(args_int, j);
                    args_int.push(i + j);
                }
                break;
 
            case LEX_TIMES:
                from_st(args_int, i);
                from_st(args_int, j);
                args_int.push(i * j);
                lst_stack = 0;
                break;
 
            case LEX_MINUS:
                from_st(args_int, i);
                from_st(args_int, j);
                args_int.push(j - i);
                lst_stack = 0;
                break;
 
            case LEX_SLASH:
                from_st(args_int, i);
                from_st(args_int, j);
                if(i){
                    args_int.push(j / i);
                    lst_stack = 0;
                    break;
                }
                else
                    throw "POLIZ:divide by zero";
 
            case LEX_DEQ:
                if(lst_stack){
                    from_st(args_str, i_str);
                    from_st(args_str, j_str);
                    args_int.push(i_str == j_str);
                    lst_stack = 0;
                }
                else{
                    from_st(args_int, i);
                    from_st(args_int, j);
                    args_int.push(i == j);
                }
                break;
 
            case LEX_LSS:
                if(lst_stack){
                    from_st(args_str, i_str);
                    from_st(args_str, j_str);
                    args_int.push(j_str < i_str);
                    lst_stack = 0;
                }
                else{
                    from_st(args_int, i);
                    from_st(args_int, j);
                    args_int.push(j < i);
                }
                break;
 
            case LEX_GTR:
                if(lst_stack){
                    from_st(args_str, i_str);
                    from_st(args_str, j_str);
                    args_int.push(j_str > i_str);
                    lst_stack = 0;
                }
                else{
                    from_st(args_int, i);
                    from_st(args_int, j);
                    args_int.push(j > i);
                }
                break;
 
            case LEX_LEQ:
                from_st(args_int, i);
                from_st(args_int, j);
                args_int.push(j <= i);
                lst_stack = 0;
                break;
 
            case LEX_GEQ:
                from_st(args_int, i);
                from_st(args_int, j);
                args_int.push(j >= i);
                lst_stack = 0;
                break;
 
            case LEX_NEQ:
                if(lst_stack){
                    from_st(args_str, i_str);
                    from_st(args_str, j_str);
                    args_int.push(j_str != i_str);
                    lst_stack = 0;
                }
                else{
                    from_st(args_int, i);
                    from_st(args_int, j);
                    args_int.push(j != i);
                    lst_stack = 0;
                }
                break;

            case LEX_UMINUS:
                from_st(args_int, i);
                args_int.push(-i);
                lst_stack = 0;
                break;

            case LEX_EQ:
                if(lst_stack){
                    from_st(args_str, i_str);
                    from_st(args_int, j);
                    TID[j].put_value_string(i_str);
                    TID[j].put_assign(); 
                    args_str.push(i_str);
                }
                else{
                    from_st(args_int, i);
                    from_st(args_int, j);
                    TID[j].put_value_int(i);
                    TID[j].put_assign(); 
                    args_int.push(i);
                }
                break;
            default:
                throw "POLIZ: unexpected elem";
        }
        ++index;
    };
    cout << "Finish of executing!!!" << endl;
}

class Interpretator{
    Parser pars;
    Executer E;
public:
    Interpretator(const char* program): pars(program){}
    void interpretation();
};
 
void Interpretator::interpretation(){
    pars.analyze();
    E.execute(pars.poliz);
}

int main(int argc, char *argv[]){
    if(argc < 2) cout << "FILE?" << endl;
    else{
        try{
            Interpretator P(argv[1]);
            P.interpretation();
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