-- Copyright 2020 Suraj Vijayan
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.

-- STDF Database Schema for MySQL/MariaDB
-- Generated based on stdf_db application source code

CREATE DATABASE IF NOT EXISTS stdf_db;
USE stdf_db;

-- 1. Master Information Record (MIR)
CREATE TABLE IF NOT EXISTS mir (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    setup_t DATETIME,
    start_t DATETIME,
    stat_num INT UNSIGNED,
    mode_cod CHAR(1),
    rtst_cod CHAR(1),
    prot_cod CHAR(1),
    burn_tim INT UNSIGNED,
    cmod_cod CHAR(1),
    lot_id VARCHAR(255),
    part_typ VARCHAR(255),
    node_nam VARCHAR(255),
    tstr_typ VARCHAR(255),
    job_nam VARCHAR(255),
    job_rev VARCHAR(255),
    sblot_id VARCHAR(255),
    oper_nam VARCHAR(255),
    exec_typ VARCHAR(255),
    test_cod VARCHAR(255),
    tst_temp VARCHAR(255),
    user_txt VARCHAR(255),
    aux_file VARCHAR(255),
    pkg_typ VARCHAR(255),
    family_id VARCHAR(255),
    date_cod VARCHAR(255),
    facil_id VARCHAR(255),
    floor_id VARCHAR(255),
    proc_id VARCHAR(255),
    oper_frq VARCHAR(255),
    spec_nam VARCHAR(255),
    spec_ver VARCHAR(255),
    flow_id VARCHAR(255),
    setup_id VARCHAR(255),
    dsgn_rev VARCHAR(255),
    eng_id VARCHAR(255),
    rom_cod VARCHAR(255),
    serl_num VARCHAR(255),
    supr_nam VARCHAR(255),
    uniq_hash VARCHAR(64),
    site_id INT UNSIGNED,
    INDEX idx_uniq_hash (uniq_hash),
    INDEX idx_lot_id (lot_id)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_general_cs;

-- 2. File Attributes Record (FAR)
CREATE TABLE IF NOT EXISTS far (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED UNIQUE,
    cpu_type TINYINT UNSIGNED,
    stdf_ver TINYINT UNSIGNED,
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 3. Audit Trail Record (ATR)
CREATE TABLE IF NOT EXISTS atr (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    mod_time DATETIME,
    cmd_time TEXT,
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 4. Part Information Record (PIR)
CREATE TABLE IF NOT EXISTS pir (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    head_num TINYINT UNSIGNED,
    site_num TINYINT UNSIGNED,
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE,
    INDEX idx_stdf_head_site (stdf_id, head_num, site_num)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 5. Master Results Record (MRR)
CREATE TABLE IF NOT EXISTS mrr (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED UNIQUE,
    finish_t DATETIME,
    disp_cod CHAR(1),
    usr_desc VARCHAR(255),
    exc_desc VARCHAR(255),
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 6. Part Count Record (PCR)
CREATE TABLE IF NOT EXISTS pcr (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    head_num TINYINT UNSIGNED,
    site_num TINYINT UNSIGNED,
    part_cnt INT UNSIGNED,
    rtst_cnt INT,
    abrt_cnt INT,
    good_cnt INT,
    func_cnt INT,
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 7. Hardware/Software Bin Records (HBR/SBR)
CREATE TABLE IF NOT EXISTS hbr (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    head_num TINYINT UNSIGNED,
    site_num TINYINT UNSIGNED,
    hbin_num SMALLINT UNSIGNED,
    hbin_cnt INT UNSIGNED,
    hbin_pf CHAR(1),
    hbin_nam VARCHAR(255),
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS sbr (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    head_num TINYINT UNSIGNED,
    site_num TINYINT UNSIGNED,
    sbin_num SMALLINT UNSIGNED,
    sbin_cnt INT UNSIGNED,
    sbin_pf CHAR(1),
    sbin_nam VARCHAR(255),
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 8. Wafer Records (WIR, WRR, WCR)
CREATE TABLE IF NOT EXISTS wir (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    head_num TINYINT UNSIGNED,
    site_grp TINYINT UNSIGNED,
    start_t DATETIME,
    wafer_id VARCHAR(255),
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS wrr (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    head_num TINYINT UNSIGNED,
    site_grp TINYINT UNSIGNED,
    finish_t DATETIME,
    part_cnt INT UNSIGNED,
    rtst_cnt INT,
    abrt_cnt INT,
    good_cnt INT,
    func_cnt INT,
    wafer_id VARCHAR(255),
    fabwf_id VARCHAR(255),
    frame_id VARCHAR(255),
    mask_id VARCHAR(255),
    usr_desc VARCHAR(255),
    exc_desc VARCHAR(255),
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS wcr (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    wafr_siz DOUBLE,
    die_ht DOUBLE,
    die_wid DOUBLE,
    wf_units TINYINT UNSIGNED,
    wf_flat CHAR(1),
    center_x INT,
    center_y INT,
    pos_x CHAR(1),
    pos_y CHAR(1),
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 9. Part Results Record (PRR)
CREATE TABLE IF NOT EXISTS prr (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    head_num TINYINT UNSIGNED,
    site_num TINYINT UNSIGNED,
    part_flg VARCHAR(10),
    num_test INT UNSIGNED,
    hard_bin SMALLINT UNSIGNED,
    soft_bin SMALLINT UNSIGNED,
    x_coord INT,
    y_coord INT,
    test_t INT UNSIGNED,
    part_id VARCHAR(255),
    part_txt VARCHAR(255),
    part_fix VARCHAR(255),
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE,
    INDEX idx_stdf_head_site (stdf_id, head_num, site_num)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 10. Parametric Test Result (PTR)
CREATE TABLE IF NOT EXISTS ptr (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    prr_id INT UNSIGNED,
    test_num INT UNSIGNED,
    head_num TINYINT UNSIGNED,
    site_num TINYINT UNSIGNED,
    test_flg INT UNSIGNED,
    parm_flg INT UNSIGNED,
    result DOUBLE,
    test_txt VARCHAR(255),
    opt_flag VARCHAR(10),
    alarm_id VARCHAR(255),
    res_scal INT,
    llm_scal INT,
    hlm_scal INT,
    units VARCHAR(255),
    c_resfmt VARCHAR(255),
    c_llmfmt VARCHAR(255),
    c_hlmfmt VARCHAR(255),
    lo_spec DOUBLE,
    hi_spec DOUBLE,
    scale_err BOOLEAN,
    draft_err BOOLEAN,
    ocill_err BOOLEAN,
    lovalue_err BOOLEAN,
    hivalue_err BOOLEAN,
    test_status BOOLEAN,
    cond_group_id INT UNSIGNED,
    lo_limit DOUBLE,
    hi_limit DOUBLE,
    FOREIGN KEY (prr_id) REFERENCES prr(id) ON DELETE CASCADE ON UPDATE CASCADE,
    INDEX idx_prr_id (prr_id),
    INDEX idx_stdf_id (stdf_id)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 11. Functional Test Result (FTR)
CREATE TABLE IF NOT EXISTS ftr (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    prr_id INT UNSIGNED,
    test_num INT UNSIGNED,
    head_num TINYINT UNSIGNED,
    site_num TINYINT UNSIGNED,
    test_flg INT UNSIGNED,
    opt_flag INT UNSIGNED,
    cycl_cnt INT UNSIGNED,
    rel_vadr INT UNSIGNED,
    rept_cnt INT UNSIGNED,
    num_fail INT UNSIGNED,
    xfail_ad INT UNSIGNED,
    yfail_ad INT UNSIGNED,
    vect_off INT UNSIGNED,
    rtn_icnt INT UNSIGNED,
    pgm_icnt INT UNSIGNED,
    rtn_indx BLOB,
    rtn_stat BLOB,
    pgm_indx BLOB,
    pgm_stat BLOB,
    fail_pin TEXT,
    vect_nam VARCHAR(255),
    time_set VARCHAR(255),
    op_code VARCHAR(255),
    test_txt VARCHAR(255),
    alarm_id VARCHAR(255),
    prog_txt VARCHAR(255),
    rslt_txt VARCHAR(255),
    patg_num TINYINT UNSIGNED,
    spin_map TEXT,
    alarm_flag BOOLEAN,
    timeout_flag BOOLEAN,
    abort_flag BOOLEAN,
    test_exec_flag BOOLEAN,
    cond_group_id INT UNSIGNED,
    test_status BOOLEAN,
    FOREIGN KEY (prr_id) REFERENCES prr(id) ON DELETE CASCADE ON UPDATE CASCADE,
    INDEX idx_prr_id (prr_id)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 12. Test Synopsis Record (TSR)
CREATE TABLE IF NOT EXISTS tsr (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    head_num TINYINT UNSIGNED,
    site_num TINYINT UNSIGNED,
    test_typ CHAR(1),
    test_num INT UNSIGNED,
    exec_cnt INT UNSIGNED,
    fail_cnt INT UNSIGNED,
    alrm_cnt INT UNSIGNED,
    test_nam VARCHAR(255),
    seq_name VARCHAR(255),
    test_lbl VARCHAR(255),
    opt_flag VARCHAR(10),
    test_tim DOUBLE,
    test_min DOUBLE,
    test_max DOUBLE,
    tst_sums DOUBLE,
    tst_sqrs DOUBLE,
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 13. Data Text Record (DTR)
CREATE TABLE IF NOT EXISTS dtr (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    text_dat TEXT,
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 14. Helper Table for STDF files
CREATE TABLE IF NOT EXISTS stdf_files (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    stdf_file VARCHAR(255),
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 15. Site Description Record (SDR)
CREATE TABLE IF NOT EXISTS sdr (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    head_num TINYINT UNSIGNED,
    site_grp TINYINT UNSIGNED,
    site_cnt TINYINT UNSIGNED,
    site_num VARCHAR(255),
    hand_typ VARCHAR(255),
    hand_id VARCHAR(255),
    card_typ VARCHAR(255),
    card_id VARCHAR(255),
    load_typ VARCHAR(255),
    load_id VARCHAR(255),
    dib_typ VARCHAR(255),
    dib_id VARCHAR(255),
    cabl_typ VARCHAR(255),
    cabl_id VARCHAR(255),
    cont_typ VARCHAR(255),
    cont_id VARCHAR(255),
    lasr_typ VARCHAR(255),
    lasr_id VARCHAR(255),
    extr_typ VARCHAR(255),
    extr_id VARCHAR(255),
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 16. Part Map Record (PMR)
CREATE TABLE IF NOT EXISTS pmr (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    head_num TINYINT UNSIGNED,
    site_num TINYINT UNSIGNED,
    pmr_indx SMALLINT UNSIGNED,
    chan_typ TINYINT UNSIGNED,
    chan_nam VARCHAR(255),
    phy_nam VARCHAR(255),
    log_nam VARCHAR(255),
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 17. Pin Group Record (PGR)
CREATE TABLE IF NOT EXISTS pgr (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    grp_indx SMALLINT UNSIGNED,
    grp_nam VARCHAR(255),
    indx_cnt SMALLINT UNSIGNED,
    pmr_indx VARBINARY(1024),
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 18. Pin List Record (PLR)
CREATE TABLE IF NOT EXISTS plr (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    stdf_id INT UNSIGNED,
    grp_cnt SMALLINT UNSIGNED,
    grp_indx VARBINARY(1024),
    grp_mode VARBINARY(1024),
    grp_radx VARBINARY(1024),
    pgm_char VARCHAR(255),
    rtn_char VARCHAR(255),
    pgm_chal VARCHAR(255),
    rtn_chal VARCHAR(255),
    FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

-- 16. Condition Grouping (Inferred from stored procedure calls)
CREATE TABLE IF NOT EXISTS cond_group_master (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    cond_group_name VARCHAR(512) NOT NULL,
    UNIQUE INDEX `uniqu_idx` (`cond_group_name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS cond_master (
    id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    cond_name VARCHAR(100) NOT NULL,
    cond_value VARCHAR(100) NOT NULL,
    cond_description VARCHAR(255),
    UNIQUE KEY idx_name_val (cond_name, cond_value)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

CREATE TABLE IF NOT EXISTS cond_group (
  `id` INT UNSIGNED NOT NULL AUTO_INCREMENT,
  `cond_group_id` INT UNSIGNED NULL DEFAULT NULL,
  `cond_master_id` INT UNSIGNED NULL DEFAULT NULL,
  PRIMARY KEY (`id`),
  INDEX `cond_idx_idx` (`cond_group_id`),
  INDEX `cond_master_key` (`cond_master_id`),
  UNIQUE INDEX `uniq_group_cond` (`cond_group_id`, `cond_master_id`),
  FOREIGN KEY (`cond_group_id`) REFERENCES `cond_group_master` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  FOREIGN KEY (`cond_master_id`) REFERENCES `cond_master` (`id`) ON DELETE CASCADE ON UPDATE CASCADE)
  ENGINE = InnoDB
  AUTO_INCREMENT = 1
  DEFAULT CHARACTER SET = latin1;

-- Create a view to facilitate searching for existing groups
DROP VIEW IF EXISTS `cond_view`;
CREATE OR REPLACE VIEW `cond_view` AS 
SELECT
    `G`.`cond_group_id` AS `COND_GROUP_ID`,
    `G`.`cond_master_id` AS `COND_ID`,`M`.`cond_name` AS `COND_NAME`,
    `M`.`cond_value` AS `COND_VALUE`,
    concat(`M`.`cond_name`,':',`M`.`cond_value`) AS `COND` 
    FROM (`cond_group` `G` 
    JOIN `cond_master` `M` on(`M`.`id` = `G`.`cond_master_id`)) 
    ORDER BY `G`.`cond_group_id`,`G`.`id`;

DELIMITER $$
DROP PROCEDURE IF EXISTS `process_cond_group`$$

CREATE PROCEDURE `process_cond_group`(
    IN in_cond_str TEXT,
    OUT out_cond_group_id INT,
    OUT out_status VARCHAR(100)
)
SQL SECURITY INVOKER
start_proc:BEGIN
-- This procedure takes in a COND string extracted from an STDF file.
-- This string will be broken up into COND tokens and updated to cond_master table.
-- Every unique COND_NAME + COND_VALUE will have an entry in COND master table.
    DECLARE output_string, token_name, token_value VARCHAR(100);
    DECLARE m_cond_group_name TEXT;
    DECLARE counter INT DEFAULT 1;
    DECLARE cond_cnt INT DEFAULT 0;
    DECLARE m_group_id INT;
    DECLARE m_where_clause TEXT DEFAULT '';

    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        DROP TEMPORARY TABLE IF EXISTS temp_cond_tokens;
        SET out_status = 'FAILURE. SQL Exception.';
    END;

    CREATE TEMPORARY TABLE IF NOT EXISTS temp_cond_tokens (
        t_name VARCHAR(100),
        t_value VARCHAR(100),
        t_pair VARCHAR(201),
        UNIQUE KEY uniq_pair (t_name, t_value)
    ) ENGINE=MEMORY;

    TRUNCATE TABLE temp_cond_tokens;

    loop_1: WHILE counter <= LENGTH(in_cond_str) DO
        IF LOCATE(',', in_cond_str, counter) = 0 THEN
            SET output_string = SUBSTRING(in_cond_str, counter);
            SET counter = LENGTH(in_cond_str) + 1;
        ELSE
            SET output_string = SUBSTRING(
                in_cond_str,
                counter,
                LOCATE(',', in_cond_str, counter) - counter
            );
            SET counter = counter + LENGTH(output_string) + 1;
        END IF;

        SET token_name  = TRIM(REGEXP_REPLACE(output_string, '(.*)=.*', '$1'));
        SET token_value = TRIM(REGEXP_REPLACE(output_string, '.*=(.*)', '$1'));

        IF token_name = token_value OR token_name = '' THEN
            ITERATE loop_1;
        END IF;

        SET token_name  = REPLACE(token_name, "'", "''");
        SET token_value = REPLACE(token_value, "'", "''");

        INSERT IGNORE INTO temp_cond_tokens
        VALUES (
            token_name,
            token_value,
            CONCAT(token_name, ':', token_value)
        );
    END WHILE;

    SELECT COUNT(*)
    INTO cond_cnt
    FROM temp_cond_tokens;

    IF cond_cnt = 0 THEN
        SET out_cond_group_id = 0;
        SET out_status = 'FAILURE. Invalid input.';
        DROP TEMPORARY TABLE IF EXISTS temp_cond_tokens;
        LEAVE start_proc;
    END IF;

    SELECT GROUP_CONCAT(
        t_pair ORDER BY t_name, t_value SEPARATOR ','
    ) INTO m_cond_group_name FROM temp_cond_tokens;

    SELECT GROUP_CONCAT(
        CONCAT('COND = ', QUOTE(t_pair))
        ORDER BY t_name, t_value
        SEPARATOR ' OR '
    ) INTO m_where_clause FROM temp_cond_tokens;

    SET @sql_str = CONCAT(
        'SELECT COND_GROUP_ID INTO @m_group_id FROM cond_view WHERE COND_GROUP_ID IN (',
            'SELECT COND_GROUP_ID FROM cond_view GROUP BY COND_GROUP_ID HAVING COUNT(*) = ', cond_cnt,
        ') AND (', m_where_clause, ') ',
        'GROUP BY COND_GROUP_ID HAVING COUNT(*) = ', cond_cnt,
        ' LIMIT 1'
    );

    START TRANSACTION;
    SET @m_group_id = NULL;
    PREPARE stmt FROM @sql_str;
    EXECUTE stmt;
    DEALLOCATE PREPARE stmt;

    IF @m_group_id IS NOT NULL THEN
        SET out_cond_group_id = @m_group_id;
        SET out_status = 'SUCCESS. Found existing.';
    ELSE
        INSERT IGNORE INTO cond_group_master (cond_group_name)
        VALUES (m_cond_group_name);

        SELECT id
        INTO m_group_id
        FROM cond_group_master
        WHERE cond_group_name = m_cond_group_name;

        INSERT IGNORE INTO cond_master (
            cond_name,
            cond_value,
            cond_description
        )
        SELECT
            t_name,
            t_value,
            'Auto-added via automation'
        FROM temp_cond_tokens;

        INSERT IGNORE INTO cond_group (
            cond_group_id,
            cond_master_id
        )
        SELECT
            m_group_id,
            cm.id
        FROM temp_cond_tokens t
        JOIN cond_master cm
          ON t.t_name = cm.cond_name
         AND t.t_value = cm.cond_value;

        SET out_cond_group_id = m_group_id;
        SET out_status = 'SUCCESS. Created new.';
    END IF;

    COMMIT;
    DROP TEMPORARY TABLE IF EXISTS temp_cond_tokens;
END$$
DELIMITER ;
ALTER TABLE ptr ADD INDEX idx_ptr_test (stdf_id, test_num);
ALTER TABLE ptr ADD INDEX idx_ptr_test_cond (stdf_id, test_num, cond_group_id);
ALTER TABLE ptr ADD INDEX idx_ptr_test_site (stdf_id, test_num, site_num);
ALTER TABLE ptr ADD INDEX idx_ptr_result (stdf_id, test_num, result);
ALTER TABLE prr ADD INDEX idx_prr_bins (stdf_id, hard_bin, soft_bin);
ALTER TABLE prr ADD INDEX idx_prr_xy (stdf_id, x_coord, y_coord);
ALTER TABLE prr ADD INDEX idx_prr_part_id (stdf_id, part_id);
ALTER TABLE ptr ADD CONSTRAINT fk_ptr_cond_group FOREIGN KEY (cond_group_id) REFERENCES cond_group_master(id);
ALTER TABLE ftr ADD CONSTRAINT fk_ftr_cond_group FOREIGN KEY (cond_group_id) REFERENCES cond_group_master(id);
ALTER TABLE ptr ADD CONSTRAINT fk_ptr_stdf FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE;
ALTER TABLE ftr ADD CONSTRAINT fk_ftr_stdf FOREIGN KEY (stdf_id) REFERENCES mir(id) ON DELETE CASCADE ON UPDATE CASCADE;

