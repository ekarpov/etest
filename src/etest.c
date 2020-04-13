/*
    Test helpers
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

#include "etest.h"

/* static data */
static etest_t      __etest;

/* static buffers */
static char         __file_name_buff[FILENAME_MAX];
static char         __message_buffer[2048];
static char         __time_buffer[512];

/*----------------------------------------------------------------------*/
/* helpers */
const char* _etest_format_time()
{
    time_t rawtime;
    struct tm * timeinfo;

    /* current time */ 
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    /* format time */
    sprintf(__time_buffer, "%.2d:%.2d:%.2d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    return __time_buffer;
}

/*----------------------------------------------------------------------*/
/* default log function */
void    _etest_log(void* data, const char* message) 
{
    FILE* fout = 0;
    int trace_messages;
    const char* time_message = 0;

    assert(message);
    if(message == 0) return;

    /* check if we need to trace message on screen as well */
    trace_messages = etest_instance()->settings.trace_messages;

    /* check if log file is set */
    if(etest_instance()->settings.log_file)
    {
        /* format log to output folder */
        if(etest_instance()->settings.output_dir)
        {
            sprintf(__file_name_buff, "%s/%s", etest_instance()->settings.output_dir, etest_instance()->settings.log_file);
        } else
        {
            sprintf(__file_name_buff, "%s", etest_instance()->settings.log_file);
        }

        /* open log file */
        fout = fopen(__file_name_buff, "a+");
    }

    /* make sure we log messages somewhere */
    if(fout == 0)
    {
        trace_messages = 1;
    }

    /* format time */
    time_message = _etest_format_time();

    /* report message */
    if(fout)
    {
        fprintf(fout, "[%s]: %s\n", time_message, message);
    }

    /* report message to screen as well if needed */
    if(trace_messages)
    {
        printf("[%s]: %s\n", time_message, message);
    }

    /* close file */
    if(fout != 0)
    {
        fclose(fout);
    }
}

/*----------------------------------------------------------------------*/
/* ELibc trace callback support */
void etest_elibc_trace_callback(const char* msg, void* callback_param)
{
    assert(msg);
    if(msg == 0) return;

    /* pass to log function */
    etest_instance()->log_func(etest_instance()->log_data, msg);
}

/*----------------------------------------------------------------------*/

/* etest instance */
etest_t*    etest_instance()
{
    return &__etest;
}

/* init settings from command line parameters */
void	etest_settings_init(etest_settings_t* etest_settings, int argc, char* argv[])
{
    // TODO:
    // --input_folder="c:\temp" --output_folder="c:\temp" --log_file="tests.log"
}

/* running tests */
void    etest_init(const char* name, int argc, char* argv[])
{
    assert(name);

    /* reset global instance */
    memset(etest_instance(), 0, sizeof(etest_t));

    /* copy name */
    etest_instance()->name = name;

    /* default parameters */
    etest_instance()->log_func = _etest_log;
    etest_instance()->log_data = 0;

    /* default settings */
    etest_instance()->settings.trace_messages = 1;

    /* init settings from command line parameters */
    if(argc > 0 && argv != 0)
    {
        etest_settings_init(&etest_instance()->settings, argc, argv);
    }

    ETEST_LOG("=== RUNNING %s ===\n", name);
}

void    etest_run_test(const char* test_name, etest_run_func test_func)
{
    int result;

    ETEST_LOG("-> %s ", test_name);

    /* run test */
    result = test_func();

    if(result == ETEST_SUCCESS)
    {
        etest_instance()->stats.success_count++;
        ETEST_LOG("<- %s: SUCCESS", test_name);
    } else 
    {
        etest_instance()->stats.failure_count++;
        ETEST_LOG("<- %s: FAILED", test_name);
    }    
}

void    etest_run_suite(const char* suite_name, etest_run_func suite_func)
{
    /* copy stats */
    etest_stats_t stats = etest_instance()->stats;

    ETEST_LOG("=> %s", suite_name);
    
    suite_func();

    ETEST_LOG("<= %s DONE, SUCCESS(%d), FAILED(%d)\n", suite_name, 
        etest_instance()->stats.success_count - stats.success_count,
        etest_instance()->stats.failure_count - stats.failure_count);
}

void    etest_done()
{
    /* log test results */
    ETEST_LOG("=== %s DONE, SUCCESS(%d), FAILED(%d) ===",  
        etest_instance()->name,
        etest_instance()->stats.success_count,
        etest_instance()->stats.failure_count);

    /* check for memory leaks if enabled */
#ifdef _ETEST_ENABLE_PERFORMANCE_TOOLS
    // TODO:
    if(etest_check_memory_leaks())
    {
        ETEST_LOG(" *** MEMORY LEAKS DETECTED!!! *** ");
    }
#endif
}

/* logging */
void    etest_log_message(const char* msg, ...)
{
    va_list args;

    /* format message */
    va_start(args, msg);
    vsprintf(__message_buffer, msg, args);
    va_end(args);

    /* pass to log function */
    etest_instance()->log_func(etest_instance()->log_data, __message_buffer);
}

void    etest_log_assert_failed(const char* expr, const char* file, int line)
{
    /* format message */
    etest_log_message("\"%s\" assertion failed (in file %s on line %d)", expr, file, line);
}

void    etest_log_cmp_failed(const char* val_left, const char* val_right, const char* file, int line)
{
    /* format message */
    etest_log_message("%s is not equal to %s (in file %s on line %d)", val_left, val_right, file, line);
}

void    etest_log_cmp_not_failed(const char* val_left, const char* val_right, const char* file, int line)
{
    /* format message */
    etest_log_message("%s equals to %s (in file %s on line %d)", val_left, val_right, file, line);
}

/* tracning */
void    etest_trace_message(const char* msg, ...)
{
    va_list args;
    const char* time_message = 0;

    /* ignore if tracing is not enabled */
    if(etest_instance()->settings.trace_messages == 0) return;

    /* format message */
    va_start(args, msg);
    vsprintf(__message_buffer, msg, args);
    va_end(args);

    /* format time */
    time_message = _etest_format_time();

    /* output to screen */
    printf("[%s]: %s\n", time_message, __message_buffer);
}

/* load input files */
int     etest_load_file(const char* file_name, char** buffer_out, unsigned long long* size_out, const char* file, int line)
{
    FILE* ff;
    size_t data_read = 0;

    assert(file_name);
    assert(buffer_out);
    assert(size_out);
    if(file_name == 0 || buffer_out == 0 || size_out == 0)
    {
        etest_log_message("failed to load file, arguments are not valid (in file %s on line %d)", file, line);
        return ETEST_FAILED;
    }

    /* try to open file */
    ff = fopen(file_name, "rb");
    if(ff == 0)
    {
        etest_log_message("failed to open file %s (in file %s on line %d)", file_name, file, line);
        return ETEST_FAILED;
    }

    /* get file size */
    int size_read = 0;
    if(fseek(ff, 0, SEEK_END) == 0)
    {
        *size_out = ftell(ff);
        if(fseek(ff, 0, SEEK_SET) == 0)
        {
            /* mark flag */
            size_read = 1;
        }
    }

    /* check if size has been read */
    if(size_read == 0)
    {
        etest_log_message("failed to read files size for %s (in file %s on line %d)", file_name, file, line);
        fclose(ff);
        return ETEST_FAILED;
    }

    /* check if file is not empty */
    if(*size_out == 0)
    {
        etest_log_message("file is empty %s (in file %s on line %d)", file_name, file, line);
        fclose(ff);
        return ETEST_FAILED;
    }

    /* reserve memory */
    *buffer_out = (char*)malloc(*size_out);
    if(*buffer_out == 0)
    {
        etest_log_message("memory allocation failed (in file %s on line %d)", file, line);
        fclose(ff);
        return ETEST_FAILED;
    }

    /* read whole file */
    if(fread(*buffer_out, 1, *size_out, ff) != *size_out)
    {
        etest_log_message("failed to read file %s (in file %s on line %d)", file_name, file, line);
        free(*buffer_out);
        fclose(ff);
        *buffer_out = 0;
        return ETEST_FAILED;
    }

    /* close file */
    fclose(ff);

    return ETEST_SUCCESS;
}

/* exit status */
int     etest_exit_status()
{
    return (etest_instance()->stats.failure_count == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*----------------------------------------------------------------------*/
