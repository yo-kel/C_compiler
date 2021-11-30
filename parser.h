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

    class Program;

    class Declaration;

    class LocalDecl;

    class Params;

    class TypeVar;

    class VariableDecl;

    class TypeSpec;

    class ExprStmt;

    class BlockStmt;

    class IfStmt;

    class ForStmt;

    class WhileStmt;

    class ReturnStmt;

    class PrintStmt;

    class FuncDecl;

    class Statement;

    class JumpStmt;

    class ContinueStmt;

    class BreakStmt;

    class PrintStmt;

    class Expr;

    class Assign;

    class Equal;

    class Comp;

    class Term;

    class Factor;

    class Unary;

    class Postfix;

    class Char;

    class Int;

    class Float;

    class Var;

    class Call;

    class Visitor {
    public:
        virtual void visit(Program *d) = 0;

        virtual void visit(Declaration *d) = 0;

        virtual void visit(ExprStmt *st) = 0;

        virtual void visit(BlockStmt *st) = 0;

        virtual void visit(IfStmt *st) = 0;

        virtual void visit(WhileStmt *st) = 0;
        virtual void visit(BreakStmt *st) = 0;
        virtual void visit(ContinueStmt *st) = 0;

        virtual void visit(ReturnStmt *st) = 0;

        virtual void visit(PrintStmt *st) = 0;

        virtual void visit(LocalDecl *dl) = 0;


        virtual void visit(Assign *expr) = 0;

//        virtual void visit(Equal *expr) = 0;

        virtual void visit(Comp *expr) = 0;

        virtual void visit(Term *expr) = 0;

        virtual void visit(Factor *expr) = 0;

        virtual void visit(Unary *expr) = 0;

        virtual void visit(Postfix *expr) = 0;

        virtual void visit(Char *expr) = 0;

        virtual void visit(Int *expr) = 0;

        virtual void visit(Float *expr) = 0;

        virtual void visit(Call *expr) = 0;

        virtual void visit(Var *expr) = 0;

    };

//TODO 支持数组
//TODO 变量初始化
    Program *program(); // {Declaration}+

    Declaration *decl(); // TypeVar {VariableDecl | FuncDecl}

    TypeVar *typeVar(); //  TypeSpec {*}? ID

    VariableDecl *variableDecl(); // {[NUMBER]}* ;

    TypeSpec *typeSpec(); // INT | FLOAT | CHAR | VOID

    FuncDecl *funcDecl(); //  ( Params ) BlockStmt

    Params *params(); //  VOID | TypeVar {,TypeVar}*

    LocalDecl *localDecl(); // {TypeVar}* ;

    Statement *statement(); // ExprStmt | BlockStmt | IfStmt | WhileStmt | AssertStmt | ReturnStmt

    ExprStmt *exprStmt(); // Expr ; | ;

    BlockStmt *blockStmt(); // { LocalDecl {Statement}* }

    IfStmt *ifStmt(); // if ( Expr ) Statement | if ( Expr ) Statement else Statement

    ForStmt *forStmt();

    WhileStmt *whileStmt(); // while ( Expr ) Statement

    ReturnStmt *returnStmt(); // return Expr ; | return ;

    JumpStmt *jumpStmt(); // virtual

    ContinueStmt *continueStmt(); // continue;

    BreakStmt *breakStmt(); // break;

    PrintStmt *printStmt(); // print( Expr );    print in real c is function,but here we just define it as statement.

    Expr *expr(); // Equality{ = Equality}* | Equality    actually should be called Assign

//    Expr *equality(); // COMP {( == | != ) COMP}* |COMP

    Expr *comp(); // TERM {( == | != || > | >= | < | <= | && | || | | | & ) TERM }*

    Expr *term(); // FACTOR (( + | - ) FACTOR)* | FACTOR

    Expr *factor(); // UNARY (( / | * ) UNARY)*

    Expr *unary(); // ( ! | - | -- | ++ )* POSTFIX

    Expr *postfix(); // Item ( ++ | -- )*

    Expr *item(); // ( Expr ) | ID {var | call } | Int | Float | Char | String

    Expr *var(); //  {[expr]*} TODO array

    Expr *call(); // ( {Args} )



    void init(vector<lexer::Token> *tokenListPtr);


    class ASTNode {
    public:
        void outPut() {
            cout << "?" << endl;
        }

        virtual void accept(Visitor *v) = 0;
    };


    class TypeSpec : public ASTNode {
    public:
        int type;

        TypeSpec(int type) {
            this->type = type;
        }

        void outPut() {
            cout << typeid(this).name() << " type:" << this->type << endl;
        }

        void accept(Visitor *v) override {

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

        void outPut() {
            cout << typeid(this).name() << " ID:" << this->ID << endl;
            cout << "typeSpec:" << endl;
            typeSpec->outPut();
        }

        void accept(Visitor *v) override {

        }
    };

    class Declaration : public ASTNode {
    public:
        TypeVar *typeVar;

        Declaration(TypeVar *typeVar) {
            this->typeVar = typeVar;
        }

        void outPut() {
            cout << typeid(this).name() << " nothing:" << "" << endl;
            cout << "typeVar:" << endl;
            typeVar->outPut();
        }

        void accept(Visitor *v) {
            v->visit(this);
        }
    };

    class VariableDecl : public Declaration {
    public:

        VariableDecl(TypeVar *typeVar) : Declaration(typeVar) {
        }

        void outPut() {
            cout << typeid(this).name() << " nothing:" << "" << endl;
        }

    };

    class Program : public ASTNode {
    public:
        vector<Declaration *> declarationList;

        Program(vector<Declaration *> &declarationList) {
            this->declarationList = declarationList;
        }

        void outPut() {
            cout << typeid(this).name() << " nothing:" << "" << endl;
            cout << "declaration:" << endl;
            for (int i = 0; i < declarationList.size(); i++) {
                declarationList[i]->outPut();
            }
        }

        void accept(Visitor *v) {
            v->visit(this);
        }
    };

    class Params : public ASTNode {
    public:
        vector<TypeVar> params;

        Params(vector<TypeVar> &params1) {
            params = params1;
        }

        void accept(Visitor *v) override {

        }
    };


    class LocalDecl : public ASTNode {
    public:
        vector<TypeVar *> declList;

        LocalDecl(vector<TypeVar *> &declList1) {
            declList = declList1;
        }

        void accept(Visitor *v) override {
            v->visit(this);
        }

    };


    class Statement : public ASTNode {
    };

    class BlockStmt : public Statement {
    public:
        LocalDecl *localDeclList;
        vector<Statement *> statementList;

        BlockStmt(LocalDecl *localDeclList1, vector<Statement *> &statementList1) {
            localDeclList = localDeclList1;
            statementList = statementList1;
        }

        void accept(Visitor *v) {
            v->visit(this);
        }
    };

    class JumpStmt : public Statement {
        void accept(Visitor *v) override {

        }

    };

    class Expr;

    class ReturnStmt : public JumpStmt {
    public:
        Expr *expr;

        ReturnStmt(Expr *expr1) {
            expr = expr1;
        }

        void accept(Visitor *v) {
            v->visit(this);
        }
    };

    class ContinueStmt : public JumpStmt {
    public:
        ContinueStmt() {

        }

        void accept(Visitor *v) override {
            v->visit(this);
        }
    };

    class BreakStmt : public JumpStmt {
    public:
        BreakStmt() {}

        void accept(Visitor *v) override {
            v->visit(this);
        }
    };


    class Expr : public ASTNode {
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

        void accept(Visitor *v) override {
            v->visit(this);
        }
    };

//    class Equal : public Expr {
//    public:
//        Equal(Expr *exp, vector<OpeExpr> &exprList) : Expr(exp, exprList) {}
//
//        void accept(Visitor *v) override{
//            v->visit(this);
//        }
//    };

    class Comp : public Expr {
    public:
        Comp(Expr *exp, vector<OpeExpr> &exprList) : Expr(exp, exprList) {}

        void accept(Visitor *v)override {
            v->visit(this);
        }
    };

    class Term : public Expr {
    public:
        Term(Expr *exp, vector<OpeExpr> &exprList) : Expr(exp, exprList) {}

        void accept(Visitor *v)override {
            v->visit(this);
        }
    };

    class Factor : public Expr {
    public:
        Factor(Expr *exp, vector<OpeExpr> &exprList) : Expr(exp, exprList) {}

        void accept(Visitor *v)override {
            v->visit(this);
        }
    };

    class Unary : public Expr {
    public:
        Unary(Expr *exp, vector<OpeExpr> &exprList) : Expr(exp, exprList) {}

        void accept(Visitor *v)override {
            v->visit(this);
        }
    };

    class Postfix : public Expr {
    public:
        Postfix(Expr *exp, vector<OpeExpr> &exprList) : Expr(exp, exprList) {}

        void accept(Visitor *v)override {
            v->visit(this);
        }
    };

    class Item : public Expr { // virtual
    public:
        Item(Expr *exp, vector<OpeExpr> &exprList) : Expr(exp, exprList) {}

        void accept(Visitor *v) override {

        }
    };


    class Var : public Expr {
    public:
        Token *ID;
        vector<Expr *> arrayNum;

        Var(Token *ID, vector<Expr *> &arrayNum) : Expr() {
            this->ID = ID;
            this->arrayNum = arrayNum;
        }

        void accept(Visitor *v) {
            v->visit(this);
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

        void accept(Visitor *v)override {
            v->visit(this);
        }
    };

    class Float : public ConstValue {
    public:
        float value;

        Float(float value) : ConstValue() {
            this->value = value;
        }

        void accept(Visitor *v)override {
            v->visit(this);
        }
    };

    class Char : public ConstValue {
    public:
        char value;

        Char(char value) : ConstValue() {
            this->value = value;
        }

        void accept(Visitor *v)override {
            v->visit(this);
        }
    };

    class String : public ConstValue {
    public:
        string value;

        String(string &value) : ConstValue() {
            this->value = value;
        }

        void accept(Visitor *v) override {

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

        void accept(Visitor *v) override{
            v->visit(this);
        }
    };


    class FuncDecl : public Declaration {
    public:
        Params *params;
        BlockStmt *blockStmt;

        FuncDecl(Params *params1, BlockStmt *blockStmt1, TypeVar *typeVar) : Declaration(typeVar) {
            params = params1;
            blockStmt = blockStmt1;
        }

    };


    class PrintStmt : public Statement {
    public:
        Expr *expr;

        PrintStmt(Expr *expr) {
            this->expr = expr;
        }

        void accept(Visitor *v) override{
            v->visit(this);
        }
    };

    class ExprStmt : public Statement {
    public:
        Expr *expr;

        ExprStmt(Expr *expr1) {
            expr = expr1;
        }

        void accept(Visitor *v)override {
            v->visit(this);
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

        void accept(Visitor *v) {
            v->visit(this);
        }
    };

    class ForStmt : public Statement {
        void accept(Visitor *v) override {

        }
    };

    class WhileStmt : public Statement {
    public:
        Expr *expr;
        Statement *statement;

        WhileStmt(Expr *expr1, Statement *statement1) {
            expr = expr1;
            statement = statement1;
        }

        void accept(Visitor *v) {
            v->visit(this);
        }
    };

    class AssertStmt : public Statement {
        void accept(Visitor *v) override {

        }
    };

}


#endif //C_COMPILER_PARSER_H
