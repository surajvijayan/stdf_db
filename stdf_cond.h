#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

string ltrim(const string &);
string rtrim(const string &);
string trim(const string &);
void split_str(string const &, const char, vector<string> &);
int add_cond_map(string, map<string, string> &);
int update_cond_map(string, map<int, map<string, string>> &, int);
string get_site_cond_tokens(int, map<int, map<string, string>>);
