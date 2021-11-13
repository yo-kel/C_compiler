//
// Created by xsj on 2021/11/4.
//

#ifndef C_COMPILER_LEXER_H
#define C_COMPILER_LEXER_H

#pragma once

#include <string>
#include <vector>

using namespace std;

namespace lexer {
    const string KEYWORD[] = {
            "int", "long", "short", "float", "double", "char", "void",
            "unsigned", "signed", "const",
            "volatile", "enum", "struct", "union",
            "if", "else", "goto", "switch", "case", "do", "while", "for", "continue", "break", "return", "default",
            "typedef", "sizeof", "auto", "register", "extern", "static",
    };
    enum {
        INT = 0, LONG, SHORT, FLOAT, DOUBLE, CHAR, VOID,
        UNSIGNED, SIGNED, CONST,
        VOLATILE, ENUM, STRUCT, UNION,

        IF, ELSE, GOTO, SWITCH, CASE, DO, WHILE, FOR, CONTINUE, BREAK, RETURN, DEFAULT,
        TYPEDEF, SIZEOF, AUTO, REGISTER, EXTERN, STATIC,
        SYMBOL,ID, STRING, NUMBER, CHARACTOR,

        //+ - * / < <= > >= = == != & | ^ ++ --
        ADD, SUB, MUL, DIV, LT, LTE, GT, GTE, ASSIGN, EQ, NE, LAND, LOR, XOR,INC,DEC,
        //. ; ( ) [ ] { } / \ , # && || !
        POI, SEMI, LP, RP, LB, RB, LC, RC, SLA, BSLA, COMMA, SIGN, AND, OR, NOT,
    };

    int ParseToken();

    void init(string *tar);

    void outPrintTokenList();



    struct Token {
        int type;
        string name;
        string value;
        int line;
        int column;
    };

    extern vector<Token> tokenList;
}



#endif //C_COMPILER_LEXER_H
