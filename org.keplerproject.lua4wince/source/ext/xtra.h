/*
** WinCE port: Ian McIntosh 2008-08-15
** Implements ANSI functions missing from WinCE.
*/

#include <utime.h>
#include <sys/stat.h>

#ifndef _WCHAR_T_DEFINED
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif

#ifndef _TIME_T_DEFINED
typedef long time_t;            /* time value */
#define _TIME_T_DEFINED         /* avoid multiple def's of time_t */
#endif

#ifndef _FSIZE_T_DEFINED
typedef unsigned long _fsize_t; /* Could be 64 bits for Win32 */
#define _FSIZE_T_DEFINED
#endif


#ifndef _FINDDATA_T_DEFINED

struct _finddata_t {
    unsigned    attrib;
    time_t      time_create;    /* -1 for FAT file systems */
    time_t      time_access;    /* -1 for FAT file systems */
    time_t      time_write;
    _fsize_t    size;
    char        name[260];
};

struct _finddatai64_t {
    unsigned    attrib;
    time_t      time_create;    /* -1 for FAT file systems */
    time_t      time_access;    /* -1 for FAT file systems */
    time_t      time_write;
    __int64     size;
    char        name[260];
};


#define _FINDDATA_T_DEFINED
#endif

#ifndef _WFINDDATA_T_DEFINED

struct _wfinddata_t {
    unsigned    attrib;
    time_t      time_create;    /* -1 for FAT file systems */
    time_t      time_access;    /* -1 for FAT file systems */
    time_t      time_write;
    _fsize_t    size;
    wchar_t     name[260];
};

struct _wfinddatai64_t {
    unsigned    attrib;
    time_t      time_create;    /* -1 for FAT file systems */
    time_t      time_access;    /* -1 for FAT file systems */
    time_t      time_write;
    __int64     size;
    wchar_t     name[260];
};

#define _WFINDDATA_T_DEFINED
#endif

/* File attribute constants for _findfirst() */

#define _A_NORMAL       0x00    /* Normal file - No read/write restrictions */
#define _A_RDONLY       0x01    /* Read only file */
#define _A_HIDDEN       0x02    /* Hidden file */
#define _A_SYSTEM       0x04    /* System file */
#define _A_SUBDIR       0x10    /* Subdirectory */
#define _A_ARCH         0x20    /* Archive file */


/*
**	Function prototypes
*/

int		_locking(void *handle, int mode, long nbytes);
int		umask(int pmode);
long	_findfirst(char *filespec, struct _finddata_t *fileinfo);
int		_findnext(long handle, struct _finddata_t *fileinfo);
int		_findclose(long handle);
int		utime(const char *filename, struct utimbuf *times);
int		_wince_stati64(const char *path, struct _stati64 *buffer);
