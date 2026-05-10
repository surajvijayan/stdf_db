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

#include <active_parts.h>
#include <cppconn/exception.h>
#include <exception>
#include <iostream>
#include <libconfig.h++>
#include <libstdf.h>
#include <pthread.h>

using namespace libconfig;
using namespace std;

pthread_mutex_t stdf_lock1 = PTHREAD_MUTEX_INITIALIZER;

// Forward declaration of THREAD_PROCESS is handled in the actual compilation
typedef class thread_process THREAD_PROCESS;

void *launch_pthread(void *thread_inp)
{
    THREAD_PROCESS *tp = static_cast<THREAD_PROCESS *>(thread_inp);

    if (!tp)
    {
        cerr << "ERROR: launch_pthread called with NULL thread_inp" << endl;
        return NULL;
    }

    try
    {
        tp->connect_db();
        tp->read_stdf(1);

        if (tp->f != NULL)
        {
            stdf_close(tp->f);
            tp->f = NULL; // prevent re-close
        }
        tp->con->close();
        delete tp->con;
        tp->con = NULL; // redundant but safe
        tp->driver->threadEnd();

        return NULL;
    }
    catch (sql::SQLException &e)
    {
        cerr << "# ERR1: SQLException in " << __FILE__ << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl
             << "# ERR1: " << e.what() << " (MySQL error code: " << e.getErrorCode()
             << ", SQLState: " << e.getSQLState() << " )" << endl;
        // Cleanup in exception handler
        if (tp->f != NULL)
        {
            stdf_close(tp->f);
            tp->f = NULL;
        }
        if (tp->con != NULL)
        {
            try
            {
                tp->con->rollback();
            }
            catch (...)
            {
                // Ignore rollback errors
            }
            tp->con->close();
            delete tp->con;
            tp->con = NULL;
        }
        if (tp->driver != NULL)
        {
            tp->driver->threadEnd();
        }
        return NULL;
    }
    catch (const FileIOException &fioex)
    {
        cerr << "I/O error while reading file." << endl;
        // Cleanup in exception handler
        if (tp->f != NULL)
        {
            stdf_close(tp->f);
            tp->f = NULL;
        }
        if (tp->con != NULL)
        {
            try
            {
                tp->con->rollback();
            }
            catch (...)
            {
                // Ignore rollback errors
            }
            tp->con->close();
            delete tp->con;
            tp->con = NULL;
        }
        if (tp->driver != NULL)
        {
            tp->driver->threadEnd();
        }
        return NULL;
    }
}
