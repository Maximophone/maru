#ifndef REPL_H
#define REPL_H

#include <string>
#include <vector>

using namespace std;

const string Prompt = ">> ";

void print_parser_errors(vector<string>);
void start();

#endif