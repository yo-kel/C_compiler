//
// Created by xsj on 2021/11/4.
//

#ifndef C_COMPILER_PARSER_H
#define C_COMPILER_PARSER_H

#include <string>
#include <utility>
#include <iostream>
#include "lexer.h"
#include <typeinfo>

using namespace std;
using namespace lexer;

namespace parser {
//TODO 支持数组
//TODO 变量初始化

    class ASTNode {
    public:
        void outPut(){
            cout<<"?"<<endl;
        }
    };


    class TypeSpec : public ASTNode {
    public:
        int type;

        TypeSpec(int type) {
            this->type = type;
        }

        void outPut()  {
            cout << typeid(this).name() << " type:" << this->type << endl;
        }
    };


    class TypeVar : public ASTNode {
    public:
        TypeSpec *typeSpec;
        string ID;

        TypeVar(TypeSpec *typeSpec1, string &ID1) {
            typeSpec = typeSpec1;
            ID = ID1;
        }

        void outPut()  {
            cout << typeid(this).name() << " ID:" << this->ID << endl;
            cout << "typeSpec:" << endl;
            typeSpec->outPut();
        }
    };

    class DeclarationBody : public ASTNode { //virtual

    };

    class Declaration : public ASTNode {
    public:
        TypeVar* typeVar;
        DeclarationBody *body;
        Declaration (TypeVar* typeVar,DeclarationBody *body){
            this->typeVar=typeVar;
            this->body=body;
        }
        void outPut()  {
            cout << typeid(this).name() << " nothing:" << "" << endl;
            cout << "typeVar:" << endl;
            typeVar->outPut();
            cout << "body:" << endl;
            body->outPut();
        }
    };

    class VariableDecl : public DeclarationBody {
        void outPut()  {
            cout << typeid(this).name() << " nothing:" << "" << endl;
        }

    };

    class Program: public ASTNode{
    public:
        vector<Declaration*>declarationList;
        Program(vector<Declaration*>&declarationList){
            this->declarationList=declarationList;
        }
        void outPut()  {
            cout << typeid(this).name() << " nothing:" << "" << endl;
            cout<<"declaration:"<<endl;
            for(int i=0;i<declarationList.size();i++){
                declarationList[i]->outPut();
            }
        }

    };

    class Params {
    public:
        vector<TypeVar> params;

        Params(vector<TypeVar> &params1) {
            params = params1;
        }
    };


    class LocalDecl {
    public:
        vector<TypeVar> declList;

        LocalDecl(vector<TypeVar> &declList1) {
            declList = declList1;
        }

    };

    class Statement {
    };

    class BlockStmt : public Statement {
    public:
        vector<LocalDecl *> localDeclList;
        vector<Statement *> statementList;

        BlockStmt(vector<LocalDecl *> &localDeclList1, vector<Statement *> &statementList1) {
            localDeclList = localDeclList1;
            statementList = statementList1;
        }
    };

    class Expr {
    public:
        Expr *exp{};
        struct OpeExpr {
            Expr *expr;
            Token *ope;
        };
        vector<OpeExpr> exprList;

        Expr(Expr *exp, vector<OpeExpr> &exprList) {
            this->exp = exp;
            this->exprList = exprList;
        }

        Expr() = default;
    };

    class Assign : public Expr {
    public:
        Assign(Expr *exp, vector<OpeExpr> &exprList) : Expr(exp, exprList) {}
    };

    class Equal : public Expr {
    public:
        Equal(Expr *exp, vector<OpeExpr> &exprList) : Expr(exp, exprList) {}
    };

    class Comp : public Expr {
    public:
        Comp(Expr *exp, vector<OpeExpr> &exprList) : Expr(exp, exprList) {}
    };

    class Term : public Expr {
    public:
        Term(Expr *exp, vector<OpeExpr> &exprList) : Expr(exp, exprList) {}
    };

    class Factor : public Expr {
    public:
        Factor(Expr *exp, vector<OpeExpr> &exprList) : Expr(exp, exprList) {}
    };

    class Unary : public Expr {
    public:
        Unary(Expr *exp, vector<OpeExpr> &exprList) : Expr(exp, exprList) {}
    };

    class Postfix : public Expr {
    public:
        Postfix(Expr *exp, vector<OpeExpr> &exprList) : Expr(exp, exprList) {}
    };

    class Item : public Expr { // virtual
    public:
        Item(Expr *exp, vector<OpeExpr> &exprList) : Expr(exp, exprList) {}
    };


    class Var : public Expr {
    public:
        Token *ID;
        vector<Expr *> arrayNum;

        Var(Token *ID, vector<Expr *> &arrayNum) : Expr() {
            this->ID = ID;
            this->arrayNum = arrayNum;
        }
    };

    class ConstValue : public Expr {
    public:
        ConstValue() : Expr() {};
    };

    class Int : public ConstValue {
    public:
        int value;

        Int(int value) : ConstValue() {
            this->value = value;
        }
    };

    class Float : public ConstValue {
    public:
        float value;

        Float(float value) : ConstValue() {
            this->value = value;
        }
    };

    class Char : public ConstValue {
    public:
        char value;

        Char(char value) : ConstValue() {
            this->value = value;
        }
    };

    class String : public ConstValue {
    public:
        string value;

        String(string &value) : ConstValue() {
            this->value = value;
        }
    };

    class Call : public Expr {
    public:
        Token *ID;
        vector<Expr *> args;

        Call(Token *ID, vector<Expr *> &args) : Expr() {
            this->ID = ID;
            this->args = args;
        }
    };


    class FuncDecl : public DeclarationBody {
    public:
        Params *params;
        BlockStmt *blockStmt;

        FuncDecl(Params *params1, BlockStmt *blockStmt1) {
            params = params1;
            blockStmt = blockStmt1;
        }

    };


    class PrintStmt : public Statement {
    };

    class ExprStmt : public Statement {
    public:
        Expr *expr;

        ExprStmt(Expr *expr1) {
            expr = expr1;
        }
    };

    class IfStmt : public Statement {
    public:
        Expr *expr;
        Statement *statement1, *statement2;

        IfStmt(Expr *expr, Statement *statement1, Statement *statement2) {
            this->expr = expr;
            this->statement1 = statement1;
            this->statement2 = statement2;
        }
    };

    class ForStmt : public Statement {
    };

    class WhileStmt : public Statement {
    public:
        Expr *expr;
        Statement *statement;

        WhileStmt(Expr *expr1, Statement *statement1) {
            expr = expr1;
            statement = statement1;
        }
    };

    class AssertStmt : public Statement {
    };

    class ReturnStmt : public Statement {
    public:
        Expr *expr;

        ReturnStmt(Expr *expr1) {
            expr = expr1;
        }
    };

    Program *program(); // {Declaration}+

    Declaration *decl(); // TypeVar {VariableDecl | FuncDecl}

    TypeVar *typeVar(); //  TypeSpec {*}? ID

    VariableDecl *variableDecl(); // {[NUMBER]}* ;

    TypeSpec *typeSpec(); // INT | FLOAT | CHAR | VOID

    FuncDecl *funcDecl(); //  ( Params ) BlockStmt

    Params *params(); //  VOID | TypeVar {,TypeVar}*

    LocalDecl *localDecl(); // {TypeVar}* ;

    Statement *statement(); // ExprStmt | BlockStmt | IfStmt | ForStmt | WhileStmt | AssertStmt | ReturnStmt

    ExprStmt *exprStmt(); // Expr ; | ;

    BlockStmt *blockStmt(); // { {LocalDecl}* {Statement}* }

    IfStmt *ifStmt(); // if ( Expr ) Statement | if ( Expr ) Statement else Statement

    ForStmt *forStmt();

    WhileStmt *whileStmt(); // while ( Expr ) Statement

    ReturnStmt *returnStmt(); // return Expr ; | return ;

    Expr *expr(); // Equality{ = Equality}* | Equality    actually should be called Assign

    Expr *equality(); // COMP {( == | != ) COMP}* |COMP

    Expr *comp(); // TERM {( > | >= | < | <= | && | || | | | & ) TERM }*

    Expr *term(); // FACTOR (( + | - ) FACTOR)* | FACTOR

    Expr *factor(); // UNARY (( / | * ) UNARY)*

    Expr *unary(); // ( ! | - | -- | ++ )* POSTFIX

    Expr *postfix(); // Item ( ++ | -- )*

    Expr *item(); // ( Expr ) | ID {var | call } | Int | Float | Char | String

    Expr *var(); //  {[expr]*} TODO array

    Expr *call(); // ( {Args} )



    void init(vector<lexer::Token> *tokenListPtr);
}


#endif //C_COMPILER_PARSER_H
