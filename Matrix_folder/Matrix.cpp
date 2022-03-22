#include <cstdio>
#include <iostream>
#include "mod_Matrix.h"
using namespace std;

class Matrix{
    double **M;
    int m, n;
    public:
        Matrix(int _m = 1, int _n = 1){
            if(! (_n && _m)) _m = _n = 1;
            m = _m; n = _n;
            M = new double * [m];
            for(int i = 0; i < m; i++)
                M[i] = new double [n];
            for(int i = 0; i < m; i++)
                for(int j = 0; j < n; j++)
                    M[i][j] = 0;
        }

        Matrix(const Matrix & A){
            m = A.m;  n = A.n; 
            M = new double * [m];
            for(int i = 0; i < m; i++)
                M[i] = new double [n];
            for(int i = 0; i < m; i++)
                for(int j = 0; j < n; j++)
                    M[i][j] = A.M[i][j];
        }

        double Det() const;
        double Norm() const;
        Matrix & Tr();

        friend Matrix inverse_matrix(const Matrix & A);
        friend Matrix operator*(const Matrix & A, const Matrix & B);
        friend Matrix operator*(const Matrix & A, int q);
        friend Matrix operator/(const Matrix & A, const Matrix & B);
        friend bool operator==(const Matrix & A, const Matrix & B);
        friend bool operator!=(const Matrix & A, const Matrix & B);
       
        bool low_trian() const{
            for(int i = 0; i < m; i++)
                for(int j = 0; (j < i) && (i < n) ; j++)
                    if(M[i][j] != 0) return false;
            return true;
        };

        bool up_trian() const{
            for(int i = 0; i < m; i++)
                for(int j = 0; (j > i) && (j < n) ; j++)
                    if(M[i][j] != 0) return false;
            return true;
        };

        Matrix & operator=(const Matrix & A){
            if (this != &A){
                m = A.m; n = A.n;
                for(int i = 0; i < m; i++)
                    delete [] M[i];
                delete [] M;
                M = new double * [m];
            for(int i = 0; i < m; i++)
                M[i] = new double [n];
            for(int i = 0; i < m; i++)
                for(int j = 0; j < n; j++)
                    M[i][j] = A.M[i][j];
            }
            return *this;
        };

        void operator+=(const Matrix & A){
            for(int i = 0; i < m; i++)
                for(int j = 0; j < n; j++)
                    M[i][j] += A.M[i][j];
        };

        void operator-=(const Matrix & A){
            for(int i = 0; i < m; i++)
                for(int j = 0; j < n; j++)
                    M[i][j] -= A.M[i][j];
        };
        
        void operator*=(const Matrix & B){
            Matrix A = *this;
            *this = A * B;
        };

        void fill(){
            for(int i = 0; i < m; i++)
                for(int j = 0; j < n; j++){
                    printf("M[%d][%d] = ", i + 1, j + 1);
                    scanf("%lf",  &(M[i][j]));
                }
        }

        void print() const{
            cout << "MATRIX"<< endl;
            printf("m = %d; n = %d\n", m, n);
            for(int i = 0; i < m; i++){
                for(int j = 0; j < n; j++)
                    printf("%10.5f ",  M[i][j]);
                cout << endl;
            }
        }

        ~Matrix(){
            for(int i = 0; i < m; i++)
                delete [] M[i];
            delete [] M;
        };
};

double Matrix::Det() const{
    if(m != n){
        cout << "The Matrix isn't square" << endl;
        return 0;
    }
    return findDet(M, n);
};

double Matrix::Norm() const{
    int _norm = 0;
    for(int i = 0; i < m; i++)
            for(int j = 0; j < n; j++)
                _norm += M[i][j] * M[i][j];
    return sqrt(_norm);
};

Matrix & Matrix::Tr(){
    double ** T;
    T = new double * [n];
        for(int i = 0; i < m; i++)
            T[i] = new double [m];
        for(int i = 0; i < m; i++)
            for(int j = 0; j < n; j++)
                T[j][i] = M[i][j];
        for(int i = 0; i < m; i++)
            delete [] M[i];
        delete [] M;
        M = T; 
        int t; t = n; n = m; m = t;
    return *this;
};

bool operator==(const Matrix & A, const Matrix & B){
    if( (A.m != B.m) || (A.n != B.n) ) return false;
    else{  
        for(int i = 0; (i < A.m); i++)
            for(int j = 0; (j < A.n); j++)
                if(A.M[i][j] != B.M[i][j]) return false;
    };
    return true;
};

bool operator!=(const Matrix & A, const Matrix & B){
    if( (A.m != B.m) || (A.n != B.n) ) return true;
    else{  
        for(int i = 0; (i < A.m); i++)
            for(int j = 0; (j < A.n); j++)
                if(A.M[i][j] != B.M[i][j]) return true;
    };
    return false;
};

Matrix inverse_matrix(const Matrix & A){
    Matrix A_invrs = A;
    if( (A.m != A.n) || (A.Det() == 0)){
        cout << "The matrix doesn't fit" << endl;
    }
    else inversion(A_invrs.M, A.m);
    return A_invrs;
};

Matrix operator*(const Matrix & A, const Matrix & B){
    Matrix C(A.m, B.n);
    if(A.n == B.m){
        for(int i = 0; i < C.m; i++)
            for(int j = 0; j < C.n; j++)
                for(int k = 0; k < A.n; k++)
                    C.M[i][j] += A.M[i][k] * B.M[k][j];
    }
    else cout << "Incorrect matrix sizes" << endl;
    return C;
};

Matrix operator*(const Matrix & A, int q){
    Matrix C = A;
    for(int i = 0; i < C.m; i++)
        for(int j = 0; j < C.n; j++)
            C.M[i][j] *= q;
    return C;
};

Matrix Create_Matrix(){
    int m, n;
    cout << "m, n = ";
    cin >> m >> n;
    Matrix tmp(m, n);
    tmp.fill();
    return tmp;
};

int main()
{    
    bool marker = 1; 
    for(;marker;){
        cout << "1. Determinant" << endl;
        cout << "2. Norm" << endl;
        cout << "3. Transposition" << endl;
        cout << "4. Inverse" << endl;
        cout << "5. +=" << endl;
        cout << "6. -=" << endl;
        cout << "7. *" << endl;
        cout << "8. ==" << endl;
        cout << "9. low_trian" << endl;
        cout << "10. up_trian" << endl;
        cout << "11. exit" << endl;
        int t;
        cin >> t;
        Matrix A;
        switch (t)
        {
            case 1:
                {
                Matrix A = Create_Matrix();
                cout << A.Det() << endl;
                };
                break;
            case 2:
                {
                Matrix A = Create_Matrix();
                cout << A.Norm() << endl;
                };
                break;
            case 3:
                {
                Matrix A = Create_Matrix();
                A.Tr().print();
                };
                break;
            case 4:
                {   
                Matrix A = Create_Matrix();
                inverse_matrix(A).print();
                };       
                break;
            case 5:
                {
                Matrix A = Create_Matrix();
                Matrix B = Create_Matrix();
                A += B;
                A.print();
                };
                break;
            case 6:
                {
                Matrix A = Create_Matrix();
                Matrix B = Create_Matrix();
                A -= B;
                A.print();
                };
                break;
            case 7:
                {
                Matrix A = Create_Matrix();
                Matrix B = Create_Matrix();
                (A * B).print();
                };
                break;
            case 8:
                {
                Matrix A = Create_Matrix();
                Matrix B = Create_Matrix();
                cout << (A == B) << endl;;
                };
                break;
            case 9:
                {
                Matrix A = Create_Matrix();
                cout << A.low_trian() << endl;
                };
                break;
            case 10:
                {
                Matrix A = Create_Matrix();
                cout << A.up_trian() << endl;
                };
                break;            
            default:
                marker = 0;
                break;
        };

    };
    //cout << h.low_trian() << endl;
    //cout << m.up_trian() << endl;
    return 0;
};