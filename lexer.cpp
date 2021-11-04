//
// Created by xsj on 2021/11/4.
//

#pragma once

#include <iostream>
#include <lexer.h>

using namespace lexer;


string::iterator str, lat_pos;
vector <Token> tokenList;

int line, column;

void lexer::init(string *tar) {
    str = tar->begin();
    line = 1, column = 1;
}

int isKeyword(string id) {
    for (int i = 0; i < SYMBOL; i++) {
        if (KEYWORD[i] == id)return i;
    }
    return -1;
}

void nextStr() {
    column++;
    str++;
}

void lexer::outPrintTokenList() {
    for (auto i = tokenList.begin(); i != tokenList.end(); i++) {
        cout << i->line << ":" << i->column << "  (" << i->name << "," << i->value << ")" << endl;
    }
}

int lexer::ParseToken() {
    int token = -1;
    while (token = *str) {
        nextStr();
        if (token == '\n') {
            line++;
            column = 1;
        } else if (token == '/') {
            if (*str == '/') { //  //comment
                while (token = *str, token && token != '\n')nextStr();
            } else if (*str == '*') {//  /*comment
                nextStr();
                while (token = *str, token) {
                    if (token == '\n') {
                        line++;
                        column = 1;
                    }
                    nextStr();
                    if (token == '*' && *str == '/') {
                        nextStr();
                        break;
                    }
                }
            } else { // division
                token = DIV;
                tokenList.push_back(Token{DIV, "/", "", line, column});
            }
        } else if (isalpha(token) || token == '_') { // symbol
            lat_pos = str - 1;
            while (isalnum(*str) || token == '_')nextStr();
            auto value = string(lat_pos, str);
            int keyword_id = isKeyword(value);
            if (keyword_id == -1) tokenList.push_back(Token{SYMBOL, "symbol", value, line, column});
            else tokenList.push_back(Token{keyword_id, KEYWORD[keyword_id], "", line, column});
        } else if (isdigit(token)) { // number
            lat_pos = str - 1;
            while (isdigit((*str))) {
                nextStr();
            }
            if (*str == '.') {
                nextStr();
                while (isdigit(*str)) {
                    nextStr();
                }
            }
            auto value = string(lat_pos, str);
            tokenList.push_back(Token{NUMBER, "number", value, line, column});
        } else if (token == '\'') { // char
            lat_pos = str;
            int cnt = -1;
            while (*str) {
                if (*str == '\'' && token != '\\') {
                    break;
                }
                if (token != '\\')cnt++;
                token = *str;
                nextStr();
            }
            nextStr();
            auto value = string(lat_pos, str - 1);
            if (cnt != 1) {
                printf("%d %d:wrong charactor format", line, column);
            }
            tokenList.push_back(Token{CHARACTOR, "charactor", value, line, column});
        } else if (token == '\"') { // string
            lat_pos = str;
            while (*str) {
                if (*str == '\"' && token != '\\') {
                    break;
                }
                token = *str;
                nextStr();
            }
            nextStr();
            auto value = string(lat_pos, str - 1);
            tokenList.push_back(Token{STRING, "string", value, line, column});
        } else if (token == '+') {
            tokenList.push_back(Token{ADD, "+", "", line, column});
        } else if (token == '-') {
            tokenList.push_back(Token{SUB, "-", "", line, column});
        } else if (token == '*') {
            tokenList.push_back(Token{MUL, "*", "", line, column});
        } else if (token == '<') {
            if (*str == '=') {
                nextStr();
                tokenList.push_back(Token{LTE, "<=", "", line, column});
            } else tokenList.push_back(Token{LT, "<", "", line, column});
        } else if (token == '>') {
            if (*str == '=') {
                nextStr();
                tokenList.push_back(Token{GTE, ">=", "", line, column});
            } else tokenList.push_back(Token{GT, ">", "", line, column});
        } else if (token == '=') {
            if (*str == '=') {
                nextStr();
                tokenList.push_back(Token{EQ, "==", "", line, column});
            } else tokenList.push_back(Token{ASSIGN, "=", "", line, column});
        } else if (token == '!') {
            if (*str == '=') {
                nextStr();
                tokenList.push_back(Token{NE, "!=", "", line, column});
            } else tokenList.push_back(Token{NOT, "!", "", line, column});
        } else if (token == '&') {
            if (*str == '&') {
                nextStr();
                tokenList.push_back(Token{AND, "&&", "", line, column});
            } else tokenList.push_back(Token{LAND, "&", "", line, column});
        } else if (token == '|') {
            if (*str == '|') {
                nextStr();
                tokenList.push_back(Token{OR, "||", "", line, column});
            } else tokenList.push_back(Token{LOR, "|", "", line, column});
        } else if (token == '^') {
            tokenList.push_back(Token{XOR, "^", "", line, column});
        } else if (token == ',') {
            tokenList.push_back(Token{COMMA, ",", "", line, column});
        } else if (token == '.') {
            tokenList.push_back(Token{POI, ".", "", line, column});
        } else if (token == ';') {
            tokenList.push_back(Token{SEMI, ";", "", line, column});
        } else if (token == '(') {
            tokenList.push_back(Token{LP, "(", "", line, column});
        } else if (token == ')') {
            tokenList.push_back(Token{RP, ")", "", line, column});
        } else if (token == '{') {
            tokenList.push_back(Token{LC, "{", "", line, column});
        } else if (token == '}') {
            tokenList.push_back(Token{RC, "}", "", line, column});
        } else if (token == '[') {
            tokenList.push_back(Token{LB, "[", "", line, column});
        } else if (token == ']') {
            tokenList.push_back(Token{RB, "]", "", line, column});
        } else if (token == '/') {
            tokenList.push_back(Token{SLA, "/", "", line, column});
        } else if (token == '\\') {
            tokenList.push_back(Token{BSLA, "\\", "", line, column});
        } else if (token == '#') {
            tokenList.push_back(Token{SIGN, "$", "", line, column});
        } else {
            //do nothing now
            //maybe uncognized charactor or split charactor
        }
    }
    return 0;
}