/*
** ==================================================================
** Ian McIntosh - additional defines and prototypes for port to WinCE.
** ===================================================================
*/

#if !defined(_LWINCE)
#define _LWINCE


#include <windef.h>
#include <winbase.h>
#include "locale.h"

// for implementation of missing runtime functions
#if defined(LUA_BUILD_AS_DLL)
#pragma message("Building with dllexport")
#define RTEXP extern __declspec(dllexport)
#else
#pragma message("Building with dllimport")
#define RTEXP __declspec(dllimport)
#endif

// required for Pocket PC 2002 - not defined on this platform

#ifndef _CLOCK_T_DEFINED
typedef long clock_t;
#define _CLOCK_T_DEFINED
#endif

#ifndef _TM_DEFINED
struct tm {
	int tm_sec;	/* seconds after the minute - [0,59] */
	int tm_min;	/* minutes after the hour - [0,59] */
	int tm_hour;	/* hours since midnight - [0,23] */
	int tm_mday;	/* day of the month - [1,31] */
	int tm_mon;	/* months since January - [0,11] */
	int tm_year;	/* years since 1900 */
	int tm_wday;	/* days since Sunday - [0,6] */
	int tm_yday;	/* days since January 1 - [0,365] */
	int tm_isdst;	/* daylight savings time flag */
	};
#define _TM_DEFINED
#endif

#define CLOCKS_PER_SEC	1000

#define ptrdiff_t	int

#define BUFSIZ		512
#define _IOFBF		0x0000
#define _IOLBF		0x0040
#define _IONBF		0x0004
#define L_tmpnam	MAX_PATH

#define SECONDS_IN_DAY			86400
#define SECONDS_IN_YEAR			31536000
#define SECONDS_IN_LEAP_YEAR	31622400

#ifndef _TIME_T_DEFINED
typedef long time_t;		/* time value */
#define _TIME_T_DEFINED 	/* avoid multiple def's of time_t */
#endif
typedef void FILE;

/*
** EXPORTED VARIABLES
** 'Current directory' - not supported as standard in WinCE
** Overridden version of fopen() to use "current directory" (e.g. as required by Kepler cgilua)
*/
RTEXP	char	luaCurrentDirectory[MAX_PATH];
RTEXP	FILE	*_ian_fopen( const char *filename, const char *mode );

/*
** replacement for fgets(stdin) - implemented in Seashell.c
*/
#ifdef SEASHELL
extern __declspec(dllexport) char *readline_lua(char *b, int nMax, const char *prompt);
#else
__declspec(dllimport) char *readline_lua(char *b, int nMax, const char *prompt);
#endif


/*
** Emulate support for a "current directory"
*/
RTEXP	int chdir( const char *dirname );
RTEXP	char *getcwd( char *buffer, int maxlen );
RTEXP	int _mkdir(const char *dirname);
RTEXP	int	rmdir(const char *path);


/* Since WinCE returns formatted message strings in wide-char (unicode)		*/
/* we must translate these to multi-byte ANSI strings before returning		*/
/* them to Lua.																*/
/* Use static string buffers here for simplicity.							*/
#define ERRMSG_MAX_LEN	80

RTEXP	char *strerror( int errnum );
/*RTEXP	int _isatty( void* filenum );*/
RTEXP	time_t mktime( struct tm *ptm );
RTEXP	struct tm *tm_struct_from_systime( SYSTEMTIME *systime );
RTEXP	int strcoll( const char *string1, const char *string2 );
RTEXP	char *tmpnam( char *string );
RTEXP	FILE *tmpfile( void );
RTEXP	FILE *_popen( const char *command, const char *mode );
RTEXP	int _pclose( FILE *stream );
RTEXP	int remove( const char *path );
RTEXP	int rename( const char *oldname, const char *newname );
RTEXP	FILE *freopen( const char *path, const char *mode, FILE *stream );
RTEXP	char *getenv( const char *varname );
RTEXP	int system( const char *command );
RTEXP	int setvbuf( FILE *stream, char *buffer, int mode, size_t size );

RTEXP	char *setlocale( int category, const char *locale );
RTEXP	struct lconv *localeconv( void );


/* Only Unicode (W) functions available in coredll.lib so define ASCII equivalents */
/* Must define COREDLL in project options to avoid dll linkage warnings...         */
/* See definitions of these API functions in winbase.h                             */

RTEXP	HINSTANCE LoadLibraryA(const char *path);
RTEXP	DWORD FormatMessageA(DWORD dwFlags,LPCVOID lpSource,DWORD dwMessageId,DWORD dwLanguageId,
					 char *lpBuffer,DWORD nSize,va_list *Arguments);
RTEXP	DWORD GetModuleFileNameA( HMODULE hModule, char* lpFilename, DWORD nSize );


#endif		// _LWINCE
