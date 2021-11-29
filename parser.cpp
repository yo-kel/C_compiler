//
// Created by xsj on 2021/11/4.
//

#include "parser.h"

using namespace parser;
using namespace lexer;

#define StringEOF -1


namespace parser {

    vector<Token> *TokenList;

    vector<Token>::iterator it, token;

    bool isTokenAType(Token &token) {
        switch (token.type) {
            case lexer::INT:
            case lexer::SHORT:
            case lexer::LONG:
            case lexer::FLOAT:
            case lexer::DOUBLE:
            case lexer::CHAR:
            case lexer::VOID:
                return true;
            default:
                return false;
        }
    }

    bool checkToken(int tokenType) {
        if (tokenType == -1 && it == TokenList->end())return true;
        if (it == TokenList->end())return false;
        return it->type == tokenType;
    };

    bool checkToken2(int tokenType) {
        if (it == TokenList->end() || it + 1 == TokenList->end())return false;
        return (it + 1)->type == tokenType;
    };

    Token match(int tokenType) {
        if (it == TokenList->end())return Token{-1, "", "", 0, 0};
        cout << "token:" << tokenType << " value:(" << it->value << ") " << it->type << " " << it->name << endl;
        if ((it + 1) != TokenList->end()) {
            cout << "                " << (it + 1)->name;
            if ((it + 2) != TokenList->end())cout << " " << (it + 2)->name;
            cout << endl;
        }
        if (it->type == tokenType) {
            auto tmpToken = *it;
            it++;
            return tmpToken;
        } else {
            printf("ast: match error\n");
            exit(1);
            return Token{-1};
        };
    }

    Token next() {
        if (it == TokenList->end())return Token{-1, "", "", 0, 0};
        return *(it + 1);
    }

    void init(vector<Token> *tokenListPtr) {
        TokenList = tokenListPtr;
        it = tokenList.begin();
    }

    Program *program() {
        vector<Declaration *> declarationList;
        while (!checkToken(StringEOF)) {
            TypeVar *typeVar1 = ::typeVar();
            if (checkToken(lexer::LP)) {
                // declBody = funcDecl();

                match(lexer::LP);
                Params *paramsPtr = nullptr;
                if (isTokenAType(*it)) {
                    paramsPtr = ::params();
                }
                match(lexer::RP);
                BlockStmt *blockStmtPtr = blockStmt();
                declarationList.push_back(new FuncDecl(paramsPtr, blockStmtPtr, typeVar1));

            } else {
                // declBody = variableDecl();
                declarationList.push_back(new VariableDecl(typeVar1));
            }
        }
        return new Program(declarationList);
    }

    TypeVar *typeVar() {
        //leading token
        TypeSpec *typeSpecPtr = nullptr;
        if (isTokenAType(*it)) {
            typeSpecPtr = typeSpec();
        }

        //TODO pointer has not been implemented

        auto ID = match(lexer::ID);

        return new TypeVar(typeSpecPtr, ID.value);
    }

    TypeSpec *typeSpec() {
        switch (it->type) {
            case lexer::INT:
            case lexer::SHORT:
            case lexer::LONG:
            case lexer::FLOAT:
            case lexer::DOUBLE:
            case lexer::CHAR:
            case lexer::VOID:
                auto tmpToken = it;
                match(it->type);
                return new TypeSpec(tmpToken->type);
        }
        printf("typeSpec: error\n");
        return nullptr;
    }

//    VariableDecl *variableDecl() {
//        //TODO {NUMBER}*
//        match(lexer::SEMI);
//        return new VariableDecl;
//    }

//    FuncDecl *funcDecl() {
//        match(lexer::LP);
//        Params *paramsPtr = nullptr;
//        if (isTokenAType(*it)) {
//            paramsPtr = ::params();
//        }
//        match(lexer::RP);
//        BlockStmt *blockStmtPtr = blockStmt();
//        return new FuncDecl(paramsPtr, blockStmtPtr);
//    }

    Params *params() {
        vector<TypeVar> paramList;
        paramList.push_back(*(::typeVar()));
        while (checkToken(lexer::COMMA)) {
            match(lexer::COMMA);
            paramList.push_back(*(::typeVar()));
        }

        return new Params(paramList);
    }

    LocalDecl *localDecl() {
        vector<TypeVar *> declList;

        while (isTokenAType(*it)) { // match {TypeVar}*
            declList.push_back(::typeVar());
            match(lexer::SEMI);
        }
        return new LocalDecl(declList);
    }

    Statement *statement() {
        if (checkToken(lexer::LC)) {
            return blockStmt();
        } else if (checkToken(lexer::IF)) {
            return ifStmt();
        } else if (checkToken(lexer::WHILE)) {
            return whileStmt();
        } else if (checkToken(lexer::FOR)) {
            return forStmt();
        } else if (checkToken(lexer::RETURN)) {
            return returnStmt();
        } else if (checkToken(lexer::CONTINUE)) {
            return continueStmt();
        } else if (checkToken(lexer::BREAK)) {
            return breakStmt();
        } else if (checkToken(lexer::PRINT)) {
            return printStmt();
        }
            //TODO Assert statement unimplemented
        else {
            //to simplify the procedure,Expr statement is classified in else
            return exprStmt();
        }
    }

    BlockStmt *blockStmt() {
        match(lexer::LC);
        vector<Statement *> statementList;
        auto local = localDecl();
        while (!checkToken(lexer::RC)) {
            statementList.push_back(parser::statement());
        }
//        while (!checkToken(lexer::RC)) {
//            if (isTokenAType(*it)) {
//                localDeclList.push_back(parser::localDecl());
//            } else {
//                statementList.push_back(parser::statement());
//            }
//        }
        match(lexer::RC);

        return new BlockStmt(local, statementList);
    }

    IfStmt *ifStmt() {
        match(lexer::IF);
        match(lexer::LP);

        auto expr1 = expr();
        match(lexer::RP);

        auto stmt1 = parser::statement();
        Statement *stmt2 = nullptr;
        if (checkToken(lexer::ELSE)) {
            match(lexer::ELSE);
            stmt2 = parser::statement();
        }
        return new IfStmt(expr1, stmt1, stmt2);
    }

    WhileStmt *whileStmt() {
        match(lexer::WHILE);
        match(lexer::LP);
        auto expr = parser::expr();
        match(lexer::RP);
        auto stmt = parser::statement();
        return new WhileStmt(expr, stmt);
    }

    ReturnStmt *returnStmt() {
        match(lexer::RETURN);
        Expr *expr1 = nullptr;
        if (!checkToken(lexer::SEMI))expr1 = expr();
        match(lexer::SEMI);
        return new ReturnStmt(expr1);
    }

    ContinueStmt *continueStmt() {
        match(lexer::CONTINUE);
        match(lexer::SEMI);
        return new ContinueStmt();
    }

    BreakStmt *breakStmt() {
        match(lexer::BREAK);
        match(lexer::SEMI);
        return new BreakStmt();
    }

    PrintStmt *printStmt() {
        match(lexer::PRINT);
        match(lexer::LP);
        auto ex = expr();
        match(lexer::RP);
        match(lexer::SEMI);
        return new PrintStmt(ex);
    }

    ExprStmt *exprStmt() {
        Expr *expr1 = nullptr;
        if (!checkToken(lexer::SEMI)) {
            expr1 = expr();
        }
        match(lexer::SEMI);
        return new ExprStmt(expr1);
    }

    Expr *expr() {
        Expr *expr1 = ::equality();
        if (checkToken(lexer::ASSIGN)) {
            vector<Expr::OpeExpr> exprList;
            while (checkToken(lexer::ASSIGN)) {
                auto ope = &(*it);
                match(lexer::ASSIGN);
                auto expr2 = ::equality();
                exprList.push_back(Expr::OpeExpr{expr2, ope});
            }
            return new Assign(expr1, exprList);
        } else {
            return expr1;
        }
    }

    Expr *equality() {
        Expr *expr1 = ::comp();
        if (checkToken(lexer::EQ) || checkToken(lexer::NE)) {
            vector<Expr::OpeExpr> exprList;
            while (checkToken(lexer::EQ) || checkToken(lexer::NE)) {
                auto ope = &(*it);
                match(ope->type);
                auto expr2 = ::comp();
                exprList.push_back(Expr::OpeExpr{expr2, ope});
            }
            return new Equal(expr1, exprList);
        } else {
            return expr1;
        }
    }

    Expr *comp() {
        Expr *expr1 = ::term();
        if (checkToken(lexer::GT) || checkToken(lexer::GTE) ||
            checkToken(lexer::LT) || checkToken(lexer::LTE) ||
            checkToken(lexer::AND) || checkToken(lexer::OR) ||
            checkToken(lexer::LAND) || checkToken(lexer::LOR)) {
            vector<Expr::OpeExpr> exprList;
            while (checkToken(lexer::GT) || checkToken(lexer::GTE) ||
                   checkToken(lexer::LT) || checkToken(lexer::LTE) ||
                   checkToken(lexer::AND) || checkToken(lexer::OR) ||
                   checkToken(lexer::LAND) || checkToken(lexer::LOR)) {
                auto ope = &(*it);
                match(ope->type);

                auto expr2 = ::term();
                exprList.push_back(Expr::OpeExpr{expr2, ope});
            }
            return new Comp(expr1, exprList);
        } else {
            return expr1;
        }
    }

    Expr *term() {
        Expr *expr1 = ::factor();
        if (checkToken(lexer::ADD) || checkToken(lexer::SUB)) {
            vector<Expr::OpeExpr> exprList;
            while (checkToken(lexer::ADD) || checkToken(lexer::SUB)) {
                auto ope = &(*it);
                match(ope->type);
                auto expr2 = ::factor();
                exprList.push_back(Expr::OpeExpr{expr2, ope});
            }
            return new Term(expr1, exprList);
        } else {
            return expr1;
        }
    }

    Expr *factor() {
        Expr *expr1 = ::unary();
        if (checkToken(lexer::MUL) || checkToken(lexer::DIV)) {
            vector<Expr::OpeExpr> exprList;
            while (checkToken(lexer::MUL) || checkToken(lexer::DIV)) {
                auto ope = &(*it);
                match(ope->type);
                auto expr2 = ::unary();
                exprList.push_back(Expr::OpeExpr{expr2, ope});
            }
            return new Factor(expr1, exprList);
        } else {
            return expr1;
        }
    }

    Expr *unary() {
        if (checkToken(lexer::NOT) || checkToken(lexer::SUB) ||
            checkToken(lexer::INC) || checkToken(lexer::DEC)) {
            vector<Expr::OpeExpr> exprList;
            while (checkToken(lexer::NOT) || checkToken(lexer::SUB) ||
                   checkToken(lexer::INC) || checkToken(lexer::DEC)) {
                auto ope = &(*it);
                match(ope->type);
                exprList.push_back(Expr::OpeExpr{nullptr, ope});
            }
            Expr *expr1 = ::postfix();
            return new Unary(expr1, exprList);
        } else {
            Expr *expr1 = ::postfix();
            return expr1;
        }
    }

    Expr *postfix() {
        Expr *expr1 = ::item();
        if (checkToken(lexer::INC) || checkToken(lexer::DEC)) {
            vector<Expr::OpeExpr> exprList;
            while (checkToken(lexer::INC) || checkToken(lexer::DEC)) {
                auto ope = &(*it);
                match(ope->type);
                exprList.push_back(Expr::OpeExpr{nullptr, ope});
            }
            return new Postfix(expr1, exprList);
        } else {
            return expr1;
        }
    }

    Expr *item() {
        if (checkToken(lexer::LP)) {
            match(lexer::LP);
            auto expr1 = ::expr();
            match(lexer::RP);
            return expr1;
        } else if (checkToken(lexer::ID)) {
            auto ID = &(*it);
            match(lexer::ID);
            if (checkToken(lexer::LP)) {
                match(lexer::LP);
                vector<Expr *> args;
                if (!checkToken(lexer::RP)) {
                    auto arg1 = ::expr();
                    args.push_back(arg1);
                    while (checkToken(lexer::COMMA)) {
                        match(lexer::COMMA);
                        auto arg1_ = ::expr();
                        args.push_back(arg1_);
                    }
                }
                match(lexer::RP);
                return new Call(ID, args);
            } else {
                vector<Expr *> arrayNum;
                while (checkToken(lexer::LB)) {
                    match(lexer::LB);
                    auto arg1 = ::expr();
                    match(lexer::RB);
                    arrayNum.push_back(arg1);
                }
                return new Var(ID, arrayNum);
            }
        } else if (checkToken(lexer::NUMBER)) {
            auto num = &(*it);
            auto p = num->value.find('.');
            match(lexer::NUMBER);
            if (p == (std::string::npos)) { //int
                return new Int(stoi(num->value));
            } else { //
                return new Float(stof(num->value));
            }
        } else if (checkToken(lexer::CHARACTOR)) {
            auto c = &(*it);
            match(lexer::CHARACTOR);
            return new Char(c->value[0]);
        } else if (checkToken(lexer::STRING)) {
            auto c = &(*it);
            match(lexer::STRING);
            return new String(c->value);
        }
        return nullptr;
    }

    ForStmt *forStmt() {
        return new ForStmt{};
    }
}