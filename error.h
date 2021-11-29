//
// Created by xsj on 2021/11/13.
//

#ifndef C_COMPILER_ERROR_H
#define C_COMPILER_ERROR_H


#include <string>
#include <iostream>
using namespace std;
void outError(string error);

namespace error{
    extern string EqualLeftVar;
    extern string TypeError;
    extern string NoDeclError;
    extern string IDNotFuncError;
    extern string RepeatDeclError;
}

#endif //C_COMPILER_ERROR_H
