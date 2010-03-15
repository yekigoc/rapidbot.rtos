#ifndef __DEBUG_MACROS_H__
#define __DEBUG_MACROS_H__

#ifndef NOERR
#define perr(...) {fprintf(stdout, "error %s: %i - ", __FILE__, __LINE__); fprintf(stdout,  __VA_ARGS__); fprintf(stdout, "\n");}
#else
#define perr(...)
#endif

#ifndef NOWAR
#define pwar(...) {fprintf(stdout, "warning %s: %i - ", __FILE__, __LINE__); fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n");}
#else
#define pwar(...)
#endif

#ifndef NOMSG
#define pmsg(...) {fprintf(stdout, "%s: %i - ", __FILE__, __LINE__); fprintf(stdout, __VA_ARGS__); fprintf(stdout, "\n");}
#else
#define pmsg(...)
#endif

#endif
