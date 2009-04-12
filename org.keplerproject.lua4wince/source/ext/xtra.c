/*
** WinCE port: Ian McIntosh 2008-08-15
** Implements ANSI functions missing from WinCE.
*/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include <windows.h>

#include "sys/stat.h"		/* IAN */

#include "../lua/lua.h"
#include "../lua/lauxlib.h"
#include "../lua/lualib.h"
#include "../lfs/lfs.h"

#include "xtra.h"

static int previous_pmode = 0;

/* Reusable buffers for wide / single byte string conversions */
char	cbuf[BUFSIZ];
wchar_t	wbuf[BUFSIZ];

/*
**	Functions required by LuaFileSystem but not in WinCE
*/
int	_locking(void *handle, int mode, long nbytes) { errno=EBADF; return -1; }
int	umask(int pmode) {int	retval = previous_pmode; previous_pmode = pmode; return(retval); }
long _findfirst(char *filespec, struct _finddata_t *fileinfo) { errno=ENOENT; return -1; }
int _findnext(long handle, struct _finddata_t *fileinfo)  { errno=ENOENT; return -1; }
int _findclose(long handle) { errno=ENOENT; return -1; }

// TODO: Find out if these figures are precise enough for WindowsCE
LPFILETIME UnixTimeToFileTime(time_t t, LPFILETIME pft)
{
	// Note that LONGLONG is a 64-bit value
	LONGLONG ll;

	ll = Int32x32To64(t, 10000000) + 116444736000000000ll;
	pft->dwLowDateTime = (DWORD)ll;
	pft->dwHighDateTime = ll >> 32;
	return pft;
}


int utime(const char *filename, struct utimbuf *times) {
	SetLastError(0L);
	MultiByteToWideChar(CP_ACP, (DWORD)0, filename, -1, wbuf, sizeof(wbuf));
	HANDLE h = CreateFile(wbuf,GENERIC_WRITE,FILE_SHARE_READ | FILE_SHARE_WRITE,NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);

	DWORD err = GetLastError();

	if(!times || err != ERROR_SUCCESS) {
		if(err==ERROR_ACCESS_DENIED)
			errno = EACCES;
		if(err==ERROR_FILE_NOT_FOUND)
			errno = ENOENT;
		return -1;
	}

	if(times->actime || times->modtime) {
		FILETIME ac;
		FILETIME mo;
		SetLastError(0L);
		SetFileTime(h,
				NULL,
				times->actime ? UnixTimeToFileTime(times->actime, &ac) : NULL,
				times->modtime ? UnixTimeToFileTime(times->modtime, &mo) : NULL
		);
		err = GetLastError();
	}

	CloseHandle(h);

	return  (err==ERROR_SUCCESS) ? 0 : -1;
}



int _wince_stati64(const char *path, struct _stati64 *buffer)
{
	HANDLE						h;
	BY_HANDLE_FILE_INFORMATION	fi;
	SYSTEMTIME					syst;
	DWORD						fa;

	MultiByteToWideChar(CP_ACP, (DWORD)0, path, -1, wbuf, sizeof(wbuf));

	h = CreateFile(wbuf, (DWORD)0, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
							FILE_ATTRIBUTE_NORMAL, NULL);

	if(h != INVALID_HANDLE_VALUE ) {
		GetFileInformationByHandle(h, &fi);
		buffer->st_gid		= 0;	/* UNIX only					*/
		buffer->st_ino		= 0;	/* UNIX only					*/
		buffer->st_uid		= 0;	/* UNIX only					*/
		buffer->st_dev		= 0;	/* drive number					*/
		buffer->st_rdev		= 0;	/* drive number					*/
		buffer->st_nlink	= 1;	/* always 1 on non-NTFS			*/
		/* last access time	(time_t)	*/
		FileTimeToSystemTime(&fi.ftLastAccessTime, &syst);
		buffer->st_atime	= mktime(tm_struct_from_systime(&syst));
		/* creation time				*/
		FileTimeToSystemTime(&fi.ftCreationTime, &syst);
		buffer->st_ctime	= mktime(tm_struct_from_systime(&syst));
		/* modified time				*/
		FileTimeToSystemTime(&fi.ftLastWriteTime, &syst);
		buffer->st_mtime	= mktime(tm_struct_from_systime(&syst));
		/* file mode bitmask			*/
		if(fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			buffer->st_mode		= _S_IFDIR;
		else
			buffer->st_mode		= _S_IFREG;

		/* file size in bytes			*/
		buffer->st_size		= fi.nFileSizeLow;

	}
	else {
		fa = GetFileAttributes(wbuf);
		if(fa == 0xFFFFFFFF) {
			errno = ENOENT;
			return -1;
		}
		if(fa & FILE_ATTRIBUTE_DIRECTORY)
			buffer->st_mode		= _S_IFDIR;
	}

	return 0;
}






