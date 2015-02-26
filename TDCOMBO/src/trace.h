extern int g_nTraceLevel;
void 	CheckTraceLevel();

#define FTRACE  if (g_nTraceLevel >= 1) trace_to_file
#define FTRACE1 if (g_nTraceLevel >= 1) trace_to_file
#define FTRACE2 if (g_nTraceLevel >= 2) trace_to_file
#define FTRACE3 if (g_nTraceLevel >= 3) trace_to_file
#define FTRACE4 if (g_nTraceLevel >= 4) trace_to_file
#define FTRACE5 if (g_nTraceLevel >= 5) trace_to_file


#define VERIFY_RETURN(f)                                                        \
	    do                                                                      \
	    {                                                                       \
	    if (!(f) )                                                              \
            {                                                                   \
		    FTRACE("VERIFY_RETURN fired at %s, %d\n", __FILE__, __LINE__);      \
            DEBUG_BREAK;                                                    \
            return;                                                             \
            }                                                                   \
	    } while (0)

#define VERIFY_RETURN_VALUE(f, v)                                               \
	    do                                                                      \
	    {                                                                       \
	    if (!(f) )                                                              \
            {                                                                   \
		    FTRACE("VERIFY_RETURN fired at %s, %d\n", __FILE__, __LINE__);      \
            DEBUG_BREAK;                                                    \
            return v;                                                           \
            }                                                                   \
	    } while (0)




void trace_to_file(const char *format, ...);
