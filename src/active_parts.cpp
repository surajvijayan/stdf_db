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
#include <cstring>
#include <cmath>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
using namespace std;
#include <libconfig.h++>
using namespace libconfig;
#include <active_parts.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <libstdf.h>
#include <openssl/sha.h>
#include <pthread.h>
#include <stdf_cond.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef STDF_ENDIAN_LITTLE
#define STDF_ENDIAN_LITTLE 1
#endif

bool is_valid_number(const string &s);
void *launch_pthread(void *thread_inp);
extern pthread_mutex_t stdf_lock1;
ACTIVE_PARTS *active_parts::m_pInstance = NULL;
static pthread_mutex_t singleton_lock   = PTHREAD_MUTEX_INITIALIZER;

ACTIVE_PARTS *active_parts::Instance(void)
{
    // Double-checked locking pattern for thread-safe singleton
    if (!m_pInstance)
    {
        pthread_mutex_lock(&singleton_lock);
        try
        {
            if (!m_pInstance)
            {
                // Check again inside lock
                m_pInstance              = new active_parts;
                m_pInstance->max_threads_limit = 32;
                m_pInstance->set_stdf_id(0);
                memset(m_pInstance->skip_table, 0, sizeof(m_pInstance->skip_table));
                m_pInstance->cond_pattern = regex("^\\s*[COND:|COND_VAR:].*$");
            }
        }
        catch (...)
        {
            pthread_mutex_unlock(&singleton_lock);
            throw;
        }
        pthread_mutex_unlock(&singleton_lock);
    }
    return m_pInstance;
}
/******************************************************************************************************************/

int active_parts::log(int level, string msg)
{
    if (verbose >= level)
        cout << msg << endl;
    return (1);
}
/******************************************************************************************************************/

int active_parts::print_data(string src)
{
    map<int, THREAD_PROCESS>::iterator itr;
    vector<PART_INFO>::iterator itr2;
    vector<TEST_DATA>::iterator itr3;

    // loop over all threads..
    for (itr = stdf_threads.begin(); itr != stdf_threads.end(); ++itr)
    {
        // loop over all parts in a thread
        for (itr2 = itr->second.v_parts.begin(); itr2 != itr->second.v_parts.end(); ++itr2)
        {
            // loop over all PTRs in a part
            for (itr3 = itr2->v_ptr.begin(); itr3 != itr2->v_ptr.end(); ++itr3)
            {
                cout << "SOURCE : " << src;
                cout << " TEST_NUM: " << itr3->ptr.TEST_NUM << " RESFMT: " << itr3->resfmt;
                cout << " TEST_TXT: " << itr3->test_txt << endl;
            }
        }
    }
    return (1);
}
/******************************************************************************************************************/

int active_parts::print_thread_info(void)
{
    THREAD_PROCESS tp;
    ostringstream oss;
    map<int, THREAD_PROCESS>::iterator itr;

    for (itr = stdf_threads.begin(); itr != stdf_threads.end(); ++itr)
    {
        oss.str("");
        oss << "Thread Number: " << itr->first << " STDF offset: " << itr->second.stdf_thread_offset;
        tp.log(1, oss.str());
    }
    return (1);
}
/******************************************************************************************************************/

string active_parts::extract_str(char *in_str)
{
    if (in_str == NULL)
        return (string(" "));
    unsigned short len;
    string out_str;

    len = (unsigned short)*in_str;
    if (len > 0 && len < 255)
        return (string(in_str + 1, len));
    return (string(" "));
}
/******************************************************************************************************************/

string active_parts::extract_str2(char *in_str)
{
    if (in_str == NULL)
        return (string(" "));
    short len;
    string out_str;

    memcpy(&len, in_str, 2);
    if (len > 0 && len < 65535)
        return (string(in_str + 2, len));
    return (string(" "));
}
/******************************************************************************************************************/

int active_parts::set_mysql_server(string str)
{
    mysql_server = str;
    return (1);
}
/******************************************************************************************************************/

int active_parts::set_mysql_user(string str)
{
    mysql_user = str;
    return (1);
}
/******************************************************************************************************************/

int active_parts::set_mysql_pass(string str)
{
    mysql_pass = str;
    return (1);
}
/******************************************************************************************************************/

int active_parts::set_mysql_db(string str)
{
    mysql_db = str;
    return (1);
}
/******************************************************************************************************************/

int active_parts::set_pass_ignore_ftr(int val)
{
    pass_ignore_ftr = val;
    return (1);
}
/******************************************************************************************************************/

int active_parts::set_pass_ignore_ptr(int val)
{
    pass_ignore_ptr = val;
    return (1);
}
/******************************************************************************************************************/

string active_parts::get_mysql_server(void)
{
    return (mysql_server);
}
/******************************************************************************************************************/

string active_parts::get_mysql_user(void)
{
    return (mysql_user);
}
/******************************************************************************************************************/

string active_parts::get_mysql_pass(void)
{
    return (mysql_pass);
}
/******************************************************************************************************************/

string active_parts::get_mysql_db(void)
{
    return (mysql_db);
}
/******************************************************************************************************************/

int active_parts::get_pass_ignore_ptr(void)
{
    return (pass_ignore_ptr);
}
/******************************************************************************************************************/

int active_parts::get_pass_ignore_ftr(void)
{
    return (pass_ignore_ftr);
}
/******************************************************************************************************************/

int active_parts::set_stdf_id(int in_stdf_id)
{
    stdf_id = in_stdf_id;
    return (1);
}
/******************************************************************************************************************/

int active_parts::get_stdf_id(void)
{
    return (stdf_id);
}
/******************************************************************************************************************/

int active_parts::validate_stdf(char *file)
{
    rec_unknown *rec;
    rec_header prev_rec;
    stdf_file *f;
    long rec_mrr_cnt, rec_pcr_cnt, rec_hbr_cnt, rec_sbr_cnt, rec_wcr_cnt;

    cout << "Validating.." << endl;
    f = NULL;
    f = (stdf_file *)stdf_open(file);
    if (!f || f == NULL)
    {
        f = NULL;
        throw FileIOException();
    }
    /* Find the FAR record */
    rec = stdf_read_record(f);
    if (rec == NULL || HEAD_TO_REC(rec->header) != REC_FAR)
    {
        cout << "First record is not FAR!" << endl;
        return (0);
    }
    stdf_free_record(rec);
    /* Try to read all the ATR records (if they exist) */
    while ((rec = stdf_read_record(f)) != NULL)
    {
        if (HEAD_TO_REC(rec->header) != REC_ATR)
            break;
        else
            stdf_free_record(rec);
    }
    if (rec == NULL)
    {
        cout << "Initial sequence not found!" << endl;
        return (0);
    }
    /* We should now have the MIR record already read in */
    if (HEAD_TO_REC(rec->header) != REC_MIR)
    {
        cout << "Initial sequence wrong: MIR not located!" << endl;
        return (0);
    }
    /* Try to read the RDR record (if it exists) */
    stdf_free_record(rec);
    if ((rec = stdf_read_record(f)) == NULL)
    {
        cout << "EOF found after initial sequence!" << endl;
        return (0);
    }
    if (HEAD_TO_REC(rec->header) == REC_RDR)
    {
        stdf_free_record(rec);
        rec = stdf_read_record(f);
        if (rec == NULL)
        {
            cout << "EOF found after initial sequence!" << endl;
            return (0);
        }
    }
    /* Try to read the SDR records (if they exist) */
    while (HEAD_TO_REC(rec->header) == REC_SDR)
    {
        stdf_free_record(rec);
        rec = stdf_read_record(f);
        if (rec == NULL)
        {
            cout << "EOF found after initial sequence!" << endl;
            return (0);
        }
    }
    /* Now we read the rest of the file */
    rec_mrr_cnt = rec_pcr_cnt = rec_hbr_cnt = rec_sbr_cnt = rec_wcr_cnt = 0;
    while (1)
    {
        memcpy(&prev_rec, &rec->header, sizeof(rec_header));
        stdf_free_record(rec);
        rec = stdf_read_record(f);
        if (rec == NULL)
            break;
        switch (HEAD_TO_REC(rec->header))
        {
        case REC_FAR:
        case REC_ATR:
        case REC_MIR:
        case REC_RDR:
        case REC_SDR:
            printf("\tFound %s outside of initial sequence!\n",
                   stdf_get_rec_name(rec->header.REC_TYP, rec->header.REC_SUB));
            return (0);
        case REC_MRR:
            if (++rec_mrr_cnt > 1)
            {
                cout << "More than one REC_MRR was found!" << endl;
                return (0);
            }
            break;
        case REC_PCR:
            ++rec_pcr_cnt;
            break;
        case REC_HBR:
            ++rec_hbr_cnt;
            break;
        case REC_SBR:
            ++rec_sbr_cnt;
            break;

        /* need some logic with these ... */
        case REC_PMR:
            break;
        case REC_PGR:
            break;
        case REC_PLR:
            break;

        case REC_WIR:
            break; /* only 1 per wafer */
        case REC_WRR:
            break; /* only 1 per wafer */

        case REC_WCR:
            if (++rec_wcr_cnt > 1)
            {
                cout << "More than one REC_WCR was found!" << endl;
                return (0);
            }
            break;

        /* each PIR must have a PRR for same HEAD/SITE */
        /* PTR/MPR/FTR records must appear between the right PIR/PRR pairs */
        /* each BPS/EPS pair must be inside the PIR/PRR pair */
        case REC_PIR: /* only 1 per part tested */
            break;
        case REC_PTR:
            break; /* only 1 per part tested */
        case REC_MPR:
            break; /* only 1 per part tested */
        case REC_FTR:
            break; /* only 1 per part tested */
        case REC_BPS:
            break;
        case REC_EPS:
            break;
        case REC_PRR:
            break; /* only 1 per part tested */

        case REC_TSR:
            break;
        case REC_GDR:
            break;
        case REC_DTR:
            break;

        default:
            cout << "Uknown record found!" << endl;
            break;
        }
    }
    if (HEAD_TO_REC(prev_rec) != REC_MRR)
    {
        cout << "REC_MRR was not the last record in the stream!" << endl;
        return (0);
    }
    stdf_close(f);
    return (1);
}
/******************************************************************************************************************/

string active_parts::sha256(const void *data, int size)
{
    string sha256_digest;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char *)data, size, hash);

    stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << setw(2) << setfill('0') << hex << (int)hash[i];
    }
    sha256_digest = ss.str();
    return (sha256_digest);
}
/******************************************************************************************************************/

int active_parts::update_rec_info(void)
{
    all_rec_info["REC_ATR"] = REC_ATR;
    all_rec_info["REC_HBR"] = REC_HBR;
    all_rec_info["REC_SBR"] = REC_SBR;
    all_rec_info["REC_PMR"] = REC_PMR;
    all_rec_info["REC_PGR"] = REC_PGR;
    all_rec_info["REC_PLR"] = REC_PLR;
    all_rec_info["REC_RDR"] = REC_RDR;
    all_rec_info["REC_SDR"] = REC_SDR;
    all_rec_info["REC_WIR"] = REC_WIR;
    all_rec_info["REC_WRR"] = REC_WRR;
    all_rec_info["REC_WCR"] = REC_WCR;
    all_rec_info["REC_PIR"] = REC_PIR;
    all_rec_info["REC_PRR"] = REC_PRR;
    all_rec_info["REC_TSR"] = REC_TSR;
    all_rec_info["REC_PTR"] = REC_PTR;
    all_rec_info["REC_MPR"] = REC_MPR;
    all_rec_info["REC_FTR"] = REC_FTR;
    all_rec_info["REC_BPS"] = REC_BPS;
    all_rec_info["REC_EPS"] = REC_EPS;
    all_rec_info["REC_GDR"] = REC_GDR;
    all_rec_info["REC_DTR"] = REC_DTR;
    return (1);
}
/******************************************************************************************************************/

int active_parts::update_skip_table(void)
{
    // Pre-calculate skip table for O(1) lookups across all threads
    memset(skip_table, 0, sizeof(skip_table));
    for (int type : this->skip_rec_info)
    {
        if (type >= 0 && type < 65536)
            this->skip_table[type] = true;
    }
    return (1);
}
/******************************************************************************************************************/

int active_parts::connect_db(void)
{
    sql::Statement *stmt;
    sql::ConnectOptionsMap connection_properties;

    connection_properties["hostName"]          = get_mysql_server();
    connection_properties["userName"]          = get_mysql_user();
    connection_properties["password"]          = get_mysql_pass();
    connection_properties["schema"]            = get_mysql_db();
    connection_properties["port"]              = 3306;
    connection_properties["OPT_CHARSET_NAME"]  = "latin1";
    connection_properties["OPT_COLLATE"]       = "latin1_general_cs";
    connection_properties["OPT_READ_TIMEOUT"]  = 10;
    connection_properties["OPT_WRITE_TIMEOUT"] = 10;
    pthread_mutex_lock(&stdf_lock1);
    driver = get_driver_instance();
    pthread_mutex_unlock(&stdf_lock1);
    driver->threadInit();
    // con->setAutoCommit(0);
    /* Connect to the MySQL test database */
    // con->setSchema(get_mysql_db());
    con = driver->connect(connection_properties);
    con->setAutoCommit(0);
    // setting transaction isolation level to READ UNCOMMITED to improve write time
    // Set the transaction isolation level
    stmt = con->createStatement();
    // Sept. 15th. 2025. Setting TRANSACTION ISOLATION level to SESSION level
    stmt->execute("SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED");
    delete stmt;
    return (1);
}
/******************************************************************************************************************/

int active_parts::spawn_threads(char *file, list<int> &skip_rec_info, list<string> &filter_parametric_tests)
{
    rec_unknown *rec;
    stdf_file *f;
    int pir_count, thread_no;
    // flag to indicate whether the last processed rec was a PIR
    bool last_pir_flag;
    off_t prev_offset = 0;
    pthread_attr_t attr;
    ostringstream oss;
    // map to store COND tokens tokens info for each thread initialization
    map<int, map<string, string>> lcond_maps;
    smatch match;

    thread_no     = 1; // Worker threads start from 1
    pir_count     = 0;
    last_pir_flag = false;
    oss.str("");
    oss << "Thread Number:0(MAIN) indexing STDF..";
    log(1, oss.str());
    rec = NULL;
    f   = NULL;
    f   = (stdf_file *)stdf_open(file);
    if (!f || f == NULL)
        throw FileIOException();

    // World-class initialization: Let libstdf process the FAR record first 
    // to correctly identify endianness before we start the high-speed header scan.
    rec_unknown *far_rec = stdf_read_record(f);
    if (!far_rec) {
        stdf_close(f);
        throw FileIOException();
    }
    // Start manual scan from the position after FAR (Header[4] + Body)
    prev_offset = 4 + far_rec->header.REC_LEN;
    stdf_free_record(far_rec);

    if (this->get_stdf_id() == 0)
    {
        log(1, "Error: Master MIR record missing. Spawning aborted.");
        return (0);
    }

    // Initialize and set thread joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    lock_guard<mutex> lck(this->mtx);

    // Phase 1: Indexing Pass - Scan for PIR records to determine thread entry points.
    while ((rec = stdf_read_record(f)) != NULL)
    {
        uint32_t rec_type = HEAD_TO_REC(rec->header);

        if (rec_type == REC_DTR)
        {
            rec_dtr *dtr = (rec_dtr *)rec;
            if (dtr->TEXT_DAT != NULL && strstr(dtr->TEXT_DAT, "COND") != NULL)
            {
                string cond = string(dtr->TEXT_DAT);
                if (regex_search(cond, match, this->cond_pattern))
                    update_cond_map(cond, lcond_maps, thread_no);
            }
            last_pir_flag = false;
        }
        else if (rec_type == REC_PIR)
        {
            if (this->max_parts_per_thread > 0 && 
                pir_count % this->max_parts_per_thread == 0 && last_pir_flag == false)
            {
                if (thread_no >= max_threads_limit)
                {
                    log(1, "Warning: Reached max_threads_limit. Remainder of file will be handled by the last thread.");
                    stdf_free_record(rec);
                    break;
                }
                stdf_threads[thread_no].stdf_thread_offset      = prev_offset;
                stdf_threads[thread_no].thread_num              = thread_no;
                stdf_threads[thread_no].skip_rec_info           = skip_rec_info;
                stdf_threads[thread_no].filter_parametric_tests = filter_parametric_tests;
                stdf_threads[thread_no].verbose                 = verbose;
                stdf_threads[thread_no].cond_maps          = lcond_maps;
                thread_no++;
            } // Close the if (this->max_parts_per_thread > 0 && ...) block
            ++pir_count;
            oss.str("");
            oss << "Thread Number:0(MAIN) pir_count:" << pir_count << " Max. parts/thread:" << max_parts_per_thread;
            log(2, oss.str());
            last_pir_flag = true;
        }
        else
        {
            last_pir_flag = false;
        }
        
        // Robust Offset Tracking: libstdf handles endianness for REC_LEN.
        // The logical start of the NEXT record is current_start + 4 (header) + REC_LEN.
        uint32_t current_rec_size = 4 + rec->header.REC_LEN;
        stdf_free_record(rec);
        prev_offset += current_rec_size;
    }
    // core dump happening here...
    if (f != NULL)
    {
        stdf_close(f);
        f = NULL;
    }

    // Phase 2: Spawning Pass - Launch validated threads
    for (auto &it : stdf_threads)
    {
        it.second.f = (stdf_file *)stdf_open(file);
        if (!it.second.f)
            throw FileIOException();

        oss.str("");
        oss << "Thread Number:" << it.first << " being launched.STDF offset:" << it.second.stdf_thread_offset;
        log(1, oss.str());

        if ((pthread_create(&it.second.thread_id, &attr, launch_pthread, (void *)&it.second)) != 0)
        {
            cerr << "Critical Error: Unable to create worker thread " << it.first << endl;
        }
    }

    this->threads_ready = true;
    cv.notify_all(); // signal all threads
    return (1);
}
/******************************************************************************************************************/
