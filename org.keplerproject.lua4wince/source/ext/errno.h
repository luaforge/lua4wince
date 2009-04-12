/*
** ==================================================================
** Ian McIntosh - substitute for errno / strerror()... not in WinCE.
** ===================================================================
*/

#if !defined(_ERRNO)
#define _ERRNO

static int		errno=0;

/* Following for information only - we use GetLastError() instead			*/
/* Error Codes - those with comments are Win32 supported					*/

#define EPERM           1
#define ENOENT          2		/* No such file or directory				*/
#define ESRCH           3
#define EINTR           4
#define EIO             5
#define ENXIO           6
#define E2BIG           7		/* Argument list too long					*/
#define ENOEXEC         8		/* Exec format error						*/
#define EBADF           9		/* Bad file number							*/
#define ECHILD          10		/* No spawned processes						*/
#define EAGAIN          11		/* Cannot create process - low resources	*/
#define ENOMEM          12		/* Insufficient memory						*/
#define EACCES          13		/* Access denied							*/
#define EFAULT          14
#define EBUSY           16
#define EEXIST          17		/* File already exists						*/
#define EXDEV           18		/* Cross-device link						*/
#define ENODEV          19
#define ENOTDIR         20
#define EISDIR          21
#define ENFILE          23
#define EMFILE          24		/* Too many open files						*/
#define ENOTTY          25
#define EFBIG           27
#define ENOSPC          28		/* Insufficent space on device				*/
#define ESPIPE          29
#define EROFS           30
#define EMLINK          31
#define EPIPE           32
#define EDOM            33		/* Math argument							*/
#define ERANGE          34		/* Result too large							*/
#define EDEADLK         36		/* Resource deadlock						*/
#define ENAMETOOLONG    38
#define ENOLCK          39
#define ENOSYS          40
#define ENOTEMPTY       41
#define EILSEQ          42



#endif	/* _ERRNO */
