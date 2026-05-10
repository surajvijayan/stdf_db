# STDF Database Ingestor (stdf_db) - Technical Design Specification

## 1. Overview
`stdf_db` is an industrial-grade C++ utility engineered to ingest binary Standard Test Data Format (STDF) files into a MySQL/MariaDB relational database. The application is optimized for high-throughput environments, utilizing multi-threaded binary parsing, part-based file indexing, and multi-row database batching to achieve performance levels exceeding standard commercial yield management systems.

## 2. Architecture
The application follows a manager-worker architectural pattern, centered around a singleton orchestrator and multiple specialized worker threads.

### 2.1 Core Components
- **`ACTIVE_PARTS` (Singleton)**: The central manager. It handles configuration loading (via `libconfig++`), manages the global state (such as the STDF ID), and orchestrates the lifecycle of worker threads. It performs an initial indexing pass of the STDF file to enable parallel processing.
- **`THREAD_PROCESS` (Worker)**: Encapsulates the logic for processing a specific segment of an STDF file. Each worker maintains its own database connection and processes STDF records into relational data.
- **`stdf_cond` (Condition Parser)**: A logic module used to parse environmental test conditions (e.g., Temperature, Voltage) from `DTR` (Data Text Records) using optimized regex patterns. It ensures conditions are normalized and deduplicated in the database via the `process_cond_group` stored procedure.
- **`libstdf`**: A third-party library used for low-level binary parsing of STDF record structures (https://freestdf.sourceforge.io/doxygen/).

## 3. Data Processing Pipeline

### 3.1 Initialization and Indexing
The system initializes by loading settings from `stdf_db.cfg`. Before spawning workers, the `ACTIVE_PARTS` singleton performs a "light" pass over the STDF file to identify `PIR` (Part Information Record) boundaries. It records file offsets for every $N$ parts (defined by `max_parts_per_thread`), allowing workers to "jump" to their assigned file locations.

### 3.2 Parallel Ingestion
Worker threads are spawned and signaled via a condition variable (`threads_ready`). Each thread:
1.  Connects to the database with a private session.
2.  Seeks to its assigned offset in the STDF file.
3.  Parses records sequentially until it reaches the offset of the next thread or the end of the file.
4.  Accumulates test results in memory.

### 3.3 Condition Normalization (COND Logic)
Environmental variables (e.g., `COND: SITE[5], VDDP=0.745`) are extracted from `DTR` records. This normalization is critical for PVT (Process, Voltage, Temperature) analysis. The application:
1.  Parses the strings using regular expressions.
2.  Normalizes tokens into a sorted, comma-delimited string.
3.  Uses the `process_cond_group` stored procedure to map tokens to a unique `cond_group_id`. This procedure utilizes temporary memory tables and set-based logic for high-performance mapping.
4.  Associates this ID with parametric (`PTR`) and functional (`FTR`) results for downstream analysis.

## 4. Database Schema Design
The schema leverages both `InnoDB` for transactional metadata and (optionally) `ColumnStore` for high-volume test data:
- **Meta-Information**: `mir` (Lot context), `far` (File version), `atr` (Audit trail).
- **Part/Test Tracking**: `pir` (Part start), `prr` (Part results).
- **Result Data**: `ptr` (Parametric Results), `ftr` (Functional Results), `tsr` (Synopsis).
- **Normalization**: `cond_master` and `cond_group_master` manage the many-to-many relationship between tests and environmental variables.

### 4.1 Detailed Table Structure
The database schema includes the following tables, based on STDF record types:

- **mir (Master Information Record)**: Stores lot-level metadata (e.g., setup time, lot ID, test program). Primary key `id`, with unique hash for duplicate detection.
- **far (File Attributes Record)**: File format info (CPU type, STDF version).
- **atr (Audit Trail Record)**: Modification logs.
- **pir (Part Information Record)**: Part start markers, linked to MIR.
- **mrr (Master Results Record)**: Lot completion data.
- **pcr (Part Count Record)**: Aggregate counts per head/site.
- **hbr/sbr (Hardware/Software Bin Records)**: Bin definitions.
- **wir/wrr/wcr (Wafer Records)**: Wafer information and results.
- **prr (Part Results Record)**: Individual part outcomes (hard/soft bins, coordinates).
- **ptr (Parametric Test Result)**: Numeric test results, linked to PRR and condition groups.
- **ftr (Functional Test Result)**: Pass/fail test results, linked to PRR and condition groups.
- **tsr (Test Synopsis Record)**: Aggregated test statistics.
- **dtr (Data Text Record)**: Free-form text data.
- **stdf_files**: Tracks processed files.
- **sdr (Site Description Record)**: Tester site configurations.
- **pmr/pgr/plr (Pin Records)**: Pin mapping and groups.
- **Condition Tables**: `cond_group_master`, `cond_master`, `cond_group` for normalizing environmental variables.

All tables use `InnoDB` engine with `latin1` charset and case-sensitive collation for STDF compatibility. Foreign keys ensure referential integrity with CASCADE deletes.

### 4.2 Stored Procedure: process_cond_group
The `process_cond_group` stored procedure handles dynamic condition grouping for environmental variables extracted from DTR records. It takes a comma-separated string of key-value pairs (e.g., "temp=25,voltage=3.3") and returns a unique `cond_group_id`.

#### Key Features:
- **Parsing**: Splits input string, trims, and escapes values.
- **Deduplication**: Checks existing groups via the `cond_view` (a view joining condition tables).
- **Insertion**: Creates new master conditions and groups if not found.
- **Performance**: Uses temporary memory tables and dynamic SQL for efficient set-based operations.
- **Error Handling**: Rolls back on exceptions, cleans up temp tables.

#### Usage:
```sql
CALL process_cond_group('temp=25,voltage=3.3', @group_id, @status);
```

This ensures conditions are normalized and linked to test results without redundant storage.

## 5. Development Utilities
### 5.1 `cond_simulate.cpp`
A standalone sandbox utility used to validate regex patterns in `stdf_cond.cpp`. It allows developers to verify that new STDF logging formats are correctly parsed into canonical condition strings without requiring a full database or file system environment.

## 6. Performance Optimizations (The "Secret Sauce")

### 6.1 Multi-Row Batch Ingestion
The primary bottleneck in database ingestion is network round-trips. `stdf_db` minimizes this by accumulating test results for an entire part and executing a single, large multi-row `INSERT` statement (e.g., `INSERT ... VALUES (?,?,...), (?,?,...), ...`) at the `PRR` boundary.

### 6.2 Session-Level Tuning
Threads use `SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED` and disable `AutoCommit`. Transactions are committed manually after processing batches of parts or DTR records to balance safety with write performance.

### 6.3 Thread-Independent DB Sessions
Each thread creates its own `sql::Connection`. This avoids global locks in the MySQL connector and allows the database server to process 20+ streams of data in parallel.

### 6.4 Strategic Record Skipping
The ingestion engine allows users to skip heavy record types (like `PTR` or `DTR`) via the `skip_rec_types` list in the config file. This can also be used to disable `COND` processing by skipping `REC_DTR`.

## 7. Robustness and Security
- **SQL Injection Prevention**: All database interactions use `sql::PreparedStatement` with parameterized inputs.
- **Credential Management**: Supports environment variables (`STDF_DB_USER`, `STDF_DB_PASS`, etc.) to keep sensitive credentials out of version-controlled `.cfg` files.
- **Thread Safety**: Uses the Double-Checked Locking pattern for the `ACTIVE_PARTS` singleton and mutexes (`stdf_lock1`) for shared library initialization.
- **Validation**: An optional `validate_stdf` pass checks for the mandatory STDF record sequence (`FAR` -> `MIR` -> `MRR`) before ingestion starts.

## 8. Technical Specifications
- **Language**: C++14 (`-std=c++1y`)
- **Libraries**: `mysql-connector-cpp` (v1.1.x), `libstdf`, `openssl` (SHA256), `libconfig++`, `pthread`.
- **Database**: MySQL 5.7+ or MariaDB 10.3+.