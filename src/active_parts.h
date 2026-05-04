#ifndef STDF_DB_H_
#define STDF_DB_H_
#include <algorithm>
#include <condition_variable>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
using namespace std;
// #include "mysql_connection.h"
#include <bits/stdc++.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <libstdf.h>
#include <pthread.h>
#include <stdf_cond.h>
#define INACTIVE 0
#define ACTIVE 1

#define FAIL 0
#define PASS 1
#define READ(x, y) ((0u == (x & (1 << y))) ? 0u : 1u)

typedef struct ftr_data
{
    string fail_pin;
    string vect_nam;
    string time_set;
    string op_code;
    string test_txt;
    string alarm_id;
    string prog_txt;
    string rslt_txt;
    string spin_map;
    bool alarm_err;
    bool timeout_err;
    bool abort_flag;
    bool test_exec_flag;
    bool test_status;
    rec_ftr ftr;
    // cond_group_id holds the most current value for the
    // thread processing STDF
    unsigned int cond_group_id;
} FTR_DATA;

typedef struct test_data
{
    string resfmt;
    double result;
    string test_txt;
    string units;
    string llmfmt;
    string hlmfmt;
    string alarm_id;
    double lo_limit;
    double hi_limit;
    double lo_spec;
    double hi_spec;
    int llm_scal;
    int hlm_scal;
    int res_scal;
    bool scale_err;
    bool draft_err;
    bool ocill_err;
    bool hivalue_err;
    bool lovalue_err;
    bool test_status;
    rec_ptr ptr;
    // cond_group_id holds the most current value for the
    // thread processing STDF
    unsigned int cond_group_id;
} TEST_DATA;

typedef struct part_info
{
    unsigned int head_num;
    unsigned int site_num;
    int state;
    vector<TEST_DATA> v_ptr;
    vector<FTR_DATA> f_ptr;
} PART_INFO;

typedef class thread_process
{
  public:
    thread_process(void);
    // Add MIR record
    int add_mir(rec_mir *, string);
    // Add SDR recor
    int add_sdr(rec_sdr *);
    // Add FAR recor
    int add_new_far(rec_far *);
    // Add PIR on head# and site#
    int add_new_pir(rec_pir *);
    // Add MRR record
    int add_mrr(rec_mrr *);
    // Add PCR record
    int add_pcr(rec_pcr *);
    // Add HBR record
    int add_hbr(rec_hbr *);
    // Add WIR record
    int add_wir(rec_wir *);
    // Add WCR record
    int add_wcr(rec_wcr *);
    // Add WRR record
    int add_wrr(rec_wrr *);
    // Add ATR record
    int add_new_atr(rec_atr *);
    // Add SBR record
    int add_sbr(rec_sbr *);
    // Add TSR record
    int add_tsr(rec_tsr *);
    // Add PMR record
    int add_pmr(rec_pmr *);
    // Add PGR record
    int add_pgr(rec_pgr *);
    // Add PLR record
    int add_plr(rec_plr *);
    // Add DTR record
    int add_dtr(rec_dtr *);
    // Add new test record for a part being currently tested to vector v_ptr in matching v_parts vector.
    // Update PTR table in DB.
    int add_new_ptr(rec_ptr *, const string &);
    int update_batch_pir();
    int update_pir();
    int update_batch_dtr();
    int update_ptr(vector<TEST_DATA> *, unsigned int, string);
    // Add new FTR record for a part being currently tested to vector f_ptr in matching v_parts vector.
    // Update FTR table in DB.
    int add_new_ftr(rec_ftr *);
    int update_ftr(vector<FTR_DATA> *, unsigned int);
    // Process prr record. Extract part_id from prr record, update DB PTR,PRR tables. Reset 'state' to 0
    // in PART_INFO v_parts.
    int process_prr_record(rec_prr *);
    int update_atr(void);
    int update_far(void);
    int read_stdf(int);
    int set_verbose(int);
    int log(int, string);
    int call_process_cond_group(int);
    int get_cond_group_id(int);
    int check_duplicate_stdf(rec_mir *, string);
    int set_ptr_limits(rec_ptr *);
    int cleanze_ptr_limits(void);
    int update_stdf_file(char *, unsigned int);
    pthread_t thread_id;
    int thread_num;
    list<int> skip_rec_info;
    list<string> filter_parametric_tests;
    stdf_file *f;
    int verbose;
    int connect_db(void);
    // private:
    class active_parts *ap;
    sql::Statement *stmt;
    sql::ResultSet *res;
    sql::Connection *con;
    sql::Driver *driver;
    off_t stdf_thread_offset;
    vector<PART_INFO> v_parts;
    vector<string> v_dtr;
    rec_ptr sav_ptr;
    // Map to store COND tokens for one thread.
    // This is a map of maps. One map of COND
    // tokens per site of the STDF PTR record.
    map<int, map<string, string>> cond_maps;
    // cond_group_id map will have the most current site_specific cond_group_id set in
    // MySQL DB for the current thread processing STDF records.
    // cond_group_ids_map stores: SITE_NUM:COND_GROUP_ID
    map<int, int> cond_group_ids_map;
    // creating a cache for cond_group_tokens to cond_group_id to avoid MySQL query to get
    // cond_group_id. This query does lot of processing, want to minimize it.
    unordered_map<string, int> cond_group_cache_map;
    unordered_map<unsigned int, TEST_DATA> test_limits_map;
    unsigned int atr_mod_tim;
    unsigned int far_cpu_type;
    unsigned int far_stdf_ver;
    string atr_cmd_line;
} THREAD_PROCESS;

typedef class active_parts
{
  public:
    static active_parts *Instance();
    int verbose;
    int site_id;
    unsigned int stdf_id;
    int max_parts_per_thread;
    int max_threads_limit;
    bool skip_table[65536];
    regex cond_pattern;
    list<int> skip_rec_info;
    int update_skip_table(void);
    int print_data(string);
    int print_thread_info(void);
    string extract_str(char *);
    string extract_str2(char *);
    int set_mysql_server(string);
    int set_mysql_user(string);
    int set_mysql_pass(string);
    int set_pass_ignore_ptr(int);
    int set_pass_ignore_ftr(int);
    int set_mysql_db(string);
    int set_stdf_id(int);
    string get_mysql_server(void);
    int get_stdf_id(void);
    string get_mysql_user(void);
    string get_mysql_pass(void);
    string get_mysql_db(void);
    int get_pass_ignore_ptr(void);
    int get_pass_ignore_ftr(void);
    int validate_stdf(char *);
    int spawn_threads(char *, list<int> &, list<string> &);
    int connect_db(void);
    int log(int, string);
    sql::Connection *con;
    map<string, int> all_rec_info;
    int update_rec_info(void);
    string sha256(const void *, int);
    string active_parts_version();

    sql::Driver *get_mysql_driver()
    {
        return driver;
    };
    // private:
    active_parts(void) {}; // Private so that it can  not be called
    active_parts(active_parts const &) {}; // copy constructor is private
    active_parts &operator=(active_parts const &)
    {
        return *this;
    }; // assignment operator is private
    static active_parts *m_pInstance;
    map<int, THREAD_PROCESS> stdf_threads;
    sql::Driver *driver;
    string mysql_server, mysql_user, mysql_db, mysql_pass;
    int pass_ignore_ftr, pass_ignore_ptr;
    // signaling threads
    bool threads_ready = false;
    mutex mtx;
    condition_variable cv;
} ACTIVE_PARTS;
#endif
