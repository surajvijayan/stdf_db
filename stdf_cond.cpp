#include <iostream>  
#include <regex>
#include <iostream>
#include <algorithm> 
#include <map>
#include <vector>
#include <sstream>
using namespace std;
#include <active_parts.h>
#include <stdf_cond.h>
/*
	Sept. 22nd. 2023
	Suraj Vijayan
	This program is the simulator/model to create COND lines parsing from STDF DTR record.
	This program should be used to incorporate any additional COND parsing requirements.
*/
int update_cond_map(string cond_line,map<int,map <string,string> > &cond_maps,int thread_no)
{
	//COND are logged in multiple ways: 
	//1) global wtth 'COND:', these tokens will apply to all SITEs
	//In cases where COND line has SITE info logged, then the COND tokens apply only to that SITE number
	regex pattern("(^[\\s]*COND:)(.*)$");
	//2) Logged with 'COND: SITE[0]
    regex site_pattern("^\\s*COND:\\s+SITE\\s*\\[(\\d{1,})\\]\\s*,\\s*(.*)$");
	//3) Logged with 'COND: SITE=[7] 
    regex site_pattern2("^\\s*COND:\\s+SITE\\s*=\\s*\\[(\\d{1,})\\]\\s*,\\s*(.*)$");
	//4) Logged with 'COND_VAR: TPA_AVS[0] = 0.932V'
    regex site_pattern3("^\\s*COND_VAR:\\s*(.*)\\s*\\[(\\d{1,})\\]\\s*=\\s*(.*)$");
	//2) Logged with 'COND_VAR: SITE[0]
    regex site_pattern4("^\\s*COND_VAR:\\s*SITE\\s*\\[(\\d{1,})\\]\\s*,\\s*(.*)$");
	#ifndef TEST
	class active_parts *ap = active_parts::Instance();
	#endif
	map<string,string> cond_map;
	int site_no;
	stringstream ss;
	ostringstream oss;
	smatch site_match;
	ss.str("");
	ss.clear();
	//First look for site specific COND DTR logging
	if(regex_search(cond_line,site_match,site_pattern) ||
	   regex_search(cond_line,site_match,site_pattern4)||
	   regex_search(cond_line,site_match,site_pattern2))
	{
		ss << site_match[1];
		ss >> site_no;
		//Adding COND tokens to site specific maps
		cond_map.clear();
		//Creating an empty COND map with the cond_maps vector
		if(cond_maps.count(site_no) == 0)
			cond_maps[site_no] = cond_map;
		oss.str("");
       	oss.clear();
       	oss << "Thread#:" << thread_no << " SITE_NUM:" << site_no << " COND1:" << site_match[2];
		#ifdef TEST
			cout << oss.str() << endl;
		#else
       		ap->log(2,oss.str());
		#endif
		add_cond_map(site_match[2],cond_maps[site_no]);
		return(site_no);
	}
	else
	if(regex_search(cond_line,site_match,site_pattern3))
	{
		ss << site_match[2];
		ss >> site_no;

		ss.str("");
       	ss.clear();
		ss << site_match[1] << "=" << site_match[3];
		cond_map.clear();
		//NEW map key should be created only when it is not already
		//Creating an empty COND map with the cond_maps vector
		if(cond_maps.count(site_no) == 0)
			cond_maps[site_no] = cond_map;
		oss.str("");
       	oss.clear();
       	oss << "Thread#:" << thread_no << " SITE_NUM:" << site_no << " COND2:" << ss.str();
		#ifdef TEST
			cout << oss.str() << endl;
		#else
       		ap->log(2,oss.str());
		#endif
		add_cond_map(ss.str(),cond_maps[site_no]);
		return(site_no);
	}
	else //No match for site specific COND logging, falling back to global (all sites) 'COND:'
	if(regex_search(cond_line,site_match,pattern))
	{
		site_no = 9999;
		cond_map.clear();
		//Creating an empty COND map with the cond_maps vector
		//Initializing a cond_map in the cond_maps with key 9999.
		//9999 as site_no indicates all sites, since no site_no
		//is configured with the COND DTR record
		if(cond_maps.count(site_no) == 0)
			cond_maps[site_no] = cond_map;
		//Updating the cond_map within the site specfic or ALL vector
		oss.str("");
       	oss.clear();
       	oss << "Thread#:" << thread_no << " SITE_NUM:" << site_no << " COND:" << site_match[2];
		#ifdef TEST
			cout << oss.str() << endl;
		#else
       		ap->log(2,oss.str());
		#endif
		add_cond_map(site_match[2],cond_maps[site_no]);
		return(site_no);
	}
	return(-1);
}
/**********************************************************************************************/

string get_site_cond_tokens(int site_no,map<int,map <string,string> > cond_maps)
{
	bool first;
	vector<string> test_conditions;
	map<string,string> tmap,new_cond_map;
	stringstream ss;
	smatch site_match;

	if(cond_maps.count(site_no) == 0)  
		site_no = 9999;
	first = true;
	ss.str("");
	ss.clear();
	new_cond_map.clear();
	test_conditions.clear();
	//cond_map valueswith key: 9999 indicates global COND values,
	//these are COND tokens logged in STDF DTR without any specific site_no information
	new_cond_map = cond_maps[9999];
	tmap = cond_maps[site_no];
	for(auto& it : tmap)
		new_cond_map[it.first] = it.second;

	//create a vector out of global COND map
	for(const auto &s : new_cond_map)
		test_conditions.push_back(s.first + '=' + s.second);
	//Need to sort the condition tokens so that unique index of a cond_group_name can be build, just
	//based off the cond_names,irrespective of the order the cond_names many appear within the STDF.
	sort(test_conditions.begin(),test_conditions.end());
	for(auto itr = test_conditions.begin();itr != test_conditions.end();++itr)
	{
		if(first)
			ss << *itr;
		else
		ss << ',' + *itr;
		first = false;
	}
	return(ss.str());
}
/**********************************************************************************************/
string trim(const string &s)
{
    size_t first = s.find_first_not_of(" \t\n\r");
    if (string::npos == first)
    {
        return "";
    }
    size_t last = s.find_last_not_of(" \t\n\r");
    return s.substr(first, (last - first + 1));
}
/*********************************************************************************/
  
void split_str(string const &str,const char delim,vector <string> &out)  
{  
	// create a stream from the string  
    stringstream s(str);  
    string s2;  
    while ( getline (s, s2, delim) )  
    {  
        out.push_back(s2); // store the string in s2  
    }  
}  
/*********************************************************************************/

int add_cond_map(string cond,map<string,string> &cond_map)
{
    regex pattern("[\\s]*([\\S]+)[\\s]*(=|at)[\\s]*([\\S]+).*$");
    vector <string> out;
    split_str(cond,',',out);
    for (unsigned int i=0; i < out.size(); i++)
    {
        smatch match;
        if(regex_search(out[i],match,pattern))
		{
            cond_map[match[1]] = match[3];		
		}
    }
    return(1);
}
/**********************************************************************************************/
