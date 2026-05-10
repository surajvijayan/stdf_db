/*
 * Copyright 2020 Suraj Vijayan
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
