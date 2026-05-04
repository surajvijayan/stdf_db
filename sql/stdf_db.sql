/*M!999999\- enable the sandbox mode */ 
-- MariaDB dump 10.19-11.4.7-MariaDB, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: stdf_db
-- ------------------------------------------------------
-- Server version	11.4.7-MariaDB-0ubuntu0.25.04.1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*M!100616 SET @OLD_NOTE_VERBOSITY=@@NOTE_VERBOSITY, NOTE_VERBOSITY=0 */;

--
-- Table structure for table `atr`
--

DROP TABLE IF EXISTS `atr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `atr` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `mod_time` datetime DEFAULT NULL,
  `cmd_time` text DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `stdf_id` (`stdf_id`),
  CONSTRAINT `atr_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=34 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cond_group`
--

DROP TABLE IF EXISTS `cond_group`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `cond_group` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `cond_group_id` int(10) unsigned DEFAULT NULL,
  `cond_master_id` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `cond_idx_idx` (`cond_group_id`),
  KEY `cond_master_key` (`cond_master_id`),
  CONSTRAINT `cond_group_ibfk_1` FOREIGN KEY (`cond_group_id`) REFERENCES `cond_group_master` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `cond_group_ibfk_2` FOREIGN KEY (`cond_master_id`) REFERENCES `cond_master` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cond_group_master`
--

DROP TABLE IF EXISTS `cond_group_master`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `cond_group_master` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `cond_group_name` varchar(512) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `uniqu_idx` (`cond_group_name`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `cond_master`
--

DROP TABLE IF EXISTS `cond_master`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `cond_master` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `cond_name` varchar(100) NOT NULL,
  `cond_value` varchar(100) NOT NULL,
  `cond_description` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `idx_name_val` (`cond_name`,`cond_value`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Temporary table structure for view `cond_view`
--

DROP TABLE IF EXISTS `cond_view`;
/*!50001 DROP VIEW IF EXISTS `cond_view`*/;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8mb4;
/*!50001 CREATE VIEW `cond_view` AS SELECT
 1 AS `COND_GROUP_ID`,
  1 AS `COND_ID`,
  1 AS `COND_NAME`,
  1 AS `COND_VALUE`,
  1 AS `COND` */;
SET character_set_client = @saved_cs_client;

--
-- Table structure for table `dtr`
--

DROP TABLE IF EXISTS `dtr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `dtr` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `text_dat` text DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `stdf_id` (`stdf_id`),
  CONSTRAINT `dtr_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=242580 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `far`
--

DROP TABLE IF EXISTS `far`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `far` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `cpu_type` tinyint(3) unsigned DEFAULT NULL,
  `stdf_ver` tinyint(3) unsigned DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `stdf_id` (`stdf_id`),
  CONSTRAINT `far_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=34 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ftr`
--

DROP TABLE IF EXISTS `ftr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `ftr` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `prr_id` int(10) unsigned DEFAULT NULL,
  `test_num` int(10) unsigned DEFAULT NULL,
  `head_num` tinyint(3) unsigned DEFAULT NULL,
  `site_num` tinyint(3) unsigned DEFAULT NULL,
  `test_flg` int(10) unsigned DEFAULT NULL,
  `opt_flag` int(10) unsigned DEFAULT NULL,
  `cycl_cnt` int(10) unsigned DEFAULT NULL,
  `rel_vadr` int(10) unsigned DEFAULT NULL,
  `rept_cnt` int(10) unsigned DEFAULT NULL,
  `num_fail` int(10) unsigned DEFAULT NULL,
  `xfail_ad` int(10) unsigned DEFAULT NULL,
  `yfail_ad` int(10) unsigned DEFAULT NULL,
  `vect_off` int(10) unsigned DEFAULT NULL,
  `rtn_icnt` int(10) unsigned DEFAULT NULL,
  `pgm_icnt` int(10) unsigned DEFAULT NULL,
  `rtn_indx` blob DEFAULT NULL,
  `rtn_stat` blob DEFAULT NULL,
  `pgm_indx` blob DEFAULT NULL,
  `pgm_stat` blob DEFAULT NULL,
  `fail_pin` text DEFAULT NULL,
  `vect_nam` varchar(255) DEFAULT NULL,
  `time_set` varchar(255) DEFAULT NULL,
  `op_code` varchar(255) DEFAULT NULL,
  `test_txt` varchar(255) DEFAULT NULL,
  `alarm_id` varchar(255) DEFAULT NULL,
  `prog_txt` varchar(255) DEFAULT NULL,
  `rslt_txt` varchar(255) DEFAULT NULL,
  `patg_num` tinyint(3) unsigned DEFAULT NULL,
  `spin_map` text DEFAULT NULL,
  `alarm_flag` tinyint(1) DEFAULT NULL,
  `timeout_flag` tinyint(1) DEFAULT NULL,
  `abort_flag` tinyint(1) DEFAULT NULL,
  `test_exec_flag` tinyint(1) DEFAULT NULL,
  `cond_group_id` int(10) unsigned DEFAULT NULL,
  `test_status` tinyint(1) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `idx_prr_id` (`prr_id`),
  CONSTRAINT `ftr_ibfk_1` FOREIGN KEY (`prr_id`) REFERENCES `prr` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `hbr`
--

DROP TABLE IF EXISTS `hbr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `hbr` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `head_num` tinyint(3) unsigned DEFAULT NULL,
  `site_num` tinyint(3) unsigned DEFAULT NULL,
  `hbin_num` smallint(5) unsigned DEFAULT NULL,
  `hbin_cnt` int(10) unsigned DEFAULT NULL,
  `hbin_pf` char(1) DEFAULT NULL,
  `hbin_nam` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `stdf_id` (`stdf_id`),
  CONSTRAINT `hbr_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=281 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mir`
--

DROP TABLE IF EXISTS `mir`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `mir` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `setup_t` datetime DEFAULT NULL,
  `start_t` datetime DEFAULT NULL,
  `stat_num` int(10) unsigned DEFAULT NULL,
  `mode_cod` char(1) DEFAULT NULL,
  `rtst_cod` char(1) DEFAULT NULL,
  `prot_cod` char(1) DEFAULT NULL,
  `burn_tim` int(10) unsigned DEFAULT NULL,
  `cmod_cod` char(1) DEFAULT NULL,
  `lot_id` varchar(255) DEFAULT NULL,
  `part_typ` varchar(255) DEFAULT NULL,
  `node_nam` varchar(255) DEFAULT NULL,
  `tstr_typ` varchar(255) DEFAULT NULL,
  `job_nam` varchar(255) DEFAULT NULL,
  `job_rev` varchar(255) DEFAULT NULL,
  `sblot_id` varchar(255) DEFAULT NULL,
  `oper_nam` varchar(255) DEFAULT NULL,
  `exec_typ` varchar(255) DEFAULT NULL,
  `test_cod` varchar(255) DEFAULT NULL,
  `tst_temp` varchar(255) DEFAULT NULL,
  `user_txt` varchar(255) DEFAULT NULL,
  `aux_file` varchar(255) DEFAULT NULL,
  `pkg_typ` varchar(255) DEFAULT NULL,
  `family_id` varchar(255) DEFAULT NULL,
  `date_cod` varchar(255) DEFAULT NULL,
  `facil_id` varchar(255) DEFAULT NULL,
  `floor_id` varchar(255) DEFAULT NULL,
  `proc_id` varchar(255) DEFAULT NULL,
  `oper_frq` varchar(255) DEFAULT NULL,
  `spec_nam` varchar(255) DEFAULT NULL,
  `spec_ver` varchar(255) DEFAULT NULL,
  `flow_id` varchar(255) DEFAULT NULL,
  `setup_id` varchar(255) DEFAULT NULL,
  `dsgn_rev` varchar(255) DEFAULT NULL,
  `eng_id` varchar(255) DEFAULT NULL,
  `rom_cod` varchar(255) DEFAULT NULL,
  `serl_num` varchar(255) DEFAULT NULL,
  `supr_nam` varchar(255) DEFAULT NULL,
  `uniq_hash` varchar(64) DEFAULT NULL,
  `site_id` int(10) unsigned DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `idx_uniq_hash` (`uniq_hash`),
  KEY `idx_lot_id` (`lot_id`)
) ENGINE=InnoDB AUTO_INCREMENT=34 DEFAULT CHARSET=latin1 COLLATE=latin1_general_cs;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `mrr`
--

DROP TABLE IF EXISTS `mrr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `mrr` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `finish_t` datetime DEFAULT NULL,
  `disp_cod` char(1) DEFAULT NULL,
  `usr_desc` varchar(255) DEFAULT NULL,
  `exc_desc` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `stdf_id` (`stdf_id`),
  CONSTRAINT `mrr_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=29 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pcr`
--

DROP TABLE IF EXISTS `pcr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pcr` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `head_num` tinyint(3) unsigned DEFAULT NULL,
  `site_num` tinyint(3) unsigned DEFAULT NULL,
  `part_cnt` int(10) unsigned DEFAULT NULL,
  `rtst_cnt` int(11) DEFAULT NULL,
  `abrt_cnt` int(11) DEFAULT NULL,
  `good_cnt` int(11) DEFAULT NULL,
  `func_cnt` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `stdf_id` (`stdf_id`),
  CONSTRAINT `pcr_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=57 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pgr`
--

DROP TABLE IF EXISTS `pgr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pgr` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(11) unsigned DEFAULT NULL,
  `grp_indx` int(11) unsigned DEFAULT NULL,
  `grp_nam` text DEFAULT NULL,
  `indx_cnt` int(11) unsigned DEFAULT NULL,
  `pmr_indx` text DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `stdf_id` (`stdf_id`),
  CONSTRAINT `pgr_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pir`
--

DROP TABLE IF EXISTS `pir`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pir` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `head_num` tinyint(3) unsigned DEFAULT NULL,
  `site_num` tinyint(3) unsigned DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `idx_stdf_head_site` (`stdf_id`,`head_num`,`site_num`),
  CONSTRAINT `pir_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=21753 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `plr`
--

DROP TABLE IF EXISTS `plr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `plr` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(11) unsigned DEFAULT NULL,
  `grp_cnt` int(11) unsigned DEFAULT NULL,
  `grp_indx` text DEFAULT NULL,
  `grp_mode` text DEFAULT NULL,
  `grp_radx` text DEFAULT NULL,
  `pgm_char` text DEFAULT NULL,
  `rtn_char` text DEFAULT NULL,
  `pgm_chal` text DEFAULT NULL,
  `rtn_chal` text DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `stdf_id` (`stdf_id`),
  CONSTRAINT `plr_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `pmr`
--

DROP TABLE IF EXISTS `pmr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `pmr` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(11) unsigned DEFAULT NULL,
  `head_num` int(1) unsigned DEFAULT NULL,
  `site_num` int(1) unsigned DEFAULT NULL,
  `pmr_indx` int(11) unsigned DEFAULT NULL,
  `chan_typ` int(1) unsigned DEFAULT NULL,
  `chan_nam` text DEFAULT NULL,
  `phy_nam` text DEFAULT NULL,
  `log_nam` text DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `stdf_id` (`stdf_id`),
  CONSTRAINT `pmr_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=8513 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `prr`
--

DROP TABLE IF EXISTS `prr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `prr` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `head_num` tinyint(3) unsigned DEFAULT NULL,
  `site_num` tinyint(3) unsigned DEFAULT NULL,
  `part_flg` varchar(10) DEFAULT NULL,
  `num_test` int(10) unsigned DEFAULT NULL,
  `hard_bin` smallint(5) unsigned DEFAULT NULL,
  `soft_bin` smallint(5) unsigned DEFAULT NULL,
  `x_coord` int(11) DEFAULT NULL,
  `y_coord` int(11) DEFAULT NULL,
  `test_t` int(10) unsigned DEFAULT NULL,
  `part_id` varchar(255) DEFAULT NULL,
  `part_txt` varchar(255) DEFAULT NULL,
  `part_fix` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `idx_stdf_head_site` (`stdf_id`,`head_num`,`site_num`),
  CONSTRAINT `prr_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=21681 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `ptr`
--

DROP TABLE IF EXISTS `ptr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `ptr` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `prr_id` int(10) unsigned DEFAULT NULL,
  `test_num` int(10) unsigned DEFAULT NULL,
  `head_num` tinyint(3) unsigned DEFAULT NULL,
  `site_num` tinyint(3) unsigned DEFAULT NULL,
  `test_flg` int(10) unsigned DEFAULT NULL,
  `parm_flg` int(10) unsigned DEFAULT NULL,
  `result` double DEFAULT NULL,
  `test_txt` varchar(255) DEFAULT NULL,
  `opt_flag` varchar(10) DEFAULT NULL,
  `alarm_id` varchar(255) DEFAULT NULL,
  `res_scal` int(11) DEFAULT NULL,
  `llm_scal` int(11) DEFAULT NULL,
  `hlm_scal` int(11) DEFAULT NULL,
  `units` varchar(255) DEFAULT NULL,
  `c_resfmt` varchar(255) DEFAULT NULL,
  `c_llmfmt` varchar(255) DEFAULT NULL,
  `c_hlmfmt` varchar(255) DEFAULT NULL,
  `lo_spec` double DEFAULT NULL,
  `hi_spec` double DEFAULT NULL,
  `scale_err` tinyint(1) DEFAULT NULL,
  `draft_err` tinyint(1) DEFAULT NULL,
  `ocill_err` tinyint(1) DEFAULT NULL,
  `lovalue_err` tinyint(1) DEFAULT NULL,
  `hivalue_err` tinyint(1) DEFAULT NULL,
  `test_status` tinyint(1) DEFAULT NULL,
  `cond_group_id` int(10) unsigned DEFAULT NULL,
  `lo_limit` double DEFAULT NULL,
  `hi_limit` double DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `idx_prr_id` (`prr_id`),
  KEY `idx_stdf_id` (`stdf_id`),
  CONSTRAINT `ptr_ibfk_1` FOREIGN KEY (`prr_id`) REFERENCES `prr` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=7023728 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `rdr`
--

DROP TABLE IF EXISTS `rdr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `rdr` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(11) unsigned DEFAULT NULL,
  `num_bins` int(1) unsigned DEFAULT NULL,
  `rtst_bin` text DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `rdr_stdf_idx` (`stdf_id`),
  CONSTRAINT `rdr_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sbr`
--

DROP TABLE IF EXISTS `sbr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `sbr` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `head_num` tinyint(3) unsigned DEFAULT NULL,
  `site_num` tinyint(3) unsigned DEFAULT NULL,
  `sbin_num` smallint(5) unsigned DEFAULT NULL,
  `sbin_cnt` int(10) unsigned DEFAULT NULL,
  `sbin_pf` char(1) DEFAULT NULL,
  `sbin_nam` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `stdf_id` (`stdf_id`),
  CONSTRAINT `sbr_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=505 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `sdr`
--

DROP TABLE IF EXISTS `sdr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `sdr` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `head_num` tinyint(3) unsigned DEFAULT NULL,
  `site_grp` tinyint(3) unsigned DEFAULT NULL,
  `site_cnt` tinyint(3) unsigned DEFAULT NULL,
  `site_num` varchar(255) DEFAULT NULL,
  `hand_typ` varchar(255) DEFAULT NULL,
  `hand_id` varchar(255) DEFAULT NULL,
  `card_typ` varchar(255) DEFAULT NULL,
  `card_id` varchar(255) DEFAULT NULL,
  `load_typ` varchar(255) DEFAULT NULL,
  `load_id` varchar(255) DEFAULT NULL,
  `dib_typ` varchar(255) DEFAULT NULL,
  `dib_id` varchar(255) DEFAULT NULL,
  `cabl_typ` varchar(255) DEFAULT NULL,
  `cabl_id` varchar(255) DEFAULT NULL,
  `cont_typ` varchar(255) DEFAULT NULL,
  `cont_id` varchar(255) DEFAULT NULL,
  `lasr_typ` varchar(255) DEFAULT NULL,
  `lasr_id` varchar(255) DEFAULT NULL,
  `extr_typ` varchar(255) DEFAULT NULL,
  `extr_id` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `stdf_id` (`stdf_id`),
  CONSTRAINT `sdr_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=34 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `site_master`
--

DROP TABLE IF EXISTS `site_master`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `site_master` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(100) DEFAULT NULL,
  `timezone` varchar(100) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `stdf_files`
--

DROP TABLE IF EXISTS `stdf_files`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `stdf_files` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `stdf_file` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `stdf_id` (`stdf_id`),
  CONSTRAINT `stdf_files_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `tsr`
--

DROP TABLE IF EXISTS `tsr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `tsr` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `head_num` tinyint(3) unsigned DEFAULT NULL,
  `site_num` tinyint(3) unsigned DEFAULT NULL,
  `test_typ` char(1) DEFAULT NULL,
  `test_num` int(10) unsigned DEFAULT NULL,
  `exec_cnt` int(10) unsigned DEFAULT NULL,
  `fail_cnt` int(10) unsigned DEFAULT NULL,
  `alrm_cnt` int(10) unsigned DEFAULT NULL,
  `test_nam` varchar(255) DEFAULT NULL,
  `seq_name` varchar(255) DEFAULT NULL,
  `test_lbl` varchar(255) DEFAULT NULL,
  `opt_flag` varchar(10) DEFAULT NULL,
  `test_tim` double DEFAULT NULL,
  `test_min` double DEFAULT NULL,
  `test_max` double DEFAULT NULL,
  `tst_sums` double DEFAULT NULL,
  `tst_sqrs` double DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `stdf_id` (`stdf_id`),
  CONSTRAINT `tsr_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=60934 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `wcr`
--

DROP TABLE IF EXISTS `wcr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `wcr` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `wafr_siz` double DEFAULT NULL,
  `die_ht` double DEFAULT NULL,
  `die_wid` double DEFAULT NULL,
  `wf_units` tinyint(3) unsigned DEFAULT NULL,
  `wf_flat` char(1) DEFAULT NULL,
  `center_x` int(11) DEFAULT NULL,
  `center_y` int(11) DEFAULT NULL,
  `pos_x` char(1) DEFAULT NULL,
  `pos_y` char(1) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `stdf_id` (`stdf_id`),
  CONSTRAINT `wcr_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=37 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `wir`
--

DROP TABLE IF EXISTS `wir`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `wir` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `head_num` tinyint(3) unsigned DEFAULT NULL,
  `site_grp` tinyint(3) unsigned DEFAULT NULL,
  `start_t` datetime DEFAULT NULL,
  `wafer_id` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `stdf_id` (`stdf_id`),
  CONSTRAINT `wir_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=33 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Table structure for table `wrr`
--

DROP TABLE IF EXISTS `wrr`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8mb4 */;
CREATE TABLE `wrr` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `stdf_id` int(10) unsigned DEFAULT NULL,
  `head_num` tinyint(3) unsigned DEFAULT NULL,
  `site_grp` tinyint(3) unsigned DEFAULT NULL,
  `finish_t` datetime DEFAULT NULL,
  `part_cnt` int(10) unsigned DEFAULT NULL,
  `rtst_cnt` int(11) DEFAULT NULL,
  `abrt_cnt` int(11) DEFAULT NULL,
  `good_cnt` int(11) DEFAULT NULL,
  `func_cnt` int(11) DEFAULT NULL,
  `wafer_id` varchar(255) DEFAULT NULL,
  `fabwf_id` varchar(255) DEFAULT NULL,
  `frame_id` varchar(255) DEFAULT NULL,
  `mask_id` varchar(255) DEFAULT NULL,
  `usr_desc` varchar(255) DEFAULT NULL,
  `exc_desc` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `stdf_id` (`stdf_id`),
  CONSTRAINT `wrr_ibfk_1` FOREIGN KEY (`stdf_id`) REFERENCES `mir` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=29 DEFAULT CHARSET=latin1 COLLATE=latin1_swedish_ci;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping routines for database 'stdf_db'
--
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'ONLY_FULL_GROUP_BY,STRICT_TRANS_TABLES,ERROR_FOR_DIVISION_BY_ZERO,NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION' */ ;
/*!50003 DROP PROCEDURE IF EXISTS `process_cond_group` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb3 */ ;
/*!50003 SET character_set_results = utf8mb3 */ ;
/*!50003 SET collation_connection  = utf8mb3_general_ci */ ;
DELIMITER ;;
CREATE DEFINER=`sispara`@`localhost` PROCEDURE `process_cond_group`(
    IN in_cond_str TEXT,
    OUT out_cond_group_id INT,
    OUT out_status VARCHAR(100)
)
    SQL SECURITY INVOKER
start_proc:BEGIN
    
    DECLARE output_string, token_name, token_value VARCHAR(100);
    DECLARE m_cond_group_name TEXT;
    DECLARE first BOOL DEFAULT TRUE;
    DECLARE counter INT DEFAULT 1;
    DECLARE cond_cnt INT DEFAULT 0;
    DECLARE m_group_id INT;
    DECLARE m_where_clause TEXT DEFAULT '';
    DECLARE m_count INT;
    DECLARE not_found INT DEFAULT 0;

    DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
        ROLLBACK;
        
        DROP TEMPORARY TABLE IF EXISTS temp_cond_tokens;
        SET out_status = 'FAILURE. SQL Exception.';
    END;
    
    DECLARE CONTINUE HANDLER FOR 1329 SET not_found = 1;

    
    SET first = TRUE;
    SET m_cond_group_name = '';

    CREATE TEMPORARY TABLE IF NOT EXISTS temp_cond_tokens (
        t_name VARCHAR(100),
        t_value VARCHAR(100),
        t_pair VARCHAR(201)
    ) ENGINE=MEMORY;
    DELETE FROM temp_cond_tokens;

    loop_1: WHILE counter <= LENGTH(in_cond_str) DO
        IF LOCATE(',', in_cond_str, counter) = 0 THEN
            SET output_string = SUBSTRING(in_cond_str, counter);
            SET counter = LENGTH(in_cond_str) + 1;
        ELSE
            SET output_string = SUBSTRING(in_cond_str, counter, LOCATE(',', in_cond_str, counter) - counter);
            SET counter = counter + LENGTH(output_string) + 1;
        END IF;

        SET token_name = TRIM(REGEXP_REPLACE(output_string, '(.*)=.*', '$1'));
        SET token_value = TRIM(REGEXP_REPLACE(output_string, '.*=(.*)', '$1'));

        IF token_name = token_value OR token_name = '' THEN
            ITERATE loop_1;
        END IF;

        
        SET token_name = REPLACE(token_name, "'", "''");
        SET token_value = REPLACE(token_value, "'", "''");

        INSERT INTO temp_cond_tokens VALUES (token_name, token_value, CONCAT(token_name, ':', token_value));

        IF first THEN
            SET m_cond_group_name = CONCAT(token_name, ':', token_value);
            SET m_where_clause = CONCAT("cond = ", QUOTE(CONCAT(token_name, ':', token_value)));
        ELSE
            SET m_cond_group_name = CONCAT(m_cond_group_name, ',', token_name, ':', token_value);
            SET m_where_clause = CONCAT(m_where_clause, " OR cond = ", QUOTE(CONCAT(token_name, ':', token_value)));
        END IF;
        
        SET first = FALSE;
        SET cond_cnt = cond_cnt + 1;
    END WHILE;

    IF first THEN
        SET out_cond_group_id = 0;
        SET out_status = 'FAILURE. Invalid input.';
        DROP TEMPORARY TABLE IF EXISTS temp_cond_tokens;
        LEAVE start_proc;
    END IF;

    
    SET @sql_str = CONCAT('SELECT COND_GROUP_ID INTO @m_group_id FROM cond_view WHERE COND_GROUP_ID IN (',
                          'SELECT COND_GROUP_ID FROM cond_view GROUP BY COND_GROUP_ID HAVING COUNT(*) = ', cond_cnt, 
                          ') AND (', m_where_clause, ') GROUP BY COND_GROUP_ID HAVING COUNT(*) = ', cond_cnt, ' LIMIT 1');

    START TRANSACTION;
    
    SET @m_group_id = NULL;
    PREPARE stmt FROM @sql_str;
    EXECUTE stmt;
    DEALLOCATE PREPARE stmt;

    IF @m_group_id IS NOT NULL THEN
        SET out_cond_group_id = @m_group_id;
        SET out_status = 'SUCCESS. Found existing.';
    ELSE
        
        INSERT INTO cond_group_master (cond_group_name) VALUES (m_cond_group_name);
        SET m_group_id = LAST_INSERT_ID();
        
        
        INSERT IGNORE INTO cond_master (cond_name, cond_value, cond_description)
        SELECT t_name, t_value, 'Auto-added via automation'
        FROM temp_cond_tokens;

        
        INSERT INTO cond_group (cond_group_id, cond_master_id)
        SELECT m_group_id, cm.id
        FROM temp_cond_tokens t
        JOIN cond_master cm ON t.t_name = cm.cond_name AND t.t_value = cm.cond_value;
        
        SET out_cond_group_id = m_group_id;
        SET out_status = 'SUCCESS. Created new.';
    END IF;

    COMMIT;
    DROP TEMPORARY TABLE IF EXISTS temp_cond_tokens;
END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;

--
-- Final view structure for view `cond_view`
--

/*!50001 DROP VIEW IF EXISTS `cond_view`*/;
/*!50001 SET @saved_cs_client          = @@character_set_client */;
/*!50001 SET @saved_cs_results         = @@character_set_results */;
/*!50001 SET @saved_col_connection     = @@collation_connection */;
/*!50001 SET character_set_client      = utf8mb3 */;
/*!50001 SET character_set_results     = utf8mb3 */;
/*!50001 SET collation_connection      = utf8mb3_general_ci */;
/*!50001 CREATE ALGORITHM=UNDEFINED */
/*!50013 DEFINER=`sispara`@`localhost` SQL SECURITY DEFINER */
/*!50001 VIEW `cond_view` AS select `G`.`cond_group_id` AS `COND_GROUP_ID`,`G`.`cond_master_id` AS `COND_ID`,`M`.`cond_name` AS `COND_NAME`,`M`.`cond_value` AS `COND_VALUE`,concat(`M`.`cond_name`,':',`M`.`cond_value`) AS `COND` from (`cond_group` `G` join `cond_master` `M` on(`M`.`id` = `G`.`cond_master_id`)) order by `G`.`cond_group_id`,`G`.`id` */;
/*!50001 SET character_set_client      = @saved_cs_client */;
/*!50001 SET character_set_results     = @saved_cs_results */;
/*!50001 SET collation_connection      = @saved_col_connection */;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*M!100616 SET NOTE_VERBOSITY=@OLD_NOTE_VERBOSITY */;

-- Dump completed on 2026-05-04  9:51:16
