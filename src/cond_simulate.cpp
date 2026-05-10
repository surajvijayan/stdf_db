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

#include <algorithm>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <vector>
using namespace std;
#include "stdf_cond.h"
#include <cppconn/prepared_statement.h>
#include <mysql/mysql.h>
/*
    Sept. 22nd. 2023
    Suraj Vijayan
    This program is the simulator/model to create COND lines parsing from STDF DTR record.
    This program should be used to incorporate any additional COND parsing requirements.
*/
int main()
{
    regex pattern("^\\s*[COND:|COND_VAR:].*$");
    smatch match;
    map<int, map<string, string>> cond_maps;

    // COND are logged in multiple ways:
    // 1) global wtth 'COND:', these tokens will apply to all SITEs
    // 2) Logged with 'COND_VAR: SITE[0]
    //    In cases where COND line has SITE info logged, then the COND tokens apply only to that SITE number
    // 3) Logged with 'COND: SITE=[7]
    // 3) Logged with 'COND: SITE[7]
    string cond_lines[] = {
        //    "COND:ZDDC = 1.070V, VDDO = 3.531V , DDR_VDDC = 1.070V , DDR_VDDO = 1.444V , VDDC = 1.926V  VDD at
        //    107.00%",
        //    "COND:VDDC = 4.070V, VDDO = 3.531V , DDR_VDDC = 1.070D , DDR_VDDO = 1.444V , VDDC = 1.926V",
        //    "COND:DDR_VDDO = 1101V, VDD0 = 3.531V , DDR_VDDC = 1.170V , DD1_VDDO = 1.44V , ADDC = 1.116V",
        //    "COND_VAR: SITE[0], TEST1 = RAJA",
        //    "COND_VAR: SITE[0], TEST=SURAJ"
        "COND: SITE[5], VDDP=0.745, OTP_VDDO=1.800", "COND_VAR: TPA_AVS[2] = 0.855V"
        /*
        site_pattern4("^\\s*COND:\\s+SITE\\s*\\[(\\d
        {
        1,})\\]\\s*,\\s+(.*)$");
            "COND_VAR: SITE[2], VDDC=0.796, OTP_VDDO=1.800",
            "COND_VAR: SITE[2], VDDC=0.760, OTP_VDDO=1.800",
            "COND_VAR: SITE[4], VDDC=0.755, OTP_VDDO=1.800",
            "COND_VAR: SITE[5], VDDC=0.762, OTP_VDDO=1.800",
            "COND_VAR: SITE[6], VDDC=0.762, OTP_VDDO=1.800",
            "COND: SITE=[7] , VDDC=0.752, OTP_VDDO=1.800",
            "COND_VAR: TPA_AVS[0]=0.932V"
            "COND_VAR: TPA_AVS[1]= 0.939V",
            "COND: VDDC = TPA_AVS, VDDO = 3.069V , DDR_VDDC = 0.930V , DDR_VDDO = 1.550V , VDDP = 1.674V VDD at 93.00%"
        */
    };
    // COND: SITE=[0], x=9
    // Accumulate all COND logged DTR reconds until a PTR record is found
    for (unsigned int i = 0; i < sizeof(cond_lines) / sizeof(cond_lines[0]); i++)
    {
        // check whether a DTR record has COND logging
        if (regex_search(cond_lines[i], match, pattern))
            update_cond_map(cond_lines[i], cond_maps, 1);
    }
    for (const auto &tmap : cond_maps)
    {
        string cond_data;
        // if(tmap.first == 2)
        //     cond_data = get_site_cond_tokens(201,cond_maps);
        // else
        cond_data = get_site_cond_tokens(tmap.first, cond_maps);
        if (!cond_data.empty())
            cout << "SITE: " << tmap.first << " DATA:" << cond_data << endl;
        else
            cout << "SITE: " << tmap.first << " DATA: key does not exist!" << endl;
    }
}
/**********************************************************************************************/