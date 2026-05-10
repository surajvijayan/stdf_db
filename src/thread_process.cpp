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
#include <cmath>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <mutex>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
using namespace std;
#include <libconfig.h++>
using namespace libconfig;
#include <active_parts.h>
#include <cppconn/connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <libstdf.h>
#include <openssl/sha.h>
#include <stdf_cond.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

// #include "mysql_connection.h"
extern pthread_mutex_t stdf_lock1;
thread_process::thread_process(void)
{
    v_parts = {};
    ap      = active_parts::Instance();
    f       = NULL;
}
/**********************************************************************************/

int thread_process::add_new_pir(rec_pir *pir)
{
    bool found;
    vector<PART_INFO>::iterator itr;
    PART_INFO part_info;
    found = false;
    sql::PreparedStatement *pstmt;
    for (itr = v_parts.begin(); itr != v_parts.end(); ++itr)
    {
        if (itr->head_num == pir->HEAD_NUM && itr->site_num == pir->SITE_NUM)
        {
            itr->state = ACTIVE;
            found      = true;
            itr->v_ptr.clear();
            itr->f_ptr.clear();
            break;
        }
    }
    if (found == false)
    {
        part_info.head_num = pir->HEAD_NUM;
        part_info.site_num = pir->SITE_NUM;
        part_info.state    = ACTIVE;
        v_parts.push_back(part_info);
    }
    // March 24th. 2026.
    pstmt = con->prepareStatement("INSERT INTO pir(stdf_id,head_num,site_num) VALUES(?,?,?)");
    pstmt->setUInt(1, ap->get_stdf_id());
    pstmt->setUInt(2, pir->HEAD_NUM);
    pstmt->setUInt(3, pir->SITE_NUM);
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;
    return (1);
}
/**********************************************************************************/

int thread_process::get_cond_group_id(int site_no)
{
    // get cond_group_id for a specific site_no
    if (cond_group_ids_map.count(site_no) == 0)
    {
        if (cond_group_ids_map.count(9999) != 0)
            return (cond_group_ids_map[9999]);
        else
            return (0);
    }
    else
        return (cond_group_ids_map[site_no]);
}
/**********************************************************************************/

int thread_process::check_duplicate_stdf(rec_mir *mir, string hash)
{
    int stdf_id;
    sql::PreparedStatement *pstmt;
    stdf_id = 0;
    pstmt   = con->prepareStatement("SELECT id FROM mir WHERE uniq_hash=?");
    pstmt->setString(1, hash);
    res = pstmt->executeQuery();
    while (res->next())
    {
        stdf_id = res->getInt(1);
    }
    delete res;
    pstmt->close();
    delete pstmt;
    return (stdf_id);
}
/**********************************************************************************/

int thread_process::add_mir(rec_mir *mir, string hash)
{
    vector<PART_INFO>::iterator itr;
    sql::PreparedStatement *pstmt;

    sql::SQLString qry = "INSERT INTO mir(setup_t,start_t,stat_num,mode_cod,rtst_cod,prot_cod,burn_tim,cmod_cod,";
    qry += "lot_id,part_typ,node_nam,tstr_typ,job_nam,job_rev,sblot_id, oper_nam, exec_typ,test_cod,";
    qry += "tst_temp,user_txt,aux_file,pkg_typ,family_id,date_cod,facil_id,floor_id,proc_id,oper_frq,";
    qry += "spec_nam,spec_ver,flow_id,setup_id,dsgn_rev,eng_id,rom_cod,serl_num,supr_nam,uniq_hash,site_id) ";
    qry += "VALUES(FROM_UNIXTIME(?),FROM_UNIXTIME(?),?,CHAR(?),CHAR(?),CHAR(?),?,CHAR(?),?,?,?,?,?,?,?,";
    qry += "?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";

    pstmt = con->prepareStatement(qry);
    pstmt->setUInt(1, mir->SETUP_T);
    pstmt->setUInt(2, mir->START_T);
    pstmt->setUInt(3, mir->STAT_NUM);
    pstmt->setString(4, to_string(mir->MODE_COD));
    pstmt->setString(5, to_string(mir->RTST_COD));
    pstmt->setString(6, to_string(mir->PROT_COD));
    pstmt->setUInt(7, mir->BURN_TIM);
    pstmt->setString(8, to_string(mir->CMOD_COD));
    pstmt->setString(9, ap->extract_str(mir->LOT_ID));
    pstmt->setString(10, ap->extract_str(mir->PART_TYP));
    pstmt->setString(11, ap->extract_str(mir->NODE_NAM));
    pstmt->setString(12, ap->extract_str(mir->TSTR_TYP));
    pstmt->setString(13, ap->extract_str(mir->JOB_NAM));
    pstmt->setString(14, ap->extract_str(mir->JOB_REV));
    pstmt->setString(15, ap->extract_str(mir->SBLOT_ID));
    pstmt->setString(16, ap->extract_str(mir->OPER_NAM));
    pstmt->setString(17, ap->extract_str(mir->EXEC_TYP));
    pstmt->setString(18, ap->extract_str(mir->TEST_COD));
    pstmt->setString(19, ap->extract_str(mir->TST_TEMP));
    pstmt->setString(20, ap->extract_str(mir->USER_TXT));
    pstmt->setString(21, ap->extract_str(mir->AUX_FILE));
    pstmt->setString(22, ap->extract_str(mir->PKG_TYP));
    pstmt->setString(23, ap->extract_str(mir->FAMILY_ID));
    pstmt->setString(24, ap->extract_str(mir->DATE_COD));
    pstmt->setString(25, ap->extract_str(mir->FACIL_ID));
    pstmt->setString(26, ap->extract_str(mir->FLOOR_ID));
    pstmt->setString(27, ap->extract_str(mir->PROC_ID));
    pstmt->setString(28, ap->extract_str(mir->OPER_FRQ));
    pstmt->setString(29, ap->extract_str(mir->SPEC_NAM));
    pstmt->setString(30, ap->extract_str(mir->SPEC_VER));
    pstmt->setString(31, ap->extract_str(mir->FLOW_ID));
    pstmt->setString(32, ap->extract_str(mir->SETUP_ID));
    pstmt->setString(33, ap->extract_str(mir->DSGN_REV));
    pstmt->setString(34, ap->extract_str(mir->ENG_ID));
    pstmt->setString(35, ap->extract_str(mir->ROM_COD));
    pstmt->setString(36, ap->extract_str(mir->SERL_NUM));
    pstmt->setString(37, ap->extract_str(mir->SUPR_NAM));
    pstmt->setString(38, hash);
    pstmt->setUInt(39, ap->site_id);
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;

    // Use LAST_INSERT_ID() for thread-safe session-local ID retrieval
    sql::Statement *id_stmt = con->createStatement();
    sql::ResultSet *id_res = id_stmt->executeQuery("SELECT LAST_INSERT_ID()");
    if (id_res->next())
    {
        ap->set_stdf_id(id_res->getUInt(1));
    }
    delete id_res;
    delete id_stmt;
    return (1);
}
/*********************************************************************************/

int thread_process::add_mrr(rec_mrr *mrr)
{
    vector<PART_INFO>::iterator itr;
    sql::PreparedStatement *pstmt;
    char str[2];
    sql::SQLString qry = "INSERT INTO mrr(stdf_id,finish_t,disp_cod,usr_desc,exc_desc)";
    qry += "VALUES(?,FROM_UNIXTIME(?),?,?,?)";
    pstmt = con->prepareStatement(qry);
    pstmt->setUInt(1, ap->get_stdf_id());
    pstmt->setUInt(2, mrr->FINISH_T);
    str[0] = (char)mrr->DISP_COD;
    str[1] = 0;
    pstmt->setString(3, string(str));
    pstmt->setString(4, ap->extract_str(mrr->USR_DESC));
    pstmt->setString(5, ap->extract_str(mrr->EXC_DESC));
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;
    return (1);
}
/*********************************************************************************/

int thread_process::add_pcr(rec_pcr *pcr)
{
    vector<PART_INFO>::iterator itr;
    sql::PreparedStatement *pstmt;

    sql::SQLString qry = "INSERT INTO pcr(stdf_id,head_num,site_num,part_cnt,rtst_cnt,abrt_cnt,good_cnt,func_cnt)";
    qry += "VALUES(?,?,?,?,?,?,?,?)";
    pstmt = con->prepareStatement(qry);
    pstmt->setUInt(1, ap->get_stdf_id());
    pstmt->setUInt(2, pcr->HEAD_NUM);
    pstmt->setUInt(3, pcr->SITE_NUM);
    pstmt->setUInt(4, pcr->PART_CNT);
    pstmt->setInt(5, ((pcr->RTST_CNT == 0xffffffff) ? -1 : pcr->RTST_CNT));
    pstmt->setInt(6, ((pcr->ABRT_CNT == 0xffffffff) ? -1 : pcr->ABRT_CNT));
    pstmt->setInt(7, ((pcr->GOOD_CNT == 0xffffffff) ? -1 : pcr->GOOD_CNT));
    pstmt->setInt(8, ((pcr->FUNC_CNT == 0xffffffff) ? -1 : pcr->FUNC_CNT));
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;
    return (1);
}
/*********************************************************************************/

int thread_process::update_stdf_file(char *stdf_file, unsigned int stdf_id)
{
    sql::PreparedStatement *pstmt;
    sql::SQLString qry = "UPDATE stdf_files SET stdf_file=? WHERE stdf_id=?";
    pstmt              = con->prepareStatement(qry);
    pstmt->setString(1, string(basename(stdf_file)));
    pstmt->setUInt(2, stdf_id);
    pstmt->executeUpdate();
    con->commit();
    pstmt->close();
    delete pstmt;
    return (1);
}
/*********************************************************************************/

int thread_process::add_wrr(rec_wrr *wrr)
{
    vector<PART_INFO>::iterator itr;
    sql::PreparedStatement *pstmt;

    sql::SQLString qry = "INSERT INTO wrr(stdf_id,head_num,site_grp,finish_t,part_cnt,rtst_cnt,abrt_cnt,";
    qry += "good_cnt,func_cnt,wafer_id,fabwf_id,frame_id,mask_id,usr_desc,exc_desc)";
    qry += "VALUES(?,?,?,FROM_UNIXTIME(?),?,?,?,?,?,?,?,?,?,?,?)";
    pstmt = con->prepareStatement(qry);
    pstmt->setUInt(1, ap->get_stdf_id());
    pstmt->setUInt(2, wrr->HEAD_NUM);
    pstmt->setUInt(3, wrr->SITE_GRP);
    pstmt->setUInt(4, wrr->FINISH_T);
    pstmt->setUInt(5, wrr->PART_CNT);
    pstmt->setInt(6, ((wrr->RTST_CNT == 0xffffffff) ? -1 : wrr->RTST_CNT));
    pstmt->setInt(7, ((wrr->ABRT_CNT == 0xffffffff) ? -1 : wrr->ABRT_CNT));
    pstmt->setInt(8, ((wrr->GOOD_CNT == 0xffffffff) ? -1 : wrr->GOOD_CNT));
    pstmt->setInt(9, ((wrr->FUNC_CNT == 0xffffffff) ? -1 : wrr->FUNC_CNT));
    pstmt->setString(10, ap->extract_str(wrr->WAFER_ID));
    pstmt->setString(11, ap->extract_str(wrr->FABWF_ID));
    pstmt->setString(12, ap->extract_str(wrr->FRAME_ID));
    pstmt->setString(13, ap->extract_str(wrr->MASK_ID));
    pstmt->setString(14, ap->extract_str(wrr->USR_DESC));
    pstmt->setString(15, ap->extract_str(wrr->EXC_DESC));
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;
    return (1);
}
/*********************************************************************************/

int thread_process::add_wir(rec_wir *wir)
{
    vector<PART_INFO>::iterator itr;
    sql::PreparedStatement *pstmt;

    sql::SQLString qry = "INSERT INTO wir(stdf_id,head_num,site_grp,start_t,wafer_id)";
    qry += "VALUES(?,?,?,FROM_UNIXTIME(?),?)";
    pstmt = con->prepareStatement(qry);
    pstmt->setUInt(1, ap->get_stdf_id());
    pstmt->setUInt(2, wir->HEAD_NUM);
    pstmt->setUInt(3, wir->SITE_GRP);
    pstmt->setUInt(4, wir->START_T);
    pstmt->setString(5, ap->extract_str(wir->WAFER_ID));
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;
    return (1);
}
/*********************************************************************************/

int thread_process::add_wcr(rec_wcr *wcr)
{
    vector<PART_INFO>::iterator itr;
    sql::PreparedStatement *pstmt;
    char str[2];

    sql::SQLString qry =
        "INSERT INTO wcr(stdf_id,wafr_siz,die_ht,die_wid,wf_units,wf_flat,center_x,center_y,pos_x,pos_y)";
    qry += "VALUES(?,?,?,?,?,?,?,?,?,?)";
    pstmt = con->prepareStatement(qry);
    pstmt->setUInt(1, ap->get_stdf_id());
    pstmt->setDouble(2, wcr->WAFR_SIZ);
    pstmt->setDouble(3, wcr->DIE_HT);
    pstmt->setDouble(4, wcr->DIE_WID);
    pstmt->setUInt(5, wcr->WF_UNITS);
    str[0] = (char)wcr->WF_FLAT;
    str[1] = 0;
    pstmt->setString(6, string(str));
    // if(wcr->CENTER_X >= 0 && wcr->CENTER_X < 32768)
    pstmt->setInt(7, wcr->CENTER_X);
    // if(wcr->CENTER_Y >= 0 && wcr->CENTER_Y < 32768)
    pstmt->setInt(8, wcr->CENTER_Y);
    str[0] = (char)wcr->POS_X;
    str[1] = 0;
    pstmt->setString(9, string(str));
    str[0] = (char)wcr->POS_Y;
    str[1] = 0;
    pstmt->setString(10, string(str));
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;
    return (1);
}
/*********************************************************************************/

int thread_process::add_hbr(rec_hbr *hbr)
{
    vector<PART_INFO>::iterator itr;
    sql::PreparedStatement *pstmt;
    char str[2];
    sql::SQLString qry = "INSERT INTO hbr(stdf_id,head_num,site_num,hbin_num,hbin_cnt,hbin_pf,hbin_nam)";
    qry += " VALUES(?,?,?,?,?,?,?)";
    pstmt = con->prepareStatement(qry);
    pstmt->setUInt(1, ap->get_stdf_id());
    pstmt->setUInt(2, hbr->HEAD_NUM);
    pstmt->setUInt(3, hbr->SITE_NUM);
    pstmt->setUInt(4, hbr->HBIN_NUM);
    pstmt->setUInt(5, hbr->HBIN_CNT);
    str[0] = (char)hbr->HBIN_PF;
    str[1] = 0;
    pstmt->setString(6, string(str));
    pstmt->setString(7, ap->extract_str(hbr->HBIN_NAM));
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;
    return (1);
}
/*********************************************************************************/

int thread_process::add_sbr(rec_sbr *sbr)
{
    vector<PART_INFO>::iterator itr;
    sql::PreparedStatement *pstmt;
    char str[2];
    sql::SQLString qry = "INSERT INTO sbr(stdf_id,head_num,site_num,sbin_num,sbin_cnt,sbin_pf,sbin_nam)";
    qry += " VALUES(?,?,?,?,?,?,?)";
    pstmt = con->prepareStatement(qry);
    pstmt->setUInt(1, ap->get_stdf_id());
    pstmt->setUInt(2, sbr->HEAD_NUM);
    pstmt->setUInt(3, sbr->SITE_NUM);
    pstmt->setUInt(4, sbr->SBIN_NUM);
    pstmt->setUInt(5, sbr->SBIN_CNT);
    str[0] = (char)sbr->SBIN_PF;
    str[1] = 0;
    pstmt->setString(6, string(str));
    pstmt->setString(7, ap->extract_str(sbr->SBIN_NAM));
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;
    return (1);
}
/*********************************************************************************/

int thread_process::set_ptr_limits(rec_ptr *ptr)
{
    double neg_inf = -std::numeric_limits<double>::infinity();
    double pos_inf = std::numeric_limits<double>::infinity();
    TEST_DATA t_data;

    // check for valid data in this ptr struct.
    // If yes, then save all parametric test static values to
    // a thread specific rec_ptr map with TEST_NUM as key.
    if (ptr->HI_LIMIT != 0 && ptr->HI_LIMIT != neg_inf && ptr->HI_LIMIT != pos_inf)
    {
        // if(READ(ptr->OPT_FLAG,5) || READ(ptr->OPT_FLAG,7))
        //{
        if (test_limits_map.count(ptr->TEST_NUM) > 0)
        {
            test_limits_map[ptr->TEST_NUM].hi_limit = ptr->HI_LIMIT;
            test_limits_map[ptr->TEST_NUM].lo_limit = ptr->LO_LIMIT;
        }
        else
        {
            t_data.resfmt                  = ap->extract_str(ptr->C_RESFMT);
            t_data.llmfmt                  = ap->extract_str(ptr->C_LLMFMT);
            t_data.hlmfmt                  = ap->extract_str(ptr->C_HLMFMT);
            t_data.units                   = ap->extract_str(ptr->UNITS);
            t_data.lo_limit                = ptr->LO_LIMIT;
            t_data.hi_limit                = ptr->HI_LIMIT;
            t_data.lo_spec                 = ptr->LO_SPEC;
            t_data.hi_spec                 = ptr->HI_SPEC;
            t_data.llm_scal                = ptr->LLM_SCAL;
            t_data.hlm_scal                = ptr->HLM_SCAL;
            t_data.res_scal                = ptr->RES_SCAL;
            t_data.scale_err               = ((READ(ptr->PARM_FLG, 0)) ? 1 : 0);
            t_data.draft_err               = ((READ(ptr->PARM_FLG, 1)) ? 1 : 0);
            t_data.ocill_err               = ((READ(ptr->PARM_FLG, 2)) ? 1 : 0);
            t_data.hivalue_err             = ((READ(ptr->PARM_FLG, 3)) ? 1 : 0);
            t_data.lovalue_err             = ((READ(ptr->PARM_FLG, 4)) ? 1 : 0);
            test_limits_map[ptr->TEST_NUM] = t_data;
        }
        //}
    }
    return (1);
}
/*********************************************************************************/

int thread_process::add_new_ptr(rec_ptr *ptr, const string &test_txt)
{
    string str_value;
    TEST_DATA t_data = {}; // Zero-initialize struct
    vector<PART_INFO>::iterator itr;
    int cond_group_id;

    set_ptr_limits(ptr);
    for (itr = v_parts.begin(); itr != v_parts.end(); ++itr)
    {
        if (itr->head_num == ptr->HEAD_NUM && itr->site_num == ptr->SITE_NUM)
        {
            if (!READ(ptr->TEST_FLG, 0) && !READ(ptr->TEST_FLG, 1) && !READ(ptr->TEST_FLG, 2) &&
                !READ(ptr->TEST_FLG, 3) && !READ(ptr->TEST_FLG, 4) && !READ(ptr->TEST_FLG, 5) &&
                !READ(ptr->PARM_FLG, 0) && !READ(ptr->PARM_FLG, 1) && !READ(ptr->PARM_FLG, 2))
                t_data.result = ptr->RESULT;
            else
                t_data.result = 0;
            if (!READ(ptr->TEST_FLG, 6))
                t_data.test_status = ((!READ(ptr->TEST_FLG, 7)) ? PASS : FAIL);
            t_data.test_txt = test_txt;
            if (!READ(ptr->TEST_FLG, 0))
                t_data.alarm_id = ap->extract_str(ptr->ALARM_ID);
            else
                t_data.alarm_id = " ";
            t_data.ptr = *ptr;
            // Setting the most current thread_specific cond_group_id to the PTR record
            cond_group_id        = get_cond_group_id(ptr->SITE_NUM);
            t_data.cond_group_id = cond_group_id;
            itr->v_ptr.push_back(t_data);
            break;
        }
    }
    return (1);
}
/**********************************************************************************/

int thread_process::update_pir()
{
    int cnt = 0;
    vector<PART_INFO>::iterator itr;
    ostringstream oss;
    sql::PreparedStatement *pstmt;
    sql::SQLString qry = "INSERT INTO pir(stdf_id,head_num,site_num) ";
    qry += "VALUES(?,?,?)";
    pstmt = con->prepareStatement(qry);

    if (v_parts.size() == 0)
        return (1);
    for (itr = v_parts.begin(); itr != v_parts.end(); ++itr)
    {
        // pstmt = con->prepareStatement(qry);
        pstmt->setUInt(1, ap->get_stdf_id());
        pstmt->setUInt(2, itr->head_num);
        pstmt->setUInt(3, itr->site_num);
        pstmt->executeUpdate();
        cnt++;
        // pstmt->close();
    }
    con->commit();
    pstmt->close();
    delete pstmt;
    return (1);
}
/**********************************************************************************/

int thread_process::update_batch_pir()
{
    vector<PART_INFO>::iterator itr;
    bool first;
    int fld_ctr;
    ostringstream oss;
    sql::PreparedStatement *pstmt = NULL;
    sql::SQLString qry = "INSERT INTO pir(stdf_id,head_num,site_num) VALUES";

    if (v_parts.size() == 0)
        return (1);
    first   = true;
    fld_ctr = 1;
    for (itr = v_parts.begin(); itr != v_parts.end(); ++itr)
    {
        if (first == true)
            qry += "(?,?,?)";
        else
            qry += ",(?,?,?)";
        first = false;
    }
    pstmt = con->prepareStatement(qry);
    for (itr = v_parts.begin(); itr != v_parts.end(); ++itr)
    {
        pstmt->setUInt(fld_ctr, ap->get_stdf_id());
        fld_ctr++;
        pstmt->setUInt(fld_ctr, itr->head_num);
        fld_ctr++;
        pstmt->setUInt(fld_ctr, itr->site_num);
        fld_ctr++;
    }
    oss.clear();
    oss << "Thread# " << thread_num << " Updating PIR table..";
    log(2, oss.str());
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;
    return (1);
}
/**********************************************************************************/

int thread_process::update_ptr(vector<TEST_DATA> *v_ptr, unsigned int t_prr_id, string part_id)
{
    double neg_inf = -std::numeric_limits<double>::infinity();
    double pos_inf = std::numeric_limits<double>::infinity();
    int cnt        = 0;
    vector<TEST_DATA>::iterator itr;
    TEST_DATA t_data;
    sql::PreparedStatement *pstmt;
    ostringstream oss;

    if (v_ptr->size() == 0)
        return (1);

    // Build multi-row INSERT statement with all accumulated rows
    string qry_str = "INSERT INTO ptr(stdf_id,prr_id,test_num,head_num,site_num,test_flg,"
                     "parm_flg,result,test_txt,opt_flag,alarm_id,res_scal,llm_scal,hlm_scal,units,c_resfmt,c_llmfmt,c_hlmfmt,lo_"
                     "spec,hi_spec,"
                     "scale_err,draft_err,ocill_err,lovalue_err,hivalue_err,test_status,cond_group_id,lo_limit,hi_limit) VALUES";

    // Optimization: Reserve capacity to minimize reallocations during string construction
    qry_str.reserve(qry_str.size() + v_ptr->size() * 128);

    // Accumulate VALUES clauses for all records
    bool first = true;
    for (itr = v_ptr->begin(); itr != v_ptr->end(); ++itr)
    {
        if (first)
            qry_str += "(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
        else
            qry_str += ",(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
        first = false;
    }

    pstmt = con->prepareStatement(qry_str);

    // Bind all parameters for all records sequentially
    int param_idx = 1;
    unsigned int last_test_num = 0;

    for (itr = v_ptr->begin(); itr != v_ptr->end(); ++itr)
    {
        // Cache lookup: Exploits STDF ordering to reduce map access overhead
        if (itr->ptr.TEST_NUM != last_test_num)
        {
            t_data = test_limits_map[itr->ptr.TEST_NUM];
            last_test_num = itr->ptr.TEST_NUM;
        }

        pstmt->setUInt(param_idx++, ap->get_stdf_id());
        pstmt->setUInt(param_idx++, t_prr_id);
        pstmt->setUInt(param_idx++, itr->ptr.TEST_NUM);
        pstmt->setUInt(param_idx++, itr->ptr.HEAD_NUM);
        pstmt->setUInt(param_idx++, itr->ptr.SITE_NUM);
        pstmt->setUInt(param_idx++, itr->ptr.TEST_FLG);
        pstmt->setUInt(param_idx++, itr->ptr.PARM_FLG);
        
        // Safety: Scrub NaN/Inf to prevent SQL ingestion errors
        //pstmt->setDouble(param_idx++, (std::isnan(itr->result) || std::isinf(itr->result) ? NULL : itr->result));
        if (std::isnan(itr->result) || std::isinf(itr->result))
        {
            pstmt->setNull(param_idx++, sql::DataType::DOUBLE);
        }
        else
        {
            pstmt->setDouble(param_idx++, itr->result);
        }

        pstmt->setString(param_idx++, itr->test_txt);
        pstmt->setString(param_idx++, to_string(itr->ptr.OPT_FLAG));
        pstmt->setString(param_idx++, itr->alarm_id);
        pstmt->setInt(param_idx++, t_data.res_scal);
        pstmt->setInt(param_idx++, t_data.llm_scal);
        pstmt->setInt(param_idx++, t_data.hlm_scal);
        pstmt->setString(param_idx++, t_data.units);
        pstmt->setString(param_idx++, t_data.resfmt);
        pstmt->setString(param_idx++, t_data.llmfmt);
        pstmt->setString(param_idx++, t_data.hlmfmt);
        pstmt->setDouble(param_idx++, t_data.lo_spec);
        pstmt->setDouble(param_idx++, t_data.hi_spec);
        pstmt->setBoolean(param_idx++, t_data.scale_err);
        pstmt->setBoolean(param_idx++, t_data.draft_err);
        pstmt->setBoolean(param_idx++, t_data.ocill_err);
        pstmt->setBoolean(param_idx++, t_data.lovalue_err);
        pstmt->setBoolean(param_idx++, t_data.hivalue_err);
        pstmt->setBoolean(param_idx++, itr->test_status);
        pstmt->setUInt(param_idx++, itr->cond_group_id);

        // Robustness: Use industry-standard limits for undefined spec boundaries
        pstmt->setDouble(param_idx++, (std::isnan(t_data.lo_limit) || (t_data.lo_limit == neg_inf) ? -9.9e37 : t_data.lo_limit));
        pstmt->setDouble(param_idx++, (std::isnan(t_data.hi_limit) || (t_data.hi_limit == pos_inf) ? 9.9e37 : t_data.hi_limit));
        cnt++;
    }
    // Execute ONCE for all accumulated records
    oss.str("");
    oss << "Thread# " << thread_num << " Batching " << cnt << " PTR records in single INSERT..";
    log(2, oss.str());
    pstmt->executeUpdate();

    // clearing v_ptr
    v_ptr->clear();
    con->commit();
    pstmt->close();
    delete pstmt;
    // test_limits_map.clear();
    return (1);
}
/*********************************************************************************/

int thread_process::add_tsr(rec_tsr *tsr)
{
    sql::PreparedStatement *pstmt;
    sql::SQLString qry = "INSERT INTO tsr(stdf_id,head_num,site_num,test_typ,test_num,";
    qry +=
        "exec_cnt,fail_cnt,alrm_cnt,test_nam,seq_name,test_lbl,opt_flag,test_tim,test_min,test_max,tst_sums,tst_sqrs)";
    qry += "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
    char str[2];
    pstmt = con->prepareStatement(qry);
    pstmt->setUInt(1, ap->get_stdf_id());
    pstmt->setUInt(2, tsr->HEAD_NUM);
    pstmt->setUInt(3, tsr->SITE_NUM);
    str[0] = (char)tsr->TEST_TYP;
    str[1] = 0;
    pstmt->setString(4, string(str));
    pstmt->setUInt(5, tsr->TEST_NUM);
    pstmt->setUInt(6, tsr->EXEC_CNT);
    pstmt->setUInt(7, tsr->FAIL_CNT);
    pstmt->setUInt(8, tsr->ALRM_CNT);
    pstmt->setString(9, ap->extract_str(tsr->TEST_NAM));
    pstmt->setString(10, ap->extract_str(tsr->SEQ_NAME));
    pstmt->setString(11, ap->extract_str(tsr->TEST_LBL));
    pstmt->setString(12, to_string(tsr->OPT_FLAG));
    if (READ(tsr->OPT_FLAG, 2) || std::isnan(tsr->TEST_TIM) || std::isinf(tsr->TEST_TIM))
        pstmt->setDouble(13, 0);
    else
        pstmt->setDouble(13, tsr->TEST_TIM);
    if (READ(tsr->OPT_FLAG, 0) || std::isnan(tsr->TEST_MIN) || std::isinf(tsr->TEST_MIN))
        pstmt->setDouble(14, 0);
    else
        pstmt->setDouble(14, tsr->TEST_MIN);
    if (READ(tsr->OPT_FLAG, 1) || std::isnan(tsr->TEST_MAX) || std::isinf(tsr->TEST_MAX))
        pstmt->setDouble(15, 0);
    else
        pstmt->setDouble(15, tsr->TEST_MAX);
    if (READ(tsr->OPT_FLAG, 4) || std::isnan(tsr->TST_SUMS) || std::isinf(tsr->TST_SUMS))
        pstmt->setDouble(16, 0);
    else
        pstmt->setDouble(16, tsr->TST_SUMS);
    if (READ(tsr->OPT_FLAG, 5) || std::isnan(tsr->TST_SQRS) || std::isinf(tsr->TST_SQRS))
        pstmt->setDouble(17, 0);
    else
        pstmt->setDouble(17, tsr->TST_SQRS);
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;
    return (1);
}
/**********************************************************************************/

int thread_process::process_prr_record(rec_prr *prr)
{
    vector<PART_INFO>::iterator itr;
    unsigned int t_prr_id = 0;
    sql::PreparedStatement *pstmt = NULL;
    sql::SQLString qry =
        "INSERT INTO prr(stdf_id,head_num,site_num,part_flg,num_test,hard_bin,soft_bin,x_coord,y_coord,";
    qry += "test_t,part_id,part_txt,part_fix)";
    qry += "VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?)";
    for (itr = v_parts.begin(); itr != v_parts.end(); ++itr)
    {
        if (itr->head_num == prr->HEAD_NUM && itr->site_num == prr->SITE_NUM)
        {
            pstmt = con->prepareStatement(qry);
            pstmt->setUInt(1, ap->get_stdf_id());
            pstmt->setUInt(2, prr->HEAD_NUM);
            pstmt->setUInt(3, prr->SITE_NUM);
            pstmt->setString(4, to_string(prr->PART_FLG));
            pstmt->setUInt(5, prr->NUM_TEST);
            pstmt->setUInt(6, prr->HARD_BIN);
            pstmt->setUInt(7, prr->SOFT_BIN);
            pstmt->setInt(8, prr->X_COORD);
            pstmt->setInt(9, prr->Y_COORD);
            pstmt->setUInt(10, prr->TEST_T);
            pstmt->setString(11, ap->extract_str(prr->PART_ID));
            pstmt->setString(12, ap->extract_str(prr->PART_TXT));
            pstmt->setString(13, ap->extract_str((char *)prr->PART_FIX).c_str());
            pstmt->executeUpdate();
            pstmt->close();
            delete pstmt;

            // Use thread-safe session-local LAST_INSERT_ID
            sql::Statement *id_stmt = con->createStatement();
            sql::ResultSet *id_res = id_stmt->executeQuery("SELECT LAST_INSERT_ID()");
            if (id_res->next())
            {
                t_prr_id = id_res->getUInt(1);
            }
            delete id_res;
            delete id_stmt;

            // update_ptr() will update ptr only for one head_num+site_num
            update_ptr(&itr->v_ptr, t_prr_id, ap->extract_str(prr->PART_ID));
            update_batch_dtr();
            update_ftr(&itr->f_ptr, t_prr_id);
            itr->v_ptr.clear();
            itr->f_ptr.clear();

            itr = v_parts.erase(itr);
            break;
        }
    }
    return (1);
}
/**********************************************************************************/

int thread_process::add_pmr(rec_pmr *pmr)
{
    sql::PreparedStatement *pstmt;

    sql::SQLString qry = "INSERT INTO pmr(stdf_id,head_num,site_num,pmr_indx,chan_typ,chan_nam,phy_nam,log_nam)";
    qry += " VALUES(?,?,?,?,?,?,?,?)";
    // if(pmr->PMR_INDX == NULL || pmr->PMR_INDX < 0 || pmr->PMR_INDX > 32767)
    if (pmr->PMR_INDX == 0 || pmr->PMR_INDX < 0 || pmr->PMR_INDX > 32767)
        pmr->PMR_INDX = 0;
    pstmt = con->prepareStatement(qry);
    pstmt->setUInt(1, ap->get_stdf_id());
    pstmt->setUInt(2, pmr->HEAD_NUM);
    pstmt->setUInt(3, pmr->SITE_NUM);
    pstmt->setUInt(4, pmr->PMR_INDX);
    pstmt->setUInt(5, pmr->CHAN_TYP);
    pstmt->setString(6, ap->extract_str(pmr->CHAN_NAM));
    pstmt->setString(7, ap->extract_str(pmr->PHY_NAM));
    pstmt->setString(8, ap->extract_str(pmr->LOG_NAM));
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;
    return (1);
}
/******************************************************************************************************************/

int thread_process::add_pgr(rec_pgr *pgr)
{
    sql::PreparedStatement *pstmt;
    // uint16_t *p_pmr;
    sql::SQLString qry = "INSERT INTO pgr(stdf_id,grp_indx,grp_nam,indx_cnt,pmr_indx)";
    qry += " VALUES(?,?,?,?,?)";
    pstmt = con->prepareStatement(qry);
    pstmt->setUInt(1, ap->get_stdf_id());
    pstmt->setUInt(2, pgr->GRP_INDX);
    pstmt->setString(3, ap->extract_str(pgr->GRP_NAM));
    pstmt->setUInt(4, pgr->INDX_CNT);
    if (pgr->INDX_CNT > 0)
        pstmt->setString(5, string((char *)pgr->PMR_INDX, 0, (2 * (pgr->INDX_CNT))));
    else
        pstmt->setString(5, " ");
    pstmt->executeUpdate();
    /*
        p_pmr = &pgr->PMR_INDX[0];
        for (int i = 0; i < pgr->INDX_CNT;i++,p_pmr++)
            cout << "PGR_IDX:" << i << " VALUE: " << *p_pmr << " " << endl;
    */
    pstmt->close();
    delete pstmt;
    return (1);
}
/******************************************************************************************************************/

int thread_process::add_plr(rec_plr *plr)
{
    sql::PreparedStatement *pstmt;

    sql::SQLString qry =
        "INSERT INTO plr(stdf_id,grp_cnt,grp_indx,grp_mode,grp_radx,pgm_char,rtn_char,pgm_chal,rtn_chal)";
    qry += " VALUES(?,?,?,?,?,?,?,?,?)";
    pstmt = con->prepareStatement(qry);
    pstmt->setUInt(1, ap->get_stdf_id());
    pstmt->setUInt(2, plr->GRP_CNT);
    if (plr->GRP_CNT > 0)
        pstmt->setString(3, string((char *)plr->GRP_INDX, 0, (2 * (plr->GRP_CNT))));
    else
        pstmt->setString(3, " ");
    if (plr->GRP_CNT > 0)
        pstmt->setString(4, string((char *)plr->GRP_MODE, 0, (2 * (plr->GRP_CNT))));
    else
        pstmt->setString(4, " ");
    if (plr->GRP_CNT > 0)
        pstmt->setString(5, string((char *)plr->GRP_RADX, 0, (2 * (plr->GRP_CNT))));
    else
        pstmt->setString(5, " ");
    // pstmt->setString(6,ap->extract_str((char *)plr->PGM_CHAR));
    // pstmt->setString(7,ap->extract_str((char *)plr->RTN_CHAR));
    pstmt->setString(6, " ");
    pstmt->setString(7, " ");
    // pstmt->setString(8,ap->extract_str((char *)plr->PGM_CHAL));
    // pstmt->setString(9,ap->extract_str((char *)plr->RTN_CHAL));
    pstmt->setString(8, " ");
    pstmt->setString(9, " ");
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;
    return (1);
}
/******************************************************************************************************************/

int thread_process::add_new_ftr(rec_ftr *ftr)
{
    string str_value;
    FTR_DATA t_data = {}; // Zero-initialize struct
    int cond_group_id;
    vector<PART_INFO>::iterator itr;
    for (itr = v_parts.begin(); itr != v_parts.end(); ++itr)
    {
        if (itr->head_num == ftr->HEAD_NUM && itr->site_num == ftr->SITE_NUM)
        {
            t_data.timeout_err    = ((READ(ftr->TEST_FLG, 3)) ? 1 : 0);
            t_data.test_exec_flag = ((READ(ftr->TEST_FLG, 4)) ? 0 : 1);
            t_data.abort_flag     = ((READ(ftr->TEST_FLG, 5)) ? 0 : 1);
            if (!READ(ftr->TEST_FLG, 6))
                t_data.test_status = ((!READ(ftr->TEST_FLG, 7)) ? PASS : FAIL);
            // potential bug here. Field is of STDF spec. type D*n, should use extract_str2
            // t_data.fail_pin = ap->extract_str((char *)ftr->FAIL_PIN);
            t_data.fail_pin = ap->extract_str2((char *)ftr->FAIL_PIN);
            t_data.vect_nam = ap->extract_str(ftr->VECT_NAM);
            t_data.time_set = ap->extract_str(ftr->TIME_SET);
            t_data.op_code  = ap->extract_str(ftr->OP_CODE);
            t_data.test_txt = ap->extract_str(ftr->TEST_TXT);
            if (READ(ftr->TEST_FLG, 0))
            {
                t_data.alarm_err = 1;
                t_data.alarm_id  = ap->extract_str(ftr->ALARM_ID);
            }
            else
                t_data.alarm_err = 0;
            t_data.prog_txt = ap->extract_str(ftr->PROG_TXT);
            t_data.rslt_txt = ap->extract_str(ftr->RSLT_TXT);
            t_data.spin_map = ap->extract_str2((char *)ftr->SPIN_MAP);
            t_data.ftr      = *ftr;
            // Setting the most current thread_specific cond_group_id to the FTR record
            cond_group_id        = get_cond_group_id(ftr->SITE_NUM);
            t_data.cond_group_id = cond_group_id;
            itr->f_ptr.push_back(t_data);
            break;
        }
    }
    return (1);
}
/******************************************************************************************************************/

int thread_process::update_ftr(vector<FTR_DATA> *f_ptr, unsigned int t_prr_id)
{
    vector<FTR_DATA>::iterator itr;
    sql::PreparedStatement *pstmt;
    ostringstream oss;

    if (f_ptr->size() == 0)
        return (1);

    // Build multi-row INSERT statement with all accumulated FTR rows
    string qry_str = "INSERT INTO ftr(stdf_id,prr_id,test_num,head_num,site_num,test_flg,"
                     "opt_flag,cycl_cnt,rel_vadr,rept_cnt,num_fail,xfail_ad,yfail_ad,vect_off,rtn_icnt,pgm_icnt,rtn_indx,rtn_stat,"
                     "pgm_indx,pgm_stat,fail_pin,vect_nam,time_set,op_code,test_txt,alarm_id,prog_txt,rslt_txt,patg_num,spin_map,"
                     "alarm_flag,timeout_flag,abort_flag,test_exec_flag,cond_group_id,test_status) VALUES";

    // Pre-reserve capacity to minimize reallocations
    qry_str.reserve(qry_str.size() + f_ptr->size() * 80);

    // Accumulate VALUES clauses for all records
    bool first = true;
    int cnt    = 0;
    for (itr = f_ptr->begin(); itr != f_ptr->end(); ++itr)
    {
        if (first)
            qry_str += "(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
        else
            qry_str += ",(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
        first = false;
        cnt++;
    }

    pstmt = con->prepareStatement(qry_str);

    // Bind all parameters for all records sequentially
    int param_idx = 1;
    for (itr = f_ptr->begin(); itr != f_ptr->end(); ++itr)
    {
        pstmt->setUInt(param_idx++, ap->get_stdf_id());
        pstmt->setUInt(param_idx++, t_prr_id);
        pstmt->setUInt(param_idx++, itr->ftr.TEST_NUM);
        pstmt->setUInt(param_idx++, itr->ftr.HEAD_NUM);
        pstmt->setUInt(param_idx++, itr->ftr.SITE_NUM);
        pstmt->setUInt(param_idx++, itr->ftr.TEST_FLG);
        pstmt->setUInt(param_idx++, itr->ftr.OPT_FLAG);
        pstmt->setUInt(param_idx++, itr->ftr.CYCL_CNT);
        pstmt->setUInt(param_idx++, itr->ftr.REL_VADR);
        pstmt->setUInt(param_idx++, itr->ftr.REPT_CNT);
        pstmt->setUInt(param_idx++, itr->ftr.NUM_FAIL);
        pstmt->setUInt(param_idx++, itr->ftr.XFAIL_AD);
        pstmt->setUInt(param_idx++, itr->ftr.YFAIL_AD);
        pstmt->setUInt(param_idx++, itr->ftr.VECT_OFF);
        pstmt->setUInt(param_idx++, itr->ftr.RTN_ICNT);
        pstmt->setUInt(param_idx++, itr->ftr.PGM_ICNT);
        /*
                pstmt->setString(17,string((char *)itr->ftr.RTN_INDX,itr->ftr.RTN_ICNT));
                pstmt->setString(18,string((char *)itr->ftr.RTN_STAT,itr->ftr.RTN_ICNT));
                pstmt->setString(19,string((char *)itr->ftr.PGM_INDX,itr->ftr.PGM_ICNT));
                pstmt->setString(20,string((char *)itr->ftr.PGM_STAT,itr->ftr.PGM_ICNT));
        */
        pstmt->setString(param_idx++, "");
        pstmt->setString(param_idx++, "");
        pstmt->setString(param_idx++, "");
        pstmt->setString(param_idx++, "");

        pstmt->setString(param_idx++, itr->fail_pin);
        pstmt->setString(param_idx++, itr->vect_nam);
        pstmt->setString(param_idx++, itr->time_set);
        pstmt->setString(param_idx++, itr->op_code);
        pstmt->setString(param_idx++, itr->test_txt);
        pstmt->setString(param_idx++, itr->alarm_id);
        pstmt->setString(param_idx++, itr->prog_txt);
        pstmt->setString(param_idx++, itr->rslt_txt);
        pstmt->setUInt(param_idx++, itr->ftr.PATG_NUM);
        pstmt->setString(param_idx++, itr->spin_map);
        pstmt->setBoolean(param_idx++, itr->alarm_err);
        pstmt->setBoolean(param_idx++, itr->timeout_err);
        pstmt->setBoolean(param_idx++, itr->abort_flag);
        pstmt->setBoolean(param_idx++, itr->test_exec_flag);
        pstmt->setUInt(param_idx++, itr->cond_group_id);
        pstmt->setBoolean(param_idx++, itr->test_status);
    }

    // Execute ONCE for all accumulated FTR records
    oss.clear();
    oss.str("");
    oss << "Thread# " << thread_num << " Batching " << cnt << " FTR records in single INSERT..";
    log(2, oss.str());
    pstmt->executeUpdate();

    pstmt->close();
    delete pstmt;
    return (1);
}
/**********************************************************************************/

int thread_process::update_batch_dtr()
{
    vector<string>::iterator itr;
    sql::PreparedStatement *pstmt;
    ostringstream oss;

    if (v_dtr.empty())
        return (1);

    if (ap->get_stdf_id() == 0)
        return (1);

    string qry_str = "INSERT INTO dtr(stdf_id,text_dat) VALUES";
    qry_str.reserve(qry_str.size() + v_dtr.size() * 256);

    for (size_t i = 0; i < v_dtr.size(); ++i)
    {
        if (i == 0)
            qry_str += "(?,?)";
        else
            qry_str += ",(?,?)";
    }

    pstmt = con->prepareStatement(qry_str);
    int param_idx = 1;
    for (itr = v_dtr.begin(); itr != v_dtr.end(); ++itr)
    {
        pstmt->setUInt(param_idx++, ap->get_stdf_id());
        pstmt->setString(param_idx++, *itr);
    }

    oss.clear();
    oss.str("");
    oss << "Thread# " << thread_num << " Updating DTR table.";
    log(2, oss.str());
    pstmt->executeUpdate();
    v_dtr.clear();
    pstmt->close();
    delete pstmt;
    return (1);
}
/**********************************************************************************/

int thread_process::add_dtr(rec_dtr *dtr)
{
    sql::PreparedStatement *pstmt;

    sql::SQLString qry = "INSERT INTO dtr(stdf_id,text_dat)";
    qry += " VALUES(?,?)";
    pstmt = con->prepareStatement(qry);
    pstmt->setUInt(1, ap->get_stdf_id());
    pstmt->setString(2, (dtr->TEXT_DAT != NULL) ? string(dtr->TEXT_DAT) : " ");
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;
    return (1);
}
/**********************************************************************************/

int thread_process::add_sdr(rec_sdr *sdr)
{
    sql::PreparedStatement *pstmt;

    sql::SQLString qry =
        "INSERT INTO sdr(stdf_id,head_num,site_grp,site_cnt,site_num,hand_typ,hand_id,card_typ,card_id,";
    qry += "load_typ,load_id,dib_typ,dib_id,cabl_typ,cabl_id,cont_typ,cont_id,lasr_typ,lasr_id,extr_typ,extr_id)";
    qry += " VALUES(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)";
    pstmt = con->prepareStatement(qry);
    pstmt->setUInt(1, ap->get_stdf_id());
    pstmt->setUInt(2, sdr->HEAD_NUM);
    pstmt->setUInt(3, sdr->SITE_GRP);
    pstmt->setUInt(4, sdr->SITE_CNT);
    pstmt->setString(5, string((char *)sdr->SITE_NUM, sdr->SITE_CNT));
    pstmt->setString(6, ap->extract_str(sdr->HAND_TYP));
    pstmt->setString(7, ap->extract_str(sdr->HAND_ID));
    pstmt->setString(8, ap->extract_str(sdr->CARD_TYP));
    pstmt->setString(9, ap->extract_str(sdr->CARD_ID));
    pstmt->setString(10, ap->extract_str(sdr->LOAD_TYP));
    pstmt->setString(11, ap->extract_str(sdr->LOAD_ID));
    pstmt->setString(12, ap->extract_str(sdr->DIB_TYP));
    pstmt->setString(13, ap->extract_str(sdr->DIB_ID));
    pstmt->setString(14, ap->extract_str(sdr->CABL_TYP));
    pstmt->setString(15, ap->extract_str(sdr->CABL_ID));
    pstmt->setString(16, ap->extract_str(sdr->CONT_TYP));
    pstmt->setString(17, ap->extract_str(sdr->CONT_ID));
    pstmt->setString(18, ap->extract_str(sdr->LASR_TYP));
    pstmt->setString(19, ap->extract_str(sdr->LASR_ID));
    pstmt->setString(20, ap->extract_str(sdr->EXTR_ID));
    pstmt->setString(21, ap->extract_str(sdr->EXTR_TYP));
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;
    return (1);
}
/*********************************************************************************/

int thread_process::add_new_atr(rec_atr *atr)
{
    atr_mod_tim  = atr->MOD_TIM;
    atr_cmd_line = ap->extract_str(atr->CMD_LINE);
    return (1);
}
/*********************************************************************************/

int thread_process::add_new_far(rec_far *far)
{
    far_cpu_type = far->CPU_TYPE;
    far_stdf_ver = far->STDF_VER;
    return (1);
}
/*********************************************************************************/

int thread_process::update_atr(void)
{
    sql::PreparedStatement *pstmt;

    sql::SQLString qry = "INSERT INTO atr(stdf_id,mod_time,cmd_time)";
    qry += " VALUES(?,FROM_UNIXTIME(?),?)";
    pstmt = con->prepareStatement(qry);
    pstmt->setUInt(1, ap->get_stdf_id());
    pstmt->setUInt(2, atr_mod_tim);
    pstmt->setString(3, atr_cmd_line);
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;
    return (1);
}
/*********************************************************************************/

int thread_process::update_far(void)
{
    sql::PreparedStatement *pstmt;

    sql::SQLString qry = "INSERT INTO far(stdf_id,cpu_type,stdf_ver)";
    qry += " VALUES(?,?,?)";
    pstmt = con->prepareStatement(qry);
    pstmt->setUInt(1, ap->get_stdf_id());
    pstmt->setUInt(2, far_cpu_type);
    pstmt->setUInt(3, far_stdf_ver);
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;
    return (1);
}
/*********************************************************************************/
// Update MySQL DB via stored procedure: process_cond_group for the input STDF (site_specific)
// with COND tokens and values.
// Multiple tables may or may not get upated at the backed,depending on the input
// thread_specific COND tokens. Stored procedure returns the cond_group_id,which,in turn
// is returned by this function
int thread_process::call_process_cond_group(int site_no)
{
    sql::PreparedStatement *pstmt;
    sql::SQLString message;
    unsigned int cond_group_id = 0;
    string cond_data;
    ostringstream oss;

    //'get_site_cond_tokens' retrieves as a string, a comma delimited and sorted
    // name,value list of CONDs for an input <site_num> or for a global <site_num> of 9999,
    // when all CONDs have been logged without any SITE_NUM token.
    cond_data = get_site_cond_tokens(site_no, cond_maps);
    // First check the cond_group_cache_map for a match
    if (cond_group_cache_map.count(cond_data) > 0)
    {
        oss.str("");
        oss.clear();
        oss << "Thread# " << thread_num << " SITE_NUM:" << site_no << " COND:" << trim(cond_data);
        oss << " cache hit for cond_group_id:" << cond_group_cache_map[cond_data];
        log(2, oss.str());
        return (cond_group_cache_map[cond_data]);
    }
    else
    {
        oss.str("");
        oss.clear();
        oss << "Thread# " << thread_num << " SITE_NUM:" << site_no << " COND:" << trim(cond_data);
        log(2, oss.str());
    }
    sql::SQLString qry = "CALL process_cond_group(?,@cond_group_id,@message)";
    pstmt              = con->prepareStatement(qry);
    pstmt->setString(1, cond_data);
    pstmt->executeUpdate();
    pstmt->close();
    delete pstmt;

    stmt = con->createStatement();
    res  = stmt->executeQuery("SELECT @cond_group_id,@message");
    while (res->next())
    {
        cond_group_id = res->getUInt(1);
        message       = res->getString(2);
        // update cond_group_cache_map
        cond_group_cache_map[cond_data] = cond_group_id;
    }
    delete res;
    delete stmt;
    return (cond_group_id);
}
/*********************************************************************************/

int thread_process::read_stdf(int read_type)
{
    // read_type = 1, read records until max_parts_per_thread PRRs are processed
    // read_type = 2, read upto first PIR record,process that many PRR records and return
    // read_type = 3, read entire STDF
    rec_unknown *rec;
    dtc_U4 stdf_ver;
    int pir_cnt, prr_cnt, dtr_cnt, cond_group_id;
    bool atr_flag;
    unordered_map<string, bool> ptr_filter_cache;
    ostringstream oss;
    smatch match;
    unique_lock<mutex> lck(ap->mtx);
    // All threads arrive here and go to sleep
    this->ap->cv.wait(lck, [this] { return ap->threads_ready; });
    if (read_type == 1)
        lseek(f->fd, stdf_thread_offset, SEEK_SET);
    pir_cnt  = 0;
    if (verbose >= 1) {
        oss.str("");
        oss << "Thread# " << thread_num << " ap->max_parts_per_thread: " << ap->max_parts_per_thread;
        log(1, oss.str());
    }
    dtr_cnt  = 1;
    prr_cnt  = 0;
    atr_flag = false;
    stdf_get_setting(f, STDF_SETTING_VERSION, &stdf_ver);
    while (1)
    {
        if ((rec = stdf_read_record(f)) == NULL)
            break;
        if (ap->skip_table[HEAD_TO_REC(rec->header)])
            continue;
        if (read_type == 2 && pir_cnt > 0)
            break;
        switch (HEAD_TO_REC(rec->header))
        {
        case REC_FAR:
        {
            if (verbose >= 2) {
                oss.str("");
                oss << "Thread# " << thread_num << " Processing FAR..";
                log(2, oss.str());
            }
            rec_far *far = (rec_far *)rec;
            add_new_far(far);
            break;
        }
        case REC_ATR:
        {
            rec_atr *atr = (rec_atr *)rec;
            oss.clear();
            oss << "Thread# " << thread_num << " Processing ATR..";
            log(2, oss.str());
            atr_flag = true;
            add_new_atr(atr);
            break;
        }
        case REC_MIR:
        {
            rec_mir *mir = (rec_mir *)rec;
            oss.str("");
            ostringstream ss;
            ss << mir->SETUP_T << mir->START_T
               << trim(ap->extract_str(mir->LOT_ID))
               << trim(ap->extract_str(mir->SBLOT_ID))
               << trim(ap->extract_str(mir->TEST_COD))
               << trim(ap->extract_str(mir->PART_TYP))
               << trim(ap->extract_str(mir->JOB_NAM))
               << (char)mir->RTST_COD;
            string hash = ap->sha256(ss.str().data(), ss.str().length());
            oss << "Thread# " << thread_num << " Key:" << ss.str() << " STDF hash:" << hash << ". Processing MIR..";
            log(1, oss.str());
            // check for duplicate/already processed STDF..
            int stdf_id = 0;
            stdf_id     = check_duplicate_stdf(mir, hash);
            if (stdf_id > 0)
            {
                oss.str();
                oss.clear();
                oss.str("");
                oss << "Thread# " << thread_num << " duplicate STDF_ID:" << stdf_id;
                log(1, oss.str());
                // return value 10 is special. It indicates a duplicate STDF_ID.
                return (10);
            }
            add_mir(mir, hash);
            update_far();
            if (atr_flag)
                update_atr();
            break;
        }
        case REC_MRR:
        {
            rec_mrr *mrr = (rec_mrr *)rec;
            if (ap->get_stdf_id() == 0)
                break;
            oss.str("");
            oss << "Thread# " << thread_num << " Processing MRR..";
            log(1, oss.str());
            add_mrr(mrr);
            oss.str("");
            oss << "Thread# " << thread_num << " Finished processing MRR..";
            log(1, oss.str());
            break;
        }
        case REC_PCR:
        {
            rec_pcr *pcr = (rec_pcr *)rec;
            if (ap->get_stdf_id() == 0)
                break;
            oss.str("");
            oss << "Thread# " << thread_num << " Processing PCR..";
            log(2, oss.str());
            add_pcr(pcr);
            break;
        }
        case REC_HBR:
        {
            rec_hbr *hbr = (rec_hbr *)rec;
            if (ap->get_stdf_id() == 0)
                break;
            oss.str("");
            oss << "Thread# " << thread_num << " Processing HBR..";
            log(2, oss.str());
            add_hbr(hbr);
            break;
        }
        case REC_SBR:
        {
            rec_sbr *sbr = (rec_sbr *)rec;
            if (ap->get_stdf_id() == 0)
                break;
            oss.str("");
            oss << "Thread# " << thread_num << " Processing SBR..";
            log(2, oss.str());
            add_sbr(sbr);
            break;
        }
        case REC_PMR:
        {
            rec_pmr *pmr = (rec_pmr *)rec;
            if (ap->get_stdf_id() == 0)
                break;
            oss.str("");
            oss << "Thread# " << thread_num << " Processing PMR..";
            log(2, oss.str());
            add_pmr(pmr);
            break;
        }
        case REC_PGR:
        {
            rec_pgr *pgr = (rec_pgr *)rec;
            if (ap->get_stdf_id() == 0)
                break;
            oss.str("");
            oss << "Thread# " << thread_num << " Processing PGR..";
            log(2, oss.str());
            add_pgr(pgr);
            break;
        }
        case REC_PLR:
        {
            rec_plr *plr = (rec_plr *)rec;
            if (ap->get_stdf_id() == 0)
                break;
            oss.str("");
            oss << "Thread# " << thread_num << " Processing PLR..";
            log(2, oss.str());
            add_plr(plr);
            break;
        }
        case REC_RDR:
        {
            // rec_rdr *rdr = (rec_rdr*)rec;
            break;
        }
        case REC_SDR:
        {
            rec_sdr *sdr = (rec_sdr *)rec;
            if (ap->get_stdf_id() == 0)
                break;
            oss.str("");
            oss << "Thread# " << thread_num << " Processing SDR..";
            log(2, oss.str());
            add_sdr(sdr);
            break;
        }
        case REC_WIR:
        {
            rec_wir *wir = (rec_wir *)rec;
            if (ap->get_stdf_id() == 0)
                break;
            oss.str("");
            oss << "Thread# " << thread_num << " Processing WIR..";
            log(2, oss.str());
            add_wir(wir);
            break;
        }
        case REC_WRR:
        {
            rec_wrr *wrr = (rec_wrr *)rec;
            if (ap->get_stdf_id() == 0)
                break;
            oss.str("");
            oss << "Thread# " << thread_num << " Processing WRR..";
            log(2, oss.str());
            add_wrr(wrr);
            break;
        }
        case REC_WCR:
        {
            rec_wcr *wcr = (rec_wcr *)rec;
            if (ap->get_stdf_id() == 0)
                break;
            oss.str("");
            oss << "Thread# " << thread_num << " Processing WCR..";
            log(2, oss.str());
            add_wcr(wcr);
            break;
        }
        case REC_PIR:
        {
            rec_pir *pir = (rec_pir *)rec;
            pir_cnt++;
            oss.clear();
            oss << "Thread# " << thread_num << " pir_cnt:" << pir_cnt << " Processing PIR..";
            log(2, oss.str());
            if (read_type != 2)
                add_new_pir(pir);
            break;
        }
        case REC_PRR:
        {
            rec_prr *prr = (rec_prr *)rec;
            dtr_cnt++;
            prr_cnt++;
            oss.clear();
            oss << "Thread# " << thread_num << " prr_cnt:" << prr_cnt << " Processing PRR..";
            log(2, oss.str());
            // update_batch_pir();
            // March 24th. 2026. PIR was getting inserted multiple times for the one part_id!
            // PIR was getting inserted within the function update_pir as a batch job using
            // the vector v_parts, but v_parts was not clearing all the elements (head# / site#)
            // in process_prr_record()
            // update_pir();
            process_prr_record(prr);

            // New logic: Exit after processing the assigned number of parts.
            // The last thread processes everything until EOF.
            if (read_type == 1 && prr_cnt >= ap->max_parts_per_thread && thread_num < (int)ap->stdf_threads.size())
            {
                oss.str("");
                oss << "Thread:" << thread_num << " exiting after processing batch of " << prr_cnt << " PRR records.";
                log(1, oss.str());
                stdf_free_record(rec);
                goto end_loop;
            }
            break;
        }
#ifdef STDF_VER3
        case REC_PDR:
        {
            rec_pdr *pdr = (rec_pdr *)rec;
            break;
        }
        case REC_FDR:
        {
            rec_fdr *fdr = (rec_fdr *)rec;
            break;
        }
#endif
        case REC_TSR:
        {
            rec_tsr *tsr = (rec_tsr *)rec;
            if (ap->get_stdf_id() == 0)
                break;
            oss.str("");
            oss << "Thread# " << thread_num << " prr_cnt:" << prr_cnt << " Processing TSR..";
            log(2, oss.str());
            add_tsr(tsr);
            break;
        }
        case REC_PTR:
        {
            rec_ptr *ptr = (rec_ptr *)rec;
            string ptr_test_name = ap->extract_str(ptr->TEST_TXT);

            // Ignore all parametric_tests that are not configured (if any)
            if (filter_parametric_tests.size() > 0)
            {
                if (ptr_filter_cache.count(ptr_test_name) == 0)
                {
                    bool found = false;
                    for (const string &pattern : filter_parametric_tests)
                    {
                        if (ptr_test_name.find(pattern) != string::npos)
                        {
                            found = true;
                            break;
                        }
                    }
                    ptr_filter_cache[ptr_test_name] = found;
                }
                
                if (!ptr_filter_cache[ptr_test_name])
                    break;
            }
            // Test PASSED
            if ((!READ(ptr->TEST_FLG, 6) && !READ(ptr->TEST_FLG, 7)) && ap->pass_ignore_ptr)
                break;
            
            if (verbose >= 2) {
                oss.str("");
                oss << "Thread# " << thread_num << " prr_cnt:" << prr_cnt << " Processing PTR..";
                log(2, oss.str());
            }
            add_new_ptr(ptr, ptr_test_name);
            break;
        }
        case REC_MPR:
        {
            // rec_mpr *mpr = (rec_mpr*)rec;
            break;
        }
        case REC_FTR:
        {
            rec_ftr *ftr = (rec_ftr *)rec;
            // Test PASSED
            if ((!READ(ftr->TEST_FLG, 6) && !READ(ftr->TEST_FLG, 7)) && ap->pass_ignore_ftr)
                break;
            oss.str("");
            oss << "Thread# " << thread_num << " Processing FTR..";
            log(2, oss.str());
            add_new_ftr(ftr);
            break;
        }
        case REC_BPS:
        {
            // rec_bps *bps = (rec_bps*)rec;
            break;
        }
        case REC_EPS:
        {
            /*rec_eps *eps = (rec_eps*)rec;*/
            break;
        }
#ifdef STDF_VER3
        case REC_SHB:
        {
            rec_shb *shb = (rec_shb *)rec;
        }
        case REC_SSB:
        {
            rec_ssb *ssb = (rec_ssb *)rec;
            break;
        }
        case REC_STS:
        {
            rec_sts *sts = (rec_sts *)rec;
            break;
        }
        case REC_SCR:
        {
            rec_scr *scr = (rec_scr *)rec;
            break;
        }
#endif
        case REC_GDR:
        {
            // oss.clear();
            // oss << "Thread# " << thread_num << " Processing GDR..";
            // log(2,oss.str());
            // rec_gdr *gdr = (rec_gdr*)rec;
            break;
        }
        case REC_DTR:
        {
            oss.str("");
            oss << "Thread# " << thread_num << " Processing DTR..";
            log(2, oss.str());
            rec_dtr *dtr = (rec_dtr *)rec;
            // COND processing logic:
            // 1. Check for COND: pattern at start of DTR
            // 2. If found, call  update_cond_map(cond_lines[i],cond_maps) to
            //    add the COND tokens to site_specific or global (site:9999) maps
            // 3. keep doing 1 and 2 for every DTR record
            // 4. Update the cond_data to MySQL DB via: cond_group_id = call_process_cond_group(int site_no);
            // 5. cond_group_id will set in cond_group_ids_map[]. Refer struct. TEST_DATA and FTR_DATA

            // check whether a DTR record has COND logging
            string cond = (dtr->TEXT_DAT != NULL) ? string(dtr->TEXT_DAT) : "";
            // checking wheher the logged DTR record has any COND: logged
            if (!cond.empty() && regex_search(cond, match, ap->cond_pattern))
            {
                int site_no = update_cond_map(cond, cond_maps, thread_num);
                // site_no return by update_cond_map the SITE[] logged with the COND line.
                // site_no will be 9999, if no SITE[] is logged within COND line.
                if (site_no >= 0)
                {
                    // update MySQL DB with the COND tokens and get the cond_group_id
                    // for the updated COND tokens. This cond_group_id and SITE_NUM have to be stored in
                    // a site_cond_group_id_map[] and retrieved for every PTR/FTR record update
                    cond_group_id = call_process_cond_group(site_no);
                    // updating SITE_NUM:COND_GROUP_ID map for each thread
                    cond_group_ids_map[site_no] = cond_group_id;
                }
            }
            v_dtr.push_back(cond);
            // add_dtr(dtr);
            break;
        }
        case REC_UNKNOWN:
        {
            /* cout << "unknown " << endl;
             rec_unknown *unk = (rec_unknown*)rec;
             printf("\tBytes: %i\n", unk->header.REC_LEN);
             printf("\tTYP: 0x%X [%i]\n", unk->header.REC_TYP, unk->header.REC_TYP);
             printf("\tSUB: 0x%X [%i]\n", unk->header.REC_SUB, unk->header.REC_SUB);
             */
        }
        }
        stdf_free_record(rec);
    }
end_loop:
    oss.str("");
    oss << "Thread# " << thread_num << " prr_cnt:" << prr_cnt << ".Updating DTR and Committing transaction..";
    log(1, oss.str());
    update_batch_dtr();
    con->commit();
    return EXIT_SUCCESS;
}
/******************************************************************************************************************/

int thread_process::connect_db(void)
{
    sql::ConnectOptionsMap connection_properties;

    connection_properties["hostName"]         = ap->get_mysql_server();
    connection_properties["userName"]         = ap->get_mysql_user();
    connection_properties["password"]         = ap->get_mysql_pass();
    connection_properties["schema"]           = ap->get_mysql_db();
    connection_properties["port"]             = 3306;
    connection_properties["OPT_CHARSET_NAME"] = "latin1";
    connection_properties["OPT_COLLATE"]      = "latin1_general_cs";
    pthread_mutex_lock(&stdf_lock1);
    driver = get_driver_instance();
    pthread_mutex_unlock(&stdf_lock1);
    driver->threadInit();
    /* Create a connection */
    // con = driver->connect(ap->get_mysql_server(),ap->get_mysql_user(),ap->get_mysql_pass());
    con = driver->connect(connection_properties);
    // con->setAutoCommit(1);
    con->setAutoCommit(0);
    con->setSchema(ap->get_mysql_db());
    // setting transaction isolation level to READ UNCOMMITED to improve write time
    // Set the transaction isolation levela
    stmt = con->createStatement();
    // Sept. 15th. 2025. Setting TRANSACTION ISOLATION level to SESSION level
    stmt->execute("SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED");
    delete stmt;
    return (1);
}
/******************************************************************************************************************/

int thread_process::log(int level, string msg)
{
    if (verbose >= level)
        cout << msg << endl;
    return (1);
}
/******************************************************************************************************************/

int thread_process::set_verbose(int in_verbose)
{
    verbose = in_verbose;
    return (1);
}
/******************************************************************************************************************/
