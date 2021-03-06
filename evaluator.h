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

        virtual Value* deepCopy()=0;
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
        Value* deepCopy()override{
            return nullptr;
        }
    };

    class BasicValue : public Value {
    public:
        virtual Value* deepCopy()=0;
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
        Value* deepCopy()override{
            return basicValue->deepCopy();
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
        BasicValue* deepCopy()override{
            return new IntValue(this->v);
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

        BasicValue* deepCopy()override{
            return new FloatValue(this->v);
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

        BasicValue* deepCopy()override{
            return new CharValue(this->v);
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
        const static int WHILE=1;
        const static int FUNC=2;
        Environment *Father;
        Environment *lastEnvironment;
        map<string, Value *> valueList;
        int type;

        Value *accessRealValue(Value *value); // ???????????????????????????????????????????????????????????????????????????

        Value *accessValue(Value *value); // ????????????????????????????????????????????????????????????????????????null

        Value *accessValue(string name);

        Value *accessValueInCurrentEnv(string name); // ????????????????????????????????????????????????????????????

        void set(string name, Value *value);
        void add(string name, Value *value);

        Environment() {
            Father = lastEnvironment = nullptr;
            type=0;
        }

        Environment(int type){
            Father = lastEnvironment = nullptr;
            this->type=type;
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

    const static int GOTOWHILE=1;
    const static int GOTOFUNC=2;
    const static int ACTIONBREAK=1;
    const static int ACTIONCONTINUE=2;
    int returning;
    int action;

    InterpretVisitor() {
        GlobalEnvironment = CurrentEnvironment = new Environment();
        this->v = this;
        value = nullptr;
        returning = 0;
        action=0;
    }

    void LoadVisitor(Visitor *v) {
        this->v = v;
    }

    void LoadEnv(Environment *newEnv) { // ????????????????????????
        newEnv->Father = CurrentEnvironment;
        newEnv->lastEnvironment = CurrentEnvironment;
        CurrentEnvironment = newEnv;
    }

    void LoadEnvWithoutContext(Environment *newEnv) { // ???????????????????????????
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
    void visit(parser::BreakStmt *st) override;
    void visit(parser::ContinueStmt *st) override;

    void visit(parser::ReturnStmt *st) override;

    void visit(parser::PrintStmt *st) override;

    void visit(LocalDecl *dl) override;

    void visit(Assign *expr)override;

//    void visit(parser::Equal *expr) override;

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
