//
// Created by xsj on 2021/11/13.
//

#ifndef C_COMPILER_EVALUATOR_H
#define C_COMPILER_EVALUATOR_H


#include <map>
#include <vector>
#include "parser.h"
#include "lexer.h"
#include "error.h"

using namespace std;
using namespace lexer;
using namespace parser;


namespace environment {
    class Environment;

    class Value;

    class Value {
    public:
        virtual void Print() =0;

        virtual ~Value() {};
    };

    class CallValue : public Value {
    public:
        FuncDecl *funcDecl;

        explicit CallValue(FuncDecl *funcDecl) {
            this->funcDecl = funcDecl;
        }
        void Print()override{

        }
    };

    class BasicValue : public Value {
    public:
    };


    class VariableValue : public Value {
    public:
        BasicValue *basicValue;
        string var;

        explicit VariableValue(string &var) {
            this->var = var;
        }

        void Print() {
            basicValue->Print();
        }

        VariableValue(string &var, BasicValue *basicValue) {
            this->var = var;
            this->basicValue = basicValue;
        }
    };

    class IntValue : public BasicValue {
    public:
        int v;

        explicit IntValue(int v) {
            this->v = v;
        }

        void Print() {
            printf("%d", v);
        }
    };

    class FloatValue : public BasicValue {
    public:
        float v;

        void Print() {
            printf("%.6f", v);
        }

        explicit FloatValue(float v) {
            this->v = v;
        }
    };

    class CharValue : public BasicValue {
    public:
        char v;

        void Print() {
            printf("%c", v);
        }

        explicit CharValue(char v) {
            this->v = v;
        }
    };

    struct ValueCast {
        IntValue *v_int;
        FloatValue *v_float;
        CharValue *v_char;
        VariableValue *v_var;
        int type;
    };

    ValueCast valueCast(Value *v);

    template<typename T>
    bool checkValueSubType(Value &v);

    template<typename T>
    bool checkValueSubType(BasicValue &v);

    template<class T=BasicValue>
    class ArrayValue : public Value {
    public:
        T *arrayList;
        int size;

        ArrayValue();

        ~ArrayValue();
    };

    class Environment {
    public:
        Environment *Father;
        Environment *lastEnvironment;
        map<string, Value *> valueList;
        bool callEnvMark;

        Value *accessRealValue(Value *value); // 如果是变量类型，查找在符号表种的值，否则直接返回值

        Value *accessValue(Value *value); // 如果是变量类型，查找在符号表种的值，否则直接返回null

        Value *accessValue(string name);

        Value *accessValueInCurrentEnv(string name); // 在当前上下文中查找，用于局部变量声明检查

        void set(string name, Value *value);
        void add(string name, Value *value);

        Environment() {
            Father = lastEnvironment = nullptr;
        }

        ~Environment();
    };

}

using namespace environment;

class InterpretVisitor : public parser::Visitor {
public:
    Value *value;

    Visitor *v;

    Environment *GlobalEnvironment;


    Environment *CurrentEnvironment;

    bool returning;

    InterpretVisitor() {
        GlobalEnvironment = CurrentEnvironment = new Environment();
        this->v = this;
        value = nullptr;
        returning = false;
    }

    void LoadVisitor(Visitor *v) {
        this->v = v;
    }

    void LoadEnv(Environment *newEnv) { // 衔接上下文的切换
        newEnv->Father = CurrentEnvironment;
        newEnv->lastEnvironment = CurrentEnvironment;
        CurrentEnvironment = newEnv;
    }

    void LoadEnvWithoutContext(Environment *newEnv) { // 不衔接上下文的切换
        newEnv->Father = GlobalEnvironment;
        newEnv->lastEnvironment = CurrentEnvironment;
        CurrentEnvironment = newEnv;
    }

    void SwitchEnvBack() {
        CurrentEnvironment = CurrentEnvironment->lastEnvironment;
    }

    Value *getValue() {
        return this->value;
    }

    void visit(parser::Program *p) override;

    void visit(parser::Declaration *d) override;

    void visit(parser::ExprStmt *st) override;

    void visit(parser::BlockStmt *st) override;

    void visit(parser::IfStmt *st) override;

    void visit(parser::WhileStmt *st) override;

    void visit(parser::ReturnStmt *st) override;

    void visit(parser::PrintStmt *st) override;

    void visit(LocalDecl *dl) override;

    void visit(Assign *expr)override;

    void visit(parser::Equal *expr) override;

    void visit(parser::Comp *expr) override;

    void visit(parser::Term *expr) override;

    void visit(parser::Factor *expr) override;

    void visit(parser::Unary *expr) override;

    void visit(parser::Postfix *expr) override;

    void visit(parser::Char *expr) override;

    void visit(parser::Int *expr) override;

    void visit(parser::Float *expr) override;

    void visit(parser::Call *expr) override;

    void visit(parser::Var *expr) override;

};


#endif //C_COMPILER_EVALUATOR_H
