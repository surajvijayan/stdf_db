# STDF_DB: High-Performance STDF to Relational Database Ingestor

Developed by Suraj Vijayan

Initial release: Feb. 15th, 2020  
Licensed under Apache License 2.0

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
- **MySQL Connector/C++:** v8.x (or later) for database interaction. Note: v1.1.x is deprecated.
- **libconfig++:** For configuration file parsing.
- **OpenSSL:** For SHA256 hashing functionality.
- **Pthreads:** For multi-threaded orchestration.

## Building
The project uses GNU autotools for portable builds. The root directory contains `configure.ac` and `Makefile.am`, and `src/Makefile.am` defines the source build targets. Ensure dependencies are installed, then run:
```bash
autoreconf -fiv
./configure
make
make install  # optional
```
`make install` installs binaries and supporting files to the configured prefix, which defaults to `/usr/local` unless overridden with `./configure --prefix=/your/path`.
This detects libraries automatically and builds the `stdf_db` executable and `cond_simulate` utility.

## Configuration (`.cfg`)
`stdf_db` reads a libconfig-style configuration file, for example `src/stdf_db.cfg`. Pass the config file path as the first argument:
```bash
./stdf_db src/stdf_db.cfg <stdf_file>
```

Key sections in `stdf_db.cfg`:
- `site_id`: numeric site id. This should be setup in the site_master table.
- `verbose`: log verbosity level (0 = quiet, 2 = debug).
- `validate_stdf`: if set to `1`, the application performs a pre-flight STDF validation pass.
- `max_parts_per_thread`: number of PIR parts assigned to each worker thread.
- `max_threads_limit`: upper limit on worker thread count.
- `use_threads`: enables parallel ingestion when set to `1`.

Database connection settings live under the `mysql` block:
- `mysql_server`
- `mysql_user`
- `mysql_pass`
- `mysql_db`
These values may be overridden by environment variables for automation and security.

Test-record controls are defined under `test_rec`:
- `ignore_passed_ptr`: skip passing parametric records.
- `ignore_passed_ftr`: skip passing functional records.
- `skip_rec_types`: array of STDF record names to ignore, such as `REC_PLR` or `REC_DTR`.
- `filter_parametric_tests`: list of test-name prefixes to include for PTR ingestion.

This configuration enables flexible filtering, selective record loading, and tuned threading behavior for high-volume STDF ingestion.

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

## Contributing
Contributions are welcome! Please submit issues or pull requests on the project repository. All contributions are licensed under Apache License 2.0.

## License
This project is licensed under the Apache License 2.0 - see the [LICENSE](LICENSE) file for details.
