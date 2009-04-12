/*
** ==================================================================
** Ian McIntosh - WinCE port of Lua 5.1
** ===================================================================
** Additional definitions and fuctions required by Lua (and Lua
** libraries) that are not available in base WinCE.
** ===================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "time.h"
#include "errno.h"			/* Custom version */


#include "lwince.h"

#include "../lua/lua.h"
#include "../lua/lauxlib.h"
#include "../lua/lualib.h"

/*
** Implementation of a "current directory" as used by override to fopen().
*/
extern __declspec(dllexport) char	luaCurrentDirectory[MAX_PATH] = "";

/*
** Reusable buffers for wide / multi-byte conversions (Unicode - ASCII)
** Shared by several functions so not persistent.
*/
wchar_t			wbuffer[BUFSIZ];
char			mbuffer[BUFSIZ];
static char		tmpFileName[MAX_PATH];


/*
** ==================================================================
** Ian McIntosh - additional functions for port to WinCE.
** ===================================================================
*/

/*
** Emulate support for a "current directory"
*/

RTEXP int chdir( const char *dirname ) {
	char	*ptr;

	if (GetFileAttributesA(dirname) != FILE_ATTRIBUTE_DIRECTORY) /* JASONSANTOS -- Added check to whether directory exists */
		return -1; /* dirname is not a valid directory */

	strcpy(luaCurrentDirectory, dirname);
	while((ptr=strchr(luaCurrentDirectory, '/')) != NULL)
		*ptr = '\\';
	if(luaCurrentDirectory[strlen(luaCurrentDirectory)-1] != '\\')
		strcat(luaCurrentDirectory, "\\");
	return 0;
}


RTEXP char *getcwd( char *buffer, int maxlen ) {
	if(buffer == NULL) {
		buffer = (char*)malloc(strlen(luaCurrentDirectory));
		strcpy(buffer, luaCurrentDirectory);
	}
	else {
		if((size_t)maxlen > strlen(luaCurrentDirectory))
			strcpy(buffer, luaCurrentDirectory);
		else
			return NULL;
	}
	return buffer;
};

RTEXP	int _mkdir(const char *dirname) {

	BOOL success = CreateDirectoryA(dirname, NULL);

	if (success!=0)
		return 0;

	DWORD err = GetLastError();

	if (err==ERROR_PATH_NOT_FOUND)
		errno=ENOENT;
	else if (err==ERROR_ALREADY_EXISTS)
		errno=EEXIST;
	else
		errno=EIO;
	return -1;
}

RTEXP	int	rmdir(const char *dirname) {
	BOOL success = RemoveDirectoryA(dirname);

	if (success!=0)
		return 0;

	DWORD err = GetLastError();

	if (err==ERROR_PATH_NOT_FOUND)
		errno=ENOENT;
	else if (err==ERROR_ACCESS_DENIED)
		errno=EPERM;
	else
		errno=EIO;
	return -1;
}


/*
** Overridden version of runtime fopen, that checks "current directory" if
** no explicit path supplied in filename.
** As possible future enhancement could also search a PATH variable.
*/
RTEXP FILE *_ian_fopen( const char *filename, const char *mode ) {
	char	fullpath[MAX_PATH];

	// if a path is specified use it
	if(strchr(filename, '\\') || strchr(filename, '/'))
		return(fopen(filename, mode));

	// otherwise try current directory
	sprintf(fullpath, "%s%s", luaCurrentDirectory, filename);
	return(fopen(fullpath, mode));
}


/*
** Provide substitute for ANSI tmpnam function.
** If called with *str parameter != NULL then stores temp filename
** in that buffer. Otherwise stores in static buffer tmpFileName and
** returns pointer to it.
**
** NOTE: WindowsCE has no concept of PATH environment variable...
**
** "The OS does not support the concept of a current directory.
** If a path to a file is not supplied along with the file name,
** the OS will look for the file in the \Windows directory as well
** as in the root of the file system. To access a file in any other
** path, the application must supply the absolute path to the file.
** In some cases, the GetModuleFileName function can supply the
** working directory of the currently running executable file."
**
** Since base OS functions seem not to be reliable and we must in
** any case supply the unique filename component, this function has
** been rewritten. Creates \Temp folder if it does not already exist.
** Better to use this location rather than \Windows or a subfolder as
** these are usually slow to browse, and temp files are not automatically
** deleted, so facilitates manual cleanup.
** Filename uses last 5 digits of tick count so is not guaranteed unique,
** but should be adequate for occasional use.
*/
RTEXP	char *tmpnam( char *str ) {
	char	num[16];
	char	*p = num;
	sprintf(num, "%X", GetTickCount());
	p = p + (strlen(num)-4);
	CreateDirectoryW(L"\\Temp", NULL);
	sprintf(tmpFileName, "\\Temp\\Lua%s.TMP", p);
	if(str != NULL) {
		strcpy(str, tmpFileName);
		return(str);
	}
	else return(tmpFileName);
}

/*
** Create temporary file. Should be automatically deleted when file
** is closed or program terminates normally - not yet implemented.
*/
RTEXP FILE *tmpfile() {
	tmpnam( NULL );
	return fopen( tmpFileName, "w+b" );
}


/*
** Provide substitute for ANSI strerror function.
** Returns pointer to an ANSI formatted string with last system error message.
*/
static char		errmsg[ERRMSG_MAX_LEN+1];
static wchar_t	werrmsg[ERRMSG_MAX_LEN+1];

RTEXP	char *strerror( int errnum ) {
	errno = (int)GetLastError();
	if(0 == FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				(LPCVOID)NULL, GetLastError(), (DWORD)NULL,	werrmsg, (DWORD)ERRMSG_MAX_LEN, NULL)) {
		wcscpy(werrmsg, L"Error text not available");
	}
	if(0 == WideCharToMultiByte(CP_ACP, (DWORD)NULL, werrmsg, -1, errmsg, ERRMSG_MAX_LEN, NULL, NULL)) {
		strcpy(errmsg, "Error text not available");
	}
	return(errmsg);
}

/*
** Compare strings : locale-specific
*/
RTEXP	int strcoll( const char *string1, const char *string2 ){

	wchar_t	wcs1[BUFSIZ];
	wchar_t	wcs2[BUFSIZ];
	int		nRet;

	MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, string1, -1, wcs1, BUFSIZ);
	MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, string2, -1, wcs2, BUFSIZ);

	nRet = CompareStringW(LOCALE_USER_DEFAULT,(DWORD)0,wcs1,-1,wcs2,-1);

	switch(nRet) {
		case CSTR_LESS_THAN :		return(-1);
		case CSTR_EQUAL :			return(0);
		case CSTR_GREATER_THAN :	return(1);
		default:					return(0);
	};
}

/*
** Implementation of _isatty() function
*//*
RTEXP	int _isatty( void* filenum ) {
	//return ((int)stdin);
	return (TRUE);
}
*/
/*
** Used by Lua io.popen - system independent and NOT available on
** this platform.
*/
RTEXP	FILE *_popen( const char *command, const char *mode ) {
	return NULL;
}
RTEXP	int _pclose( FILE *stream ){
	return -1;
}

/*
** Implementation of required functions from "time.h"

  Notes:

ANSI definitions:

typedef unsigned long  time_t;		seconds since 1970-01-01 00:00:00 (UTC)

struct tm {
	int tm_sec;		seconds after the minute - [0,59]
	int tm_min;		minutes after the hour - [0,59]
	int tm_hour;	hours since midnight - [0,23]
	int tm_mday;	day of the month - [1,31]
	int tm_mon;		months since January - [0,11]
	int tm_year;	years since 1900
	int tm_wday;	days since Sunday - [0,6]
	int tm_yday;	days since January 1 - [0,365]
	int tm_isdst;	daylight savings time flag
	};

WinCE definitions:

typedef struct _SYSTEMTIME {
WORD wYear;			full year
WORD wMonth;		month 1-12
WORD wDayOfWeek;	0 = Sunday
WORD wDay;			1 - 31
WORD wHour;			0 - 23
WORD wMinute;		0 - 59
WORD wSecond;		0 - 59
WORD wMilliseconds; 0 - 999
} SYSTEMTIME;


**
*/

/*
** The Gregorian calendar stipulates that a year that is evenly divisible by 100
** (for example, 1900) is a leap year only if it is also evenly divisible by 400
*/

int	IsLeap(int year) {
	if(year < 1582) return 0;		/* sorry - don't do pre-Gregorian! */
	if(year % 400 == 0) return 1;
	if(year % 100 == 0) return 0;
	return((year % 4 == 0 ? 1 : 0));
}

int DaysInMonth(int month, int year) {
	int	days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	if(month < 1 || month > 12) return(-1);
	if(month == 2 && IsLeap(year)) return(29);
	return days[month-1];
}

/*
** This is always going to be a bit of a hack (as is the very concept itself) since
** the rules vary by country and over time.
** We can blame William Willet 1856-1915 ('Bill the Builder' from Farnham).
** He proposed the idea in 1907. The bill was finally passed by Parliament and
** the practice implemented in 1916.
** We shall use the English rules in his honour, rather than Microsoft's USA rules.
** If the time concerned is "now" we can use the WinCE function:
** (GetTimeZoneInformation(&tz) == TIME_ZONE_ID_DAYLIGHT ? 1 : 0)
** but this may not be the case.
*/

int IsDaylightSaving(struct tm *when) {
	TIME_ZONE_INFORMATION tz;
	SYSTEMTIME now;
	int year = when->tm_year + 1900;
	int month = when->tm_mon + 1;

	GetLocalTime(&now);
	if(now.wYear == year && now.wMonth == month && now.wDay == when->tm_mday)
		return((GetTimeZoneInformation(&tz) == TIME_ZONE_ID_DAYLIGHT ? 1 : 0));

	if(year < 1916) return(0);
	if((month >= 3 && when->tm_mday > 22) &&
		(month <= 10 && when->tm_mday > 22)) return(1);

	return(0);
}

/*
** Convert from suppled tm structure to a time_t (seconds since 1970-01-01 00:00:00)
*/
RTEXP	time_t mktime( struct tm *ptm ) {

	time_t	t=0;
	int		y, m;
	int		year = ptm->tm_year + 1900;

	if(year < 1970) return((time_t)-1);

	y = 1970;
	while(y < year) t += (IsLeap(y++) ? SECONDS_IN_LEAP_YEAR : SECONDS_IN_YEAR);

	m = 1;
	while(m <= ptm->tm_mon) t += (SECONDS_IN_DAY * DaysInMonth(m++, year));

	t += (SECONDS_IN_DAY * (ptm->tm_mday -1));

	t += (3600 * ptm->tm_hour);
	t += (60 * ptm->tm_min);
	t += ptm->tm_sec;

	return t;
}

/*
** Convert from suppled SYSTEMTIME structure to a tm
*/
RTEXP	struct tm *tm_struct_from_systime( SYSTEMTIME *systime ) {
	static struct tm gtm;
	int m;

	gtm.tm_sec		= systime->wSecond;
	gtm.tm_min		= systime->wMinute;
	gtm.tm_hour		= systime->wHour;
	gtm.tm_mday		= systime->wDay;
	gtm.tm_mon		= systime->wMonth -1;
	gtm.tm_year		= systime->wYear - 1900;
	gtm.tm_wday		= systime->wDayOfWeek;

	gtm.tm_yday		= 0; m = 1;
	while(m < systime->wMonth) gtm.tm_yday += DaysInMonth(m++, systime->wYear);
	gtm.tm_yday += (systime->wDay -1);

	gtm.tm_isdst	= IsDaylightSaving(&gtm);

	return &gtm;
}

/*
** Convert from suppled tm structure to a SYSTEMTIME
*/
SYSTEMTIME *systime_from_tm_struct( const struct tm *stm ) {
	static SYSTEMTIME	syst;
	syst.wYear			= stm->tm_year + 1900;
	syst.wMonth			= stm->tm_mon +1;
	syst.wDayOfWeek		= stm->tm_wday;
	syst.wDay			= stm->tm_mday;
	syst.wHour			= stm->tm_hour;
	syst.wMinute		= stm->tm_min;
	syst.wSecond		= stm->tm_sec;
	syst.wMilliseconds	= 0;
	return &syst;
}

/*
** Return the current system time as time_t (elapsed seconds since
** 1970-01-01 00:00:00 in UTC)
** (and store it if pointer argument suppled)
*/

RTEXP	time_t time( time_t *timer ) {
	SYSTEMTIME systime;
	GetSystemTime( &systime );
	if(timer != NULL){
		*timer = mktime( tm_struct_from_systime( &systime ));
	}
	return mktime( tm_struct_from_systime( &systime ));
}

/*
** Convert a time_t (UTC) to a tm structure in UTC time.
** Lua only calls this once (from os_date).
*/
RTEXP	struct tm *gmtime( const time_t *timer )
{
	static struct tm		stm;
	SYSTEMTIME				syst;
	int						secs;
	int						tdays;

	secs = *timer;

	/* Calculate day of week. 1970-01-01 was a Thursday. (day 4 in SYSTEMTIME format)*/
	tdays = secs / SECONDS_IN_DAY;
	syst.wDayOfWeek = 4 + (tdays % 7);
	if(syst.wDayOfWeek > 6) syst.wDayOfWeek -= 7;

	syst.wYear = 1970;
	while(secs >= SECONDS_IN_YEAR) {
		if(IsLeap(syst.wYear) && secs >= SECONDS_IN_LEAP_YEAR) {
			secs -= SECONDS_IN_LEAP_YEAR;
			syst.wYear++;
		}
		else {
			secs -= SECONDS_IN_YEAR;
			syst.wYear++;
		}
	}
	syst.wMonth = 1;
	while(secs) {
		if(secs >= DaysInMonth(syst.wMonth, syst.wYear) * SECONDS_IN_DAY)
			secs -= (DaysInMonth(syst.wMonth++, syst.wYear) * SECONDS_IN_DAY);
		else break;
	}
	syst.wDay = 1;
	while(secs) {
		if(secs >= SECONDS_IN_DAY) { secs -= SECONDS_IN_DAY; syst.wDay++; }
		else break;
	}
	syst.wHour = 0;
	while(secs) {
		if(secs >= 3600) { secs -= 3600; syst.wHour++; }
		else break;
	}
	syst.wMinute = 0;
	while(secs) {
		if(secs >= 60) { secs -= 60; syst.wMinute++; }
		else break;
	}
	syst.wSecond = secs;
	syst.wMilliseconds = 0;

	return  tm_struct_from_systime( &syst );
}

/*
** Convert a time_t (UTC) to a tm structure in local time.
** Lua only calls this once (from os_date).
*/
RTEXP	struct tm *localtime( const time_t *timer )
{
	time_t					t_local;
	TIME_ZONE_INFORMATION	tz;
	t_local = *timer;
	/*
	** Conversion from UTC to local time (but only works for 'now')
	*/
	switch(GetTimeZoneInformation(&tz)) {
		case  TIME_ZONE_ID_STANDARD :
				t_local -= (tz.Bias * 60);
				break;
		case  TIME_ZONE_ID_DAYLIGHT :
				t_local -= ((tz.Bias + tz.DaylightBias) * 60);
				break;
		case  TIME_ZONE_ID_UNKNOWN : break;
	}
	return(gmtime(&t_local));
}

/*
** For a given date in *ptm, return the week number (use ISO8601 rules)
** i.e. week 1 contains 4th January,or first Thursday in year
** This is not 100% complete.
*/
int GetWeekNumber(const struct tm *ptm) {
	return((((ptm->tm_yday + 1) - 4) / 7) + 1);
}

/*
** Write a formatted date / time string to the specified buffer, according to
** the specifications supplied.
** Lua only calls this once (from os_date), and thankfully calls us iteratively
** with only one format specifier at a time. Hash modifier not yet supported.
*/
RTEXP	size_t strftime( char *strDest, size_t maxsize, const char *format, const struct tm *ptm ) {

	size_t					i;
	int						count;
	BOOL					hash;
	wchar_t					wout[BUFSIZ] = L"";
	char					out[BUFSIZ] = "";
	char					*outp;
	TIME_ZONE_INFORMATION	tz;
	SYSTEMTIME				*syst = systime_from_tm_struct((const struct tm*)ptm);

	strcpy(out, "");
	wcscpy(wout, L"");
	outp = out;
	count = 0;
	i = 0;
	while(i < (size_t) strlen(format)) {

		if(format[i] == '%') {

			if(format[i+1] == '#') { hash = TRUE; i++; }

			switch(format[i+1]) {

			case 'a' :	/* abbreviated weekday name */
						GetDateFormatW(LOCALE_SYSTEM_DEFAULT, (DWORD)0, syst, L"ddd", wout, BUFSIZ);
						outp += WideCharToMultiByte(CP_ACP, (DWORD)NULL, wout, -1, out, BUFSIZ, NULL, NULL) -1;
						break;

			case 'A' :	/* full weekday name */
						GetDateFormatW(LOCALE_SYSTEM_DEFAULT, (DWORD)0, syst, L"dddd", wout, BUFSIZ);
						outp += WideCharToMultiByte(CP_ACP, (DWORD)NULL, wout, -1, out, BUFSIZ, NULL, NULL) -1;
						break;

			case 'b' :	/* abbreviated month name */
						GetDateFormatW(LOCALE_SYSTEM_DEFAULT, (DWORD)0, syst, L"MMM", wout, BUFSIZ);
						outp += WideCharToMultiByte(CP_ACP, (DWORD)NULL, wout, -1, out, BUFSIZ, NULL, NULL) -1;
						break;

			case 'B' :	/* full month name */
						GetDateFormatW(LOCALE_SYSTEM_DEFAULT, (DWORD)0, syst, L"MMMM", wout, BUFSIZ);
						outp += WideCharToMultiByte(CP_ACP, (DWORD)NULL, wout, -1, out, BUFSIZ, NULL, NULL) -1;
						break;

			case 'c' :	/* date and time representation appropriate for locale */
						GetDateFormatW(LOCALE_SYSTEM_DEFAULT, DATE_LONGDATE, syst, NULL, wout, BUFSIZ);
						outp += WideCharToMultiByte(CP_ACP, (DWORD)NULL, wout, -1, out, BUFSIZ, NULL, NULL) -1;
						break;

			case 'd' :	/* day of month as decimal number 01 - 31 */
						outp += sprintf(outp, "%02d", syst->wDay);
						break;

			case 'H' :	/* hour in 24 hour format 00-23 */
						outp += sprintf(outp, "%02d", syst->wHour);
						break;

			case 'I' :	/* hour in 12 hour format 00-12 */
						outp += sprintf(outp, "%02d", (syst->wHour > 12 ? syst->wHour - 12 : syst->wHour));
						break;

			case 'j' :	/* day of year as decimal number 001 - 366 */
						outp += sprintf(outp, "%03d", ptm->tm_yday +1);
						break;

			case 'm' :	/* month as decimal number 01 - 12 */
						outp += sprintf(outp, "%02d", syst->wMonth);
						break;

			case 'M' :	/* minute as decimal number 00 - 59 */
						outp += sprintf(outp, "%02d", syst->wMinute);
						break;

			case 'p' :	/* current locale's AM/PM indicator for 12-hour clock */
						GetTimeFormatW(LOCALE_SYSTEM_DEFAULT, (DWORD)0, syst, L"tt", wout, BUFSIZ);
						outp += WideCharToMultiByte(CP_ACP, (DWORD)NULL, wout, -1, out, BUFSIZ, NULL, NULL) -1;
						break;

			case 'S' :	/* second as decimal number 01 - 59 */
						outp += sprintf(outp, "%02d", syst->wSecond);
						break;

			case 'U' :	/* week of year as decimal number (Sunday is first day of week) 00 - 53 */
						outp += sprintf(outp, "%02d", GetWeekNumber(ptm));
						break;

			case 'w' :	/* weekday as decimal number (0 = Sunday) 0 - 6 */
						outp += sprintf(outp, "%01d", ptm->tm_wday);
						break;

			case 'W' :	/* week of year as decimal number (Monday is first day of week) 00 - 53 */
						outp += sprintf(outp, "%02d", GetWeekNumber(ptm));
						break;

			case 'x' :	/* date representation for current locale */
						GetDateFormatW(LOCALE_SYSTEM_DEFAULT, DATE_SHORTDATE, syst, NULL, wout, BUFSIZ);
						outp += WideCharToMultiByte(CP_ACP, (DWORD)NULL, wout, -1, out, BUFSIZ, NULL, NULL) -1;
						break;

			case 'X' :	/* time representation for current locale */
						GetTimeFormatW(LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE, syst, NULL, wout, BUFSIZ);
						outp += WideCharToMultiByte(CP_ACP, (DWORD)NULL, wout, -1, out, BUFSIZ, NULL, NULL) -1;
						break;

			case 'y' :	/* year without century as decimal number 00 - 99 */
						GetDateFormatW(LOCALE_SYSTEM_DEFAULT, (DWORD)0, syst, L"yy", wout, BUFSIZ);
						outp += WideCharToMultiByte(CP_ACP, (DWORD)NULL, wout, -1, out, BUFSIZ, NULL, NULL) -1;
						break;

			case 'Y' :	/* year with century as decimal number */
						outp += sprintf(outp, "%02d", syst->wYear);
						break;

			case 'z' :	/* timezone name or abbreviation, no charachers if unknown */
			case 'Z' :
						GetTimeZoneInformation(&tz);
						outp += WideCharToMultiByte(CP_ACP, (DWORD)NULL, tz.StandardName, -1, out, BUFSIZ, NULL, NULL) -1;
						break;

			case '%' :	/* percent symbol (escaped) */
						outp += sprintf(outp, "%s", "%");
						break;

			default :	/* copy character unmodifed to output buffer */
						*outp = format[i+1]; outp++;
						break;

			}
		}

		i++;
	}

	if((size_t)strlen(out) <= maxsize) {
		strcpy(strDest, out);
		return(strlen(strDest));
	}
	else return(0);
}

/*
** Implementation of freopen() - requires wide char conversion
*/
RTEXP	FILE *freopen( const char *path, const char *mode, FILE *stream ) {
	wchar_t	wpath[BUFSIZ];
	wchar_t	wmode[8];

	MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, path, -1, wpath, BUFSIZ);
	MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, mode, -1, wmode, 8);
	return(_wfreopen(wpath, wmode, stream));
}


/*
** Implementation of system() - requires wide char conversion
*/
RTEXP	int system( const char *command ) {
	wchar_t					wcmdline[BUFSIZ];
	BOOL					ret;
	PROCESS_INFORMATION		pi;
	MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, command, -1, wcmdline, BUFSIZ);
	ret = CreateProcessW(wcmdline, NULL, NULL, NULL, (BOOL)NULL, CREATE_NEW_CONSOLE, NULL, NULL, NULL, &pi);
	return(0);
}


/*
** Implementation of LoadLibraryA - requires wide char conversion
** as WinCE uses LoadLibraryW
*/
RTEXP	HINSTANCE LoadLibraryA(const char *path) {
	wchar_t	wpath[BUFSIZ];
	MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, path, -1, wpath, BUFSIZ);
	return(LoadLibraryW(wpath));
}


/*
** Implementation of FormatMessageA - requires wide char conversion
** as WinCE uses FormatMessage (W)
*/
RTEXP	DWORD FormatMessageA(DWORD dwFlags, LPCVOID lpSource, DWORD dwMessageId, DWORD dwLanguageId,
  char *lpBuffer, DWORD nSize, va_list *Arguments) {
	DWORD ret= FormatMessageW(dwFlags, lpSource, dwMessageId, dwLanguageId, wbuffer, BUFSIZ, Arguments);
	return(WideCharToMultiByte(CP_ACP, (DWORD)NULL, wbuffer, -1, lpBuffer, nSize, NULL, NULL));
}


/*
** Implementation of GetModuleFileNameA - requires wide to sbcs char conversion
*/
RTEXP	DWORD GetModuleFileNameA( HMODULE hModule, char* lpFilename, DWORD nSize ) {
	GetModuleFileNameW(hModule, wbuffer, BUFSIZ);
	return(WideCharToMultiByte(CP_ACP, (DWORD)NULL, wbuffer, -1, lpFilename, nSize, NULL, NULL));
}


/*
** Implementation of GetSystemTimeAsFileTime - required by luasocket, not core Lua
*/
RTEXP	void GetSystemTimeAsFileTime(FILETIME *pft) {
	SYSTEMTIME	syst;
	GetSystemTime(&syst);
	SystemTimeToFileTime(&syst, pft);
	return;
}

/*
** Implementation of GetFileAttributesA - requires wide char conversion
** as WinCE uses GetFileAttributesW
*/
RTEXP	DWORD GetFileAttributesA(const char *pathName) {
	wchar_t	wpath[BUFSIZ];
	MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, pathName, -1, wpath, BUFSIZ);
	return(GetFileAttributesW(wpath));
}

/*
** Implementation of CreateDirectoryA - requires wide char conversion
** as WinCE uses CreateDirectoryW
*/
RTEXP	BOOL WINAPI  CreateDirectoryA(const char *pathName,  LPSECURITY_ATTRIBUTES lpSecurityAttributes) {
	wchar_t	wpath[BUFSIZ];
	MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, pathName, -1, wpath, BUFSIZ);
	return(CreateDirectoryW(wpath, lpSecurityAttributes));
}

/*
** Implementation of RemoveDirectoryA - requires wide char conversion
** as WinCE uses RemoveDirectoryW
*/
RTEXP	BOOL WINAPI  RemoveDirectoryA(const char *pathName) {
	wchar_t	wpath[BUFSIZ];
	MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, pathName, -1, wpath, BUFSIZ);
	return(RemoveDirectoryW(wpath));
}

/*
** ===================================================================
** Code below is incomplete - temporary hacks that must be refined.
*/

RTEXP	int setvbuf( FILE *stream, char *buffer, int mode, size_t size ) {
	return -1;
}


/*
** Implementation of clock() function
** GetTickCount is millisecs since WinCE was started, so not strictly correct.
** We should ideally return (seconds since process started x CLOCKS_PER_SEC).
** i.e. clock ticks since program began execution.
*/
RTEXP	clock_t clock( void ) {
	return GetTickCount();
}


RTEXP	void ( *signal( int sig, void (__cdecl *func) ( int sig)) ) ( int sig ) {

	return(NULL);
}

RTEXP	char *getenv( const char *varname ) {
	return NULL;
}

/* alloc's and returns a converted wide-char string from a normal one */
/* used in rename / remove functions */
wchar_t *atowcs( const char *str ) {
	int len;
	wchar_t *wstr;
	if( str == NULL )
		return NULL;
	len = strlen( str );
	wstr = (wchar_t *)malloc( sizeof( wchar_t )*2*(len + 1) );
	wsprintfW( wstr, L"%hs", str );
	return wstr;
}

RTEXP	int rename( const char *src, const char *dst ) {
	wchar_t *wsrc = atowcs( src ),
		    *wdst = atowcs( dst );
	int res = 0;

	if( !MoveFileW( wsrc, wdst ))
		res = GetLastError();

	free( wsrc );
	free( wdst );

	return res;
}

RTEXP	int remove( const char *fname ) {
	wchar_t *wfname = atowcs( fname );
	int res = 0;

	if( !DeleteFileW( wfname ))
		res = GetLastError();

	free( wfname );

	return 0;
}

RTEXP	char *setlocale( int category, const char *locale ) {
	return NULL;
}

RTEXP	struct lconv *localeconv( void ){
	return NULL;
}

