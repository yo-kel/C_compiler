#include<stdio.h>
#include "lexer.h"
#include "parser.h"
#include <fstream>
#include <streambuf>
#include <sstream>
#include <iostream>

using namespace std;

string readFileIntoString(const string &path) {
    ifstream input_file(path);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"
             << path << "'" << endl;
        exit(EXIT_FAILURE);
    }
    return string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
}

int main(int argc, char **argv) {
    string filename("1.cpp");
    string file_contents;
    file_contents = readFileIntoString(filename);
    cout << file_contents << endl;
    //std::ifstream t(argv[0]);
    file_contents = file_contents + '\0';
    lexer::init(&file_contents);
    lexer::ParseToken();
//    lexer::outPrintTokenList();
    parser::init(&lexer::tokenList);
    auto entry=parser::program();


    //std::string s = "((hi there))";
    //auto lp = s.begin() ;
    //auto rp = s.end();
    //lp++;lp += 2;
    //std::cout << std::string(lp, rp) << "\n";
    //int cnt = lexer::STATIC;
    //printf("%d\n",cnt);
    //std::ifstream t("file.txt");
    //std::stringstream buffer;
    //buffer << t.rdbuf();
    exit(EXIT_SUCCESS);
    return 0;
}