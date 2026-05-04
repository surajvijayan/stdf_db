# STDF_DB: High-Performance STDF to Relational Database Ingestor

Developed by Suraj Vijayan
Latest Update: May 2026

## Overview
`stdf_db` is an industrial-grade C++ utility engineered to ingest binary Standard Test Data Format (STDF) files into MySQL/MariaDB. It is optimized for high-throughput semiconductor manufacturing environments, utilizing parallel binary parsing and multi-row database batching to outperform standard commercial yield management systems.

## Key Features

### 1. Parallel Ingestion & Robust Indexing
The application splits a single STDF file into logical chunks based on Part Information Records (PIR). 
- **Robust Indexing:** The orchestrator performs a high-speed indexing pass, calculating logical offsets (`4 bytes header + REC_LEN`) to ensure worker threads land exactly at record boundaries, bypassing library-level buffering issues.
- **Parallel Processing:** Worker threads process assigned part ranges in parallel, each maintaining its own database connection and transaction state.
- **Database Connection Pooling:** Each thread maintains its own database connection and transaction state.

### 2. Multi-Row Batch Optimization
To eliminate the network bottleneck of individual record insertions, `stdf_db` implements multi-row `INSERT` batching:
- **PTR/FTR Batching:** Parametric and Functional results are accumulated in memory and flushed as a single multi-row query at the Part Results Record (PRR) boundary.
- **Performance Gain:** Reduces database round-trips by up to 30,000x for PTR records, resulting in significant ingestion speedups.

### 3. Duplicate Detection & Security
- **SHA256 Hashing:** Generates a unique fingerprint of the Master Information Record (MIR). Matching hashes in the database trigger an immediate abort to prevent redundant data ingestion.
- **Parameterized Queries:** All database interactions utilize `sql::PreparedStatement` to eliminate SQL injection vectors.
- **Credential Protection:** Supports environment variable overrides (`STDF_DB_USER`, `STDF_DB_PASS`, etc.) to keep sensitive secrets out of configuration files.

### 4. Environmental Condition (COND) Tracking
A unique feature of this tool is its ability to parse **DTR (Data Text Records)** for test conditions.
- **Regex Parsing:** It identifies patterns like `COND:`, `COND_VAR:`, and `SITE[n]` to extract voltage, temperature, or other hardware settings.
- **Condition Groups:** Conditions are normalized, sorted, and sent to a stored procedure (`process_cond_group`). This returns a `cond_group_id` which is then associated with every Parametric (PTR) and Functional (FTR) test record.
- **Cache Optimization:** It uses an internal cache to avoid redundant database calls for identical condition strings.

## Dependencies
- **libstdf:** Standard STDF parsing library (https://freestdf.sourceforge.io/doxygen/).
- **MySQL Connector/C++:** (v1.1.x) For database interaction.
- **libconfig++:** For configuration file parsing.
- **OpenSSL:** For SHA256 hashing functionality.
- **Pthreads:** For multi-threaded orchestration.

## Configuration (`.cfg`)
Settings are managed via `stdf_db.cfg`, allowing for record skipping, test filtering, and thread limit adjustments.

## Usage
```bash
./stdf_db <config_file> <stdf_file>
```

### Environment Variable Overrides
For security and automation, the following environment variables override the config file:
- `STDF_DB_USER`
- `STDF_DB_PASS`
- `STDF_DB_NAME`
- `STDF_DB_HOST`

## Database Schema Overview
The application expects a schema containing tables for each major record type (`mir`, `pir`, `ptr`, `ftr`, `mrr`, etc.). 
- **Transactions:** Data is loaded using batch inserts where possible.
- **Isolation:** Transactions are set to `READ COMMITTED` at the session level to optimize write performance during high-concurrency loads.

## Error Handling
The application implements comprehensive `try-catch` blocks for:
- `sql::SQLException`: Database connectivity or query errors.
- `libconfig::SettingNotFoundException`: Configuration errors.
- `FileIOException`: STDF file access issues.
In the event of a failure, threads attempt to `rollback()` the current transaction to ensure data integrity.
