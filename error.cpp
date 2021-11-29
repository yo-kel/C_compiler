//
// Created by xsj on 2021/11/13.
//
#include "error.h"

using namespace std;

void outError(string error) {
    cout << error << endl;
    exit(1);
}

namespace error {
    string EqualLeftVar = "= left should be variable";
    string TypeError = "type error";
    string NoDeclError = "ID not declared";
    string IDNotFuncError = "ID is not function";
    string RepeatDeclError="ID declaration repeat";
}
