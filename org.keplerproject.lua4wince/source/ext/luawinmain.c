/*
** main wrapper for console-based lua.exe (not used by Seashell.exe)
** Although the Pocket PC platform does support a 'console' mode, i.e. block
** terminal stdin, stdout and stderr, this compiler / SDK does not provide
** a mainCRTStartup entry point. argc and **argv are therefore not initialised
** and will cause a memory exception.
** Instead we wrap our standard main() entry point inside WinMain.
*/
//#include <winbase.h>

//int main (int argc, char **argv);


//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd) {
//
//	int			ret = 0;
//	int			argnum = 0;
//	int			argc;
//	char		*argv[16];
//	char		pgm[] = "\\lua\\lua.exe";
//	char		buffer[255];
//	int			i, len;
//	BOOL		firstchar;
//
//	/* Command line is wide char string. lpCmdLine omits the program name	*/
//	/* so we must convert to ANSI argv format	*/
//
//	// LPTSTR	wcmdline = GetCommandLine();
//	WideCharToMultiByte(CP_ACP, (DWORD)NULL, lpCmdLine, -1, buffer, 255, NULL, NULL);
//
//	/* first arg is program name */
//	argv[argnum++] = pgm;
//	argc = 1;
//	firstchar = TRUE;
//	len = strlen(buffer);
//	for(i=0; i < len; i++) {
//		if(buffer[i] == ' ' || buffer[i] == '\t' || buffer[i] == 0x00) {
//			buffer[i] = 0x00;
//			firstchar = TRUE;
//		}
//		else {
//			if(firstchar) {
//				argv[argnum++] = &buffer[i];
//				argc++;
//				firstchar = FALSE;
//			}
//		}
//	}
//	argv[argnum] = NULL;
//
//	ret = main(argc, argv);
//}

