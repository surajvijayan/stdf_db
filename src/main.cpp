
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

#include <libconfig.h++>
using namespace libconfig;
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;
#include <active_parts.h>
#include <bits/stdc++.h>
#include <libstdf.h>
#include <math.h>
#include <pthread.h>
#include <stdf_cond.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
/*
 *  Suraj Vijayan
 *  Feb. 15th. 2020. Initial release.
 *  Update: Sept. 25t. 2023. Integration DTR COND tracking
 *
 */
/************************************************************************************************/
extern pthread_mutex_t stdf_lock1;

int main(int argc, char *argv[])
{
    THREAD_PROCESS main_thread;
    ACTIVE_PARTS *ap = active_parts::Instance();
    Config cfg;
    stdf_file *f;
    map<string, int>::iterator m_itr;
    map<int, THREAD_PROCESS>::iterator itr;
    list<string> filter_parametric_tests;
    ifstream ifile;
    void *status;
    unsigned int rc, thread_no;
    ostringstream oss;
    try
    {
        // Read the file. If there is an error, report it and exit.
        if (argc <= 2)
        {
            cerr << "Usage: stdf_db <cfg_file> <STDF_FILE to process>" << endl;
            exit(1);
        }
        ap->threads_ready = true;
        ap->update_rec_info();
        cfg.readFile(argv[1]);
        // stdf_open() check is not reliable..
        ifstream ifile(argv[2]);
        if (!ifile)
            throw FileIOException();
        else
            ifile.close();
        const Setting &root             = cfg.getRoot();
        const Setting &stdf_db_config   = root["stdf_db_config"]["mysql"];
        const Setting &test_rec_config  = root["stdf_db_config"]["test_rec"];
        const int site_id               = root.lookup("stdf_db_config.site_id");
        const int verbose               = root.lookup("stdf_db_config.verbose");
        const int use_threads           = (int)root.lookup("stdf_db_config.use_threads");
        const int validate_stdf         = (int)root.lookup("stdf_db_config.validate_stdf");
        const int max_parts_per_thread  = (int)root.lookup("stdf_db_config.max_parts_per_thread");
        const int max_threads_limit     = root.exists("stdf_db_config.max_threads_limit") ? (int)root.lookup("stdf_db_config.max_threads_limit") : 32;
        ap->site_id                     = site_id;
        ap->verbose                     = verbose;
        ap->max_threads_limit           = max_threads_limit;
        ap->set_mysql_server(stdf_db_config.lookup("mysql_server").c_str());

        // Read credentials from environment variables for security (override config)
        const char *env_user = getenv("STDF_DB_USER");
        const char *env_pass = getenv("STDF_DB_PASS");
        const char *env_db   = getenv("STDF_DB_NAME");
        const char *env_host = getenv("STDF_DB_HOST");

        if (env_host)
            ap->set_mysql_server(env_host);
        if (env_user)
            ap->set_mysql_user(env_user);
        else
            ap->set_mysql_user(stdf_db_config.lookup("mysql_user").c_str());
        if (env_pass)
            ap->set_mysql_pass(env_pass);
        else
            ap->set_mysql_pass(stdf_db_config.lookup("mysql_pass").c_str());
        if (env_db)
            ap->set_mysql_db(env_db);
        else
            ap->set_mysql_db(stdf_db_config.lookup("mysql_db").c_str());
        ap->set_pass_ignore_ptr((int)test_rec_config.lookup("ignore_passed_ptr"));
        ap->set_pass_ignore_ftr((int)test_rec_config.lookup("ignore_passed_ftr"));
        f = stdf_open(argv[2]);
        if (!f || f == NULL)
            throw FileIOException();
        // Check file format immediately after opening to isolate potential issues
        if (f->file_format != STDF_FORMAT_REG) {
            cerr << "Error: STDF file format is not REG. Only REG format is supported." << endl;
            stdf_close(f);
            exit(1);
        }
        for (auto itr = test_rec_config.lookup("skip_rec_types").begin();
             itr != test_rec_config.lookup("skip_rec_types").end(); itr++)
        {
            m_itr = ap->all_rec_info.find(string(itr->c_str()));
            if (m_itr != ap->all_rec_info.end())
                ap->skip_rec_info.push_back(m_itr->second);
        }
        ap->update_skip_table();
        // filter_parametric_tests token lists of all possbile valid parametrtic tests. If empty, then all
        // parametric_tests in the PTR records are valid
        for (auto itr = test_rec_config.lookup("filter_parametric_tests").begin();
             itr != test_rec_config.lookup("filter_parametric_tests").end(); itr++)
        {
            filter_parametric_tests.push_back(itr->c_str());
        }
        main_thread.skip_rec_info           = ap->skip_rec_info;
        main_thread.filter_parametric_tests = filter_parametric_tests;
        main_thread.f                       = f;
        main_thread.thread_num              = 0; // Master Thread is 0
        main_thread.verbose                 = verbose;
        main_thread.stdf_thread_offset      = 0;
        // Connect to DB after file format check
        main_thread.connect_db();
        if (validate_stdf)
            ap->validate_stdf(argv[2]);

        ap->max_parts_per_thread = (max_parts_per_thread > 0) ? max_parts_per_thread : 500;
        ap->max_threads_limit     = (max_threads_limit > 0) ? max_threads_limit : 32;
            
        if (f->file_format != STDF_FORMAT_REG || use_threads == 0)
        {
            main_thread.read_stdf(3);
            ap->print_thread_info();
            if (f != NULL)
                stdf_close(f);
            oss.str("");
            oss.clear();
            oss << "Main thread finished. STDF_ID:" << ap->get_stdf_id();
            main_thread.update_stdf_file(argv[2], ap->get_stdf_id());
            main_thread.log(1, oss.str());
            main_thread.con->close();
            exit(0);
        }
        main_thread.f = f;
        // main thread will process all the initial STDF records
        // upto the first PIR record. Threads will be dispatching records
        // starting with different PIR start positions
        if ((main_thread.read_stdf(2)) == 10)
        {
            main_thread.con->close();
            if (f != NULL)
                stdf_close(f);
            exit(0);
        }
        // Commit the MIR and header metadata so worker threads can reference the stdf_id
        main_thread.con->commit();

        if (ap->get_stdf_id() == 0)
        {
            cerr << "Error: MIR record not found or DB update failed. Aborting." << endl;
            exit(1);
        }

        ap->print_thread_info();
        if (f != NULL)
            stdf_close(f);
        ap->threads_ready = false;
        if (!ap->spawn_threads(argv[2], ap->skip_rec_info, filter_parametric_tests))
            throw FileIOException();

        for (itr = ap->stdf_threads.begin(), thread_no = 0;
             thread_no < ap->stdf_threads.size() && itr != ap->stdf_threads.end(); itr++, thread_no++)
        {
            rc = pthread_join(itr->second.thread_id, &status);
            if (rc)
            {
                cerr << "Error:unable to join thread " << itr->second.thread_num << ", error code: " << rc << endl;
            }
            else
            {
                oss.str("");
                oss.clear();
                oss << "Completed thread id :" << itr->second.thread_num << ".Committed DB transaction.";
                oss << "Exiting with status :" << status;
                main_thread.log(1, oss.str());
            }
        }
        oss.str("");
        oss.clear();
        oss << "Main thread finished. STDF_ID:" << ap->get_stdf_id();
        main_thread.update_stdf_file(argv[2], ap->get_stdf_id());
        if (main_thread.con != NULL)
        {
            try
            {
                main_thread.con->commit();
            }
            catch (...)
            {
                // Ignore commit errors, will close anyway
            }
            main_thread.con->close();
            delete main_thread.con;
            main_thread.con = NULL;
        }
        if (main_thread.driver != NULL)
        {
            main_thread.driver->threadEnd();
        }
        main_thread.log(1, oss.str());
        ap->print_thread_info();
        exit(0);
    }
    catch (const SettingNotFoundException &nfex)
    {
        cerr << "Setting not found." << endl;
        exit(1);
    }
    catch (const FileIOException &fioex)
    {
        cerr << "I/O error while reading file." << endl;
        if (main_thread.con != NULL)
        {
            try
            {
                main_thread.con->rollback();
            }
            catch (...)
            {
                // Ignore error
            }
            main_thread.con->close();
        }
        exit(1);
    }
    catch (const ParseException &pex)
    {
        cerr << "Configuration Parse Error in " << pex.getFile() << " at line " << pex.getLine()
             << ": " << pex.getError() << endl;
        if (main_thread.con != NULL)
        {
            try
            {
                main_thread.con->rollback();
            }
            catch (...)
            {
                // Ignore error
            }
            main_thread.con->close();
        }
        exit(1);
    }
    catch (sql::SQLException &e)
    {
        cerr << "# ERR0: SQLException in " << __FILE__;
        cerr << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
        cerr << "# ERR0: " << e.what();
        cerr << " (MySQL error code: " << e.getErrorCode();
        cerr << ", SQLState: " << e.getSQLState() << " )" << endl;
        exit(0);
    }
    // Catch the base standard exception for all other standard C++ exceptions
    catch (const std::exception &e)
    {
        std::cerr << "General Exception: " << e.what() << endl;
        exit(0);
    }
    // Catch any remaining exceptions (use as a last resort)
    catch (...)
    {
        std::cerr << "An unknown exception occurred." << endl;
        exit(0);
    }
}
/*****************************************************************************************************************/
