/*
*  Simple C code unit test framework
*/

#ifndef _ETEST_H_INCLUDED_
#define _ETEST_H_INCLUDED_

/* allow using in C++ */
#ifdef __cplusplus
extern "C" {
#endif 

/*----------------------------------------------------------------------*/

#define ETEST_SUCCESS       1
#define ETEST_FAILED        0

/*----------------------------------------------------------------------*/

/* settings */
typedef struct
{
    const char*		input_dir;
    const char*		output_dir;
    const char*		log_file;

    int             trace_messages;

} etest_settings_t;

/* test statistics */
typedef struct
{
    unsigned long	success_count;
    unsigned long	failure_count;

} etest_stats_t;

/* test function */
typedef int		(*etest_run_func)();

/* log function */
typedef void    (*etest_log_func)(void*, const char*);

/* test state */
typedef struct
{
    const char*         name;

    etest_stats_t		stats;
    etest_settings_t	settings;

    etest_log_func		log_func;
    void*               log_data;

} etest_t;

/*----------------------------------------------------------------------*/

/* etest instance */
etest_t*    etest_instance();

/* init settings from command line parameters */
void	etest_settings_init(etest_settings_t* etest_settings, int argc, char* argv[]);

/* running tests */
void    etest_init(const char* name, int argc, char* argv[]);
void    etest_run_test(const char* test_name, etest_run_func test_func);
void    etest_run_suite(const char* suite_name, etest_run_func suite_func);
void    etest_done();

/* logging */
void    etest_log_message(const char* msg, ...);
void    etest_log_assert_failed(const char* expr, const char* file, int line);
void    etest_log_cmp_failed(const char* val_left, const char* val_right, const char* file, int line);
void    etest_log_cmp_not_failed(const char* val_left, const char* val_right, const char* file, int line);

/* tracning */
void    etest_trace_message(const char* msg, ...);

/* load input files */
int     etest_load_file(const char* file_name, char** buffer_out, unsigned long long* size_out, const char* file, int line);

/* exit status */
int     etest_exit_status();

/*----------------------------------------------------------------------*/
/* ELibc support */
void    etest_elibc_trace_callback(const char* msg, void* callback_param);

/*----------------------------------------------------------------------*/

/* running tests */
#define ETEST_INIT(name)                    etest_init((name), 0, 0)
#define ETEST_INIT_CMD                      etest_init
#define ETEST_RUN(test_func)                etest_run_test(#test_func, (test_func))
#define ETEST_RUN_SUITE(suite_func)         etest_run_suite(#suite_func, (suite_func))
#define ETEST_DONE                          etest_done

/* logging */
#define ETEST_LOG                           etest_log_message

/* tracing */
#define ETEST_TRACE                         etest_trace_message

/* test settings */
#define ETEST_SET_LOG_FUNC(_func, _data) \
    { \
        etest_instance()->log_func = (_func);\
        etest_instance()->log_data = (_data);\
        etest_instance()->settings.log_file = 0\
    }

#define ETEST_SET_LOG_FILE(_file_name) \
    { \
        etest_instance()->log_func = 0;\
        etest_instance()->log_data = 0;\
        etest_instance()->settings.log_file = (_file_name)\
    }

#define ETEST_SET_INPUT_FOLDER(_folder)     etest_instance()->settings.input_dir = (_folder)
#define ETEST_SET_OUTPUT_FOLDER(_folder)    etest_instance()->settings.output_dir = (_folder)

/* process exit status */
#define ETEST_EXIT_STATUS                   etest_exit_status

/*----------------------------------------------------------------------*/

/* assert expression */
#define ETEST_ASSERT(_expr) \
    { \
        if(!(_expr)) \
        { \
            etest_log_assert_failed(#_expr, __FILE__, __LINE__); \
            return ETEST_FAILED; \
        } \
    }

/* compare values */
#define ETEST_ASSERT_EQUALS(_val1, _val2) \
    { \
        if((_val1) != (_val2)) \
        { \
            etest_log_cmp_failed(#_val1, #_val2, __FILE__, __LINE__); \
            return ETEST_FAILED; \
        } \
    }

#define ETEST_ASSERT_NOT_EQUALS(_val1, _val2) \
    { \
        if((_val1) == (_val2)) \
        { \
            etest_log_cmp_not_failed(#_val1, #_val2, __FILE__, __LINE__); \
            return ETEST_FAILED; \
        } \
    }

#define ETEST_ASSERT_EQUALS_CMP(_val1, _val2, _cmpfunc) \
    { \
        if(_cmpfunc((_val1), (_val2)) != 0) \
        { \
            etest_log_cmp_failed(#_val1, #_val2, __FILE__, __LINE__); \
            return ETEST_FAILED; \
        } \
    }

#define ETEST_ASSERT_EQUALS_BIN(_val1, _val2, _size) \
    { \
        if(ememcmp((_val1), (_val2), (_size)) != 0) \
        { \
            etest_log_cmp_failed(#_val1, #_val2, __FILE__, __LINE__); \
            return ETEST_FAILED; \
        } \
    }

/* known value types */
#define ETEST_ASSERT_EQUALS_STRING(_val1, _val2) ETEST_ASSERT_EQUALS_CMP((_val1), (_val2), estrcmp)
#define ETEST_ASSERT_EQUALS_STRINGW(_val1, _val2) ETEST_ASSERT_EQUALS_CMP((_val1), (_val2), ewcscmp)

/*----------------------------------------------------------------------*/

/* load input file */
#define ETEST_LOAD_INPUT_FILE(_file_name, _buffer, _buffer_size) \
    { \
        if(etest_load_file((_file_name), (_buffer), (_buffer_size), __FILE__, __LINE__) != ETEST_SUCCESS) \
        { \
            return ETEST_FAILED; \
        } \
    }

/*----------------------------------------------------------------------*/

/* allow using in C++ */
#ifdef __cplusplus
}
#endif 

#endif /* _ETEST_H_INCLUDED_ */


