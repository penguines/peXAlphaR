#define ALPHA_DEBUG
#define ALPHA_SHOWLOG

#ifdef ALPHA_DEBUG
#	define DEBUGLOG(fmt, ...) ((void)printf(\
	"[File: %s][Line: %d] " fmt "\n", __FILE__, __LINE__, __VA_ARGS__))

#else

#	define DEBUGLOG(fmt, ...) ((void)0)

#endif

#ifdef ALPHA_SHOWLOG
#	define PRINTLOG(fmt, ...) ((void)printf(\
	"[Bot]> " fmt "\n", __VA_ARGS__))

#else

#	define PRINTLOG(fmt, ...) ((void)0)

#endif