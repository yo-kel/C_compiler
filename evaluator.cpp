//
// Created by xsj on 2021/11/13.
//

#include "evaluator.h"

using namespace environment;
using namespace error;

template<typename T>
bool checkValueSubType(Value &v) {
    if (dynamic_cast<T *>(&v) == nullptr) {
        return false;
    }
    return true;
}

template<typename T>
bool checkValueSubType(BasicValue &v) {
    if (dynamic_cast<T *>(v) == nullptr) {
        return false;
    }
    return true;
}

// using visitor design pattern to interpret ast

Value *ValueFromAstType(int type) {
    switch (type) {
        case lexer::INT:
            return new IntValue(0);
        case lexer::FLOAT:
            return new FloatValue(0);
        case lexer::CHAR:
            return new CharValue(0);
        default:
            outError(error::TypeError);
    }
}

void InterpretVisitor::visit(Program *p) {
    for (int i = 0; i < p->declarationList.size(); i++) {
        // TODO declaration needs to be refactored
        p->declarationList[i]->accept(this->v);
    }

    //execute main function
    auto value = CurrentEnvironment->accessValue("main");
    if (value == nullptr) {
        // 未声明
        outError(error::NoDeclError);
    }

    auto func = dynamic_cast<CallValue *>(value);

    if (func == nullptr) {
        // ID不是函数
        outError(error::IDNotFuncError);
    }
    func->funcDecl->blockStmt->accept(this->v);
}

void InterpretVisitor::visit(Declaration *d) {
    auto funcDecl = dynamic_cast<FuncDecl *>(d);
    auto VarDecl = dynamic_cast<VariableDecl *>(d);
    auto declCheck = this->CurrentEnvironment->accessValue(d->typeVar->ID);

    if (declCheck != nullptr) {
        outError(error::RepeatDeclError);
    }

    if (funcDecl != nullptr) { ;
        this->GlobalEnvironment->add(
                funcDecl->typeVar->ID,
                new CallValue(funcDecl));
    } else if (VarDecl != nullptr) {
        this->GlobalEnvironment->add(
                d->typeVar->ID,
                ValueFromAstType(VarDecl->typeVar->typeSpec->type));
    } else {
        outError(error::TypeError);
    }
}

void InterpretVisitor::visit(ExprStmt *st) {
    st->expr->accept(this->v);
}

void InterpretVisitor::visit(BlockStmt *st) {
    // when meeting block statement, switch context;

    // TODO however using call will switch context,
    //  then visit blockStmt switch again???

    // local variable must be declared before stmt;
    auto newEnv = new Environment();
    this->LoadEnv(newEnv);

    st->localDeclList->accept(this->v);

    for (int i = 0; i < st->statementList.size(); i++) {
        st->statementList[i]->accept(this->v);
    }

    this->SwitchEnvBack();
}

void InterpretVisitor::visit(IfStmt *st) {
    st->expr->accept(this->v);
    auto vc = valueCast(this->value);
    if (vc.v_int == nullptr) {
        outError(TypeError);
    }

    if (vc.v_int->v == 1) {
        st->statement1->accept(this->v);
    } else if (vc.v_int->v == 0) {
        st->statement2->accept(this->v);
    } else {
        outError(TypeError);
    }
}

void InterpretVisitor::visit(WhileStmt *st) {
    while (true) {
        if (this->returning)
            st->expr->accept(this->v);
        auto vc = valueCast(this->value);
        if (vc.v_int->v == 1) {
            st->statement->accept(this->v);
        } else if (vc.v_int->v == 1) {
            break;
        } else {
            outError(TypeError);
        }
    }
}

void InterpretVisitor::visit(ReturnStmt *st) {
    st->expr->accept(this->v);
    // TODO 遇到函数返回，要控制代码结束,whileStmt,blockStmt要中止语句
    this->returning = true;
}

void InterpretVisitor::visit(PrintStmt *st) {
    st->expr->accept(this->v);
    auto v = getValue();
    v->Print();
}

void InterpretVisitor::visit(LocalDecl *dl) {
    for (int i = 0; i < dl->declList.size(); i++) {
        this->GlobalEnvironment->add(
                dl->declList[i]->ID,
                ValueFromAstType(dl->declList[i]->typeSpec->type));
    }
}

//visitor method for expression;

void InterpretVisitor::visit(Assign *expr) {
    expr->exp->accept(this->v);
    auto v = getValue();
    auto c = valueCast(v);
//    cout<<c.v_var<<endl;
//    cout<<(dynamic_cast<Var*>(expr->exp))<<endl;

    if (!(::checkValueSubType<VariableValue>(*v))) {
        outError(EqualLeftVar);
    }
    auto v_var = dynamic_cast<VariableValue *>(v);

    for (int i = expr->exprList.size() - 1; i >= 1; i--) {
        expr->exprList[i].expr->accept(this->v);
        auto v2 = getValue();

        expr->exprList[i - 1].expr->accept(this->v);
        auto v_ = getValue();
        if (!::checkValueSubType<VariableValue>(*v_)) {
            outError(EqualLeftVar);
        }
        auto v_var = dynamic_cast<VariableValue *>(v_);
        auto realValue = this->CurrentEnvironment->accessRealValue(v2);
        this->CurrentEnvironment->set(v_var->var, realValue);
    }
    if (expr->exprList.size()) {


        expr->exprList[0].expr->accept(this->v);
        auto v2 = getValue();

        auto realValue = this->CurrentEnvironment->accessRealValue(v2);
        this->CurrentEnvironment->set(v_var->var, realValue);
    }
    this->value = new VariableValue(v_var->var);
}

void InterpretVisitor::visit(Equal *expr) {
    expr->exp->accept(this->v);
    auto v1 = this->CurrentEnvironment->accessRealValue(this->value);
    for (int i = 0; i < expr->exprList.size(); i++) {
        auto p = expr->exprList[i];
        p.expr->accept(this->v);
        auto v2 = this->CurrentEnvironment->accessRealValue(this->value);
        auto c1 = valueCast(v1);
        auto c2 = valueCast(v2);
        switch (p.ope->type) {
            case lexer::EQ:
                if (c1.v_int && c2.v_int) {
                    v1 = new IntValue((c1.v_int->v) == (c2.v_int->v));
                } else if (c1.v_float && c2.v_float) {
                    v1 = new FloatValue((c1.v_float->v) == (c2.v_float->v));
                } else if (c1.v_char && c2.v_char) {
                    v1 = new CharValue(char(c1.v_char->v == c2.v_char->v));
                } else {
                    outError(error::TypeError);
                }
                break;
            case lexer::NE:
                if (c1.v_int && c2.v_int) {
                    v1 = new IntValue((c1.v_int->v) != (c2.v_int->v));
                } else if (c1.v_float && c2.v_float) {
                    v1 = new FloatValue((c1.v_float->v) != (c2.v_float->v));
                } else if (c1.v_char && c2.v_char) {
                    v1 = new CharValue(char(c1.v_char->v != c2.v_char->v));
                } else {
                    outError(error::TypeError);
                }
                break;
        }
        this->value = v1;
    }
}

void InterpretVisitor::visit(Comp *expr) {
    expr->exp->accept(this->v);
    auto v1 = this->CurrentEnvironment->accessRealValue(this->value);
    for (int i = 0; i < expr->exprList.size(); i++) {
        auto p = expr->exprList[i];

        p.expr->accept(this->v);
        auto v2 = this->CurrentEnvironment->accessRealValue(this->value);

        auto v1_int = dynamic_cast<IntValue *>(v1);
        auto v2_int = dynamic_cast<IntValue *>(v2);
        auto v1_float = dynamic_cast<FloatValue *>(v1);
        auto v2_float = dynamic_cast<FloatValue *>(v2);
        auto v1_char = dynamic_cast<CharValue *>(v1);
        auto v2_char = dynamic_cast<CharValue *>(v2);

        switch (p.ope->type) {
            case lexer::GT:
                if (v1_int && v2_int) {
                    v1 = new IntValue((v1_int->v) > (v2_int->v));
                } else if (v1_float && v2_float) {
                    v1 = new IntValue((v1_float->v) > (v2_float->v));
                } else if (v1_char && v2_char) {
                    v1 = new IntValue((v1_char->v) > (v2_char->v));
                } else {
                    outError(error::TypeError);
                }
                break;
            case lexer::GTE:
                if (v1_int && v2_int) {
                    v1 = new IntValue((v1_int->v) >= (v2_int->v));
                } else if (v1_float && v2_float) {
                    v1 = new IntValue((v1_float->v) >= (v2_float->v));
                } else if (v1_char && v2_char) {
                    v1 = new IntValue((v1_char->v) >= (v2_char->v));
                } else {
                    outError(error::TypeError);
                }
                break;;
            case lexer::LT:
                if (v1_int && v2_int) {
                    v1 = new IntValue((v1_int->v) < (v2_int->v));
                } else if (v1_float && v2_float) {
                    v1 = new IntValue((v1_float->v) < (v2_float->v));
                } else if (v1_char && v2_char) {
                    v1 = new IntValue((v1_char->v) < (v2_char->v));
                } else {
                    outError(error::TypeError);
                }
                break;

            case lexer::LTE:
                if (v1_int && v2_int) {
                    v1 = new IntValue((v1_int->v) <= (v2_int->v));
                } else if (v1_float && v2_float) {
                    v1 = new IntValue((v1_float->v) <= (v2_float->v));
                } else if (v1_char && v2_char) {
                    v1 = new IntValue((v1_char->v) <= (v2_char->v));
                } else {
                    outError(error::TypeError);
                }
                break;


            case lexer::AND:
                if (v1_int && v2_int) {
                    v1 = new IntValue((v1_int->v) && (v2_int->v));
                } else if (v1_char && v2_char) {
                    v1 = new IntValue((v1_char->v) && (v2_char->v));
                } else {
                    outError(error::TypeError);
                }
                break;


            case lexer::OR:
                if (v1_int && v2_int) {
                    v1 = new IntValue((v1_int->v) || (v2_int->v));
                } else if (v1_char && v2_char) {
                    v1 = new IntValue((v1_char->v) || (v2_char->v));
                } else {
                    outError(error::TypeError);
                }
                break;
            case lexer::LAND:
                if (v1_int && v2_int) {
                    v1 = new IntValue((v1_int->v) & (v2_int->v));
                } else if (v1_char && v2_char) {
                    v1 = new IntValue((v1_char->v) & (v2_char->v));
                } else {
                    outError(error::TypeError);
                }
                break;
            case lexer::LOR:
                if (v1_int && v2_int) {
                    v1 = new IntValue((v1_int->v) | (v2_int->v));
                } else if (v1_char && v2_char) {
                    v1 = new IntValue((v1_char->v) | (v2_char->v));
                } else {
                    outError(error::TypeError);
                }
                break;
        }
    }
    this->value = v1;
}

void InterpretVisitor::visit(Term *expr) {
    expr->exp->accept(this->v);
    auto v1 = this->value;
    for (int i = 0; i < expr->exprList.size(); i++) {
        auto p = expr->exprList[i];

        p.expr->accept(this->v);
        auto v2 = this->value;

        auto c1 = valueCast(v1);
        auto c2 = valueCast(v2);

        auto v1_var = dynamic_cast<VariableValue *>(v1);
        if (v1_var) {
            c1 = valueCast(v1_var->basicValue);
        }


        auto v2_var = dynamic_cast<VariableValue *>(v2);
        if (v2_var) {
            c2 = valueCast(v2_var->basicValue);
        }

        switch (p.ope->type) {
            case lexer::ADD:
                if (c1.v_int && c2.v_int) {
                    v1 = new IntValue((c1.v_int->v) + (c2.v_int->v));
                } else if (c1.v_float && c2.v_float) {
                    v1 = new FloatValue((c1.v_float->v) + (c2.v_float->v));
                } else if (c1.v_char && c2.v_char) {
                    v1 = new CharValue(char(c1.v_char->v + c2.v_char->v));
                } else {
                    outError(error::TypeError);
                }
                break;
            case lexer::SUB:
                if (c1.v_int && c2.v_int) {
                    v1 = new IntValue((c1.v_int->v) - (c2.v_int->v));
                } else if (c1.v_float && c2.v_float) {
                    v1 = new FloatValue((c1.v_float->v) - (c2.v_float->v));
                } else if (c1.v_char && c2.v_char) {
                    v1 = new CharValue(char(c1.v_char->v - c2.v_char->v));
                } else {
                    outError(error::TypeError);
                }
                break;
        }
    }

    this->value = v1;
}

void InterpretVisitor::visit(Factor *expr) {
    expr->exp->accept(this->v);
    auto v1 = this->value;
    for (int i = 0; i < expr->exprList.size(); i++) {
        auto p = expr->exprList[i];

        p.expr->accept(this->v);
        auto v2 = this->value;

        auto c1 = valueCast(v1);
        auto c2 = valueCast(v2);

        auto v1_var = dynamic_cast<VariableValue *>(v1);
        if (v1_var) {
            c1 = valueCast(v1_var->basicValue);
        }


        auto v2_var = dynamic_cast<VariableValue *>(v2);
        if (v2_var) {
            c2 = valueCast(v2_var->basicValue);
        }

        switch (p.ope->type) {
            case lexer::MUL:
                if (c1.v_int && c2.v_int) {
                    v1 = new IntValue((c1.v_int->v) * (c2.v_int->v));
                } else if (c1.v_float && c2.v_float) {
                    v1 = new FloatValue((c1.v_float->v) * (c2.v_float->v));
                } else if (c1.v_char && c2.v_char) {
                    v1 = new CharValue(char(c1.v_char->v * c2.v_char->v));
                } else {
                    outError(error::TypeError);
                }
                break;
            case lexer::DIV:
                if (c1.v_int && c2.v_int) {
                    v1 = new IntValue((c1.v_int->v) / (c2.v_int->v));
                } else if (c1.v_float && c2.v_float) {
                    v1 = new FloatValue((c1.v_float->v) / (c2.v_float->v));
                } else if (c1.v_char && c2.v_char) {
                    v1 = new CharValue(char(c1.v_char->v / c2.v_char->v));
                } else {
                    outError(error::TypeError);
                }
                break;
        }
    }

    this->value = v1;
}

void InterpretVisitor::visit(Unary *expr) {
    expr->exp->accept(this->v);
    auto v1 = this->value;
    for (int i = 0; i < expr->exprList.size(); i++) {
        auto p = expr->exprList[i];

        auto c1 = valueCast(v1);

        switch (p.ope->type) {
            case lexer::NOT:
                if (c1.v_int) {
                    v1 = new IntValue(!c1.v_int->v);
                } else if (c1.v_char) {
                    v1 = new IntValue(!c1.v_char->v);
                } else {
                    outError(error::TypeError);
                }
                break;
            case lexer::SUB:
                if (c1.v_int) {
                    v1 = new IntValue(-c1.v_int->v);
                } else if (c1.v_char) {
                    v1 = new IntValue(-c1.v_char->v);
                } else {
                    outError(error::TypeError);
                }
                break;
            case lexer::INC:
                if (c1.v_var) {
                    auto tar = valueCast(this->CurrentEnvironment->accessValue(c1.v_var));
                    if (tar.v_char) {
                        tar.v_char++, c1.v_char++;
                    } else if (tar.v_int) {
                        tar.v_int++, c1.v_int++;
                    } else {
                        outError(error::TypeError);
                    }
                } else {
                    outError(error::TypeError);
                }
                break;
            case lexer::DEC:
                if (c1.v_var) {
                    auto tar = valueCast(this->CurrentEnvironment->accessValue(c1.v_var));
                    if (tar.v_char) {
                        tar.v_char--;
                    } else if (tar.v_int) {
                        tar.v_int--;
                    } else {
                        outError(error::TypeError);
                    }
                } else {
                    outError(error::TypeError);
                }
                break;
        }
    }

    this->value = v1;
}

void InterpretVisitor::visit(Postfix *expr) {
    expr->exp->accept(this->v);
    auto v1 = this->value;
    for (int i = 0; i < expr->exprList.size(); i++) {
        auto p = expr->exprList[i];

        auto c1 = valueCast(v1);


        switch (p.ope->type) {
            case lexer::INC:
                if (c1.v_var) {
                    auto tar = valueCast(this->CurrentEnvironment->accessValue(c1.v_var));
                    if (tar.v_char) {
                        tar.v_char--;
                    } else if (tar.v_int) {
                        tar.v_int--;
                    } else {
                        outError(error::TypeError);
                    }
                } else {
                    outError(error::TypeError);
                }
            case lexer::DEC:
                if (c1.v_var) {
                    auto tar = valueCast(this->CurrentEnvironment->accessValue(c1.v_var));
                    if (tar.v_char) {
                        tar.v_char--;
                    } else if (tar.v_int) {
                        tar.v_int--;
                    } else {
                        outError(error::TypeError);
                    }
                } else {
                    outError(error::TypeError);
                }
                break;
        }

    }
    this->value = v1;
}

void InterpretVisitor::visit(Char *expr) {
    this->value = new CharValue(expr->value);
}

void InterpretVisitor::visit(Int *expr) {
    this->value = new IntValue(expr->value);

}

void InterpretVisitor::visit(Float *expr) {
    this->value = new FloatValue(expr->value);
}

void InterpretVisitor::visit(Var *expr) {
    auto v = dynamic_cast<BasicValue *>( this->CurrentEnvironment->accessValue(expr->ID->value));
    if (v == nullptr) {
        outError(error::TypeError);
    }
    this->value = new VariableValue(expr->ID->value, v);

}

void InterpretVisitor::visit(Call *expr) {
    auto value = CurrentEnvironment->accessValue(expr->ID->value);

    auto func = dynamic_cast<CallValue *>(value);

    if (value == nullptr) {
        // 未声明
        outError(error::NoDeclError);
    }

    if (func == nullptr) {
        // ID不是函数
        outError(error::IDNotFuncError);
    }


    auto newEnv = new Environment();

    for (int i = 0; i < expr->args.size(); i++) {
        auto arg = expr->args[i];
        arg->accept(this->v);
        auto v = this->value;
        auto cast = valueCast(v);
        if (func->funcDecl->params->params[i].typeSpec->type != cast.type) {
            outError(TypeError);
        }

        // prepare new environment for function call
        newEnv->set(func->funcDecl->params->params[i].ID, v);
    }
    this->CurrentEnvironment->callEnvMark = true;
    this->LoadEnvWithoutContext(newEnv);
    //switch environment and execute calling function
    func->funcDecl->blockStmt->accept(this->v);


}

Value *Environment::accessRealValue(Value *value) {
    auto c = valueCast(value);
    if (c.v_var != nullptr) {
        return accessValue(c.v_var->var);
    }
    return value;
}

Value *Environment::accessValue(Value *value) {
    auto c = valueCast(value);
    if (c.v_var != nullptr) {
        return accessValue(c.v_var->var);
    }
    return nullptr;
}

Value *Environment::accessValue(string name) {
    auto now = this;
    while (now != nullptr) {
        if (now->valueList.find(name) != now->valueList.end()) {
            return now->valueList[name];
        }
        now = now->Father;
    }
    return nullptr;
}

Value *Environment::accessValueInCurrentEnv(string name) {
    if (this->valueList.find(name) != this->valueList.end()) {
        return this->valueList[name];
    }
    return nullptr;
}

void Environment::add(string name, Value *value) {
    auto c= valueCast(value);
    auto now=this;
    if(c.v_var){
        this->valueList[name]=c.v_var->basicValue;
    }else {
        this->valueList[name] = value;
    }
}

void Environment::set(string name, Value *value) {
    auto c= valueCast(value);
    auto now=this;
    while(now!= nullptr) {
        if(now->valueList.find(name)!=now->valueList.end()) {
            if (c.v_var) {
                now->valueList[name] = c.v_var->basicValue;
            } else {
                now->valueList[name] = value;
            }
            break;
        }
        now=now->Father;
    }
}

ValueCast environment::valueCast(Value *v) {
    ValueCast re;
    re.v_var = dynamic_cast<VariableValue *>(v);

    //如果是变量类型，查看其值是什么类型
    if (re.v_var) {
        re.v_int = dynamic_cast<IntValue *>(re.v_var->basicValue);
        re.v_float = dynamic_cast<FloatValue *>(re.v_var->basicValue);
        re.v_char = dynamic_cast<CharValue *>(re.v_var->basicValue);
    } else {
        re.v_int = dynamic_cast<IntValue *>(v);
        re.v_float = dynamic_cast<FloatValue *>(v);
        re.v_char = dynamic_cast<CharValue *>(v);
    }

    // value与ast节点类型解析不适配，通过type字段判断类型
    if (re.v_int != nullptr)re.type = lexer::INT;
    else if (re.v_float != nullptr)re.type = lexer::FLOAT;
    else if (re.v_char != nullptr)re.type = lexer::CHAR;

    return re;
}