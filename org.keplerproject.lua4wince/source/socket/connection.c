/*
 * connection.c
 *
 *  Created on: Dec 16, 2008
 *      Author: jasonsantos
 */

#define __USE_W32_SOCKETS

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <stdlib.h>


typedef struct addrinfo ADDRINFO;

struct in_addr BIN_IPV4_ADDR_LOOPBACK = {127, 0, 0, 1};
struct in6_addr BIN_IPV6_ADDR_LOOPBACK =   {   0x0, 0x0,
                                             0x0, 0x0,
                                             0x0, 0x0,
                                             0x0, 0x0,
                                             0x0, 0x0,
                                             0x0, 0x0,
                                             0x0, 0x0,
                                             0x0, 0x1 };
#define   MAX_LOCAL_NAME_LEN               64

//
// Determine if you have a IPv4 or IPv6 network address.
//
BOOL IsNetPresent()
{
   BOOL bFoundLocalAddr = FALSE;
   char szAddrASCII[MAX_LOCAL_NAME_LEN];
   ADDRINFO AddrHints, *pAI, *pAddrInfo;

   //
   // Get the local host's name in ASCII text.
   //
   if(gethostname(szAddrASCII, MAX_LOCAL_NAME_LEN - 1))
   {
      Print(TEXT("Error getting local host name, error = %d"), WSAGetLastError());
      return FALSE;
   }

   //
   // To obtain a list of all the local
   // addresses, resolve the local name with getaddrinfo for all
   // protocol families.
   //

   memset(&AddrHints, 0, sizeof(AddrHints));
   AddrHints.ai_family = PF_UNSPEC;
   AddrHints.ai_flags = AI_PASSIVE;

   if(getaddrinfo(szAddrASCII, "10", &AddrHints, &pAddrInfo))
   {
      Print(TEXT("getaddrinfo(%hs) error %d"), szAddrASCII, WSAGetLastError());
      return FALSE;
   }

   //
   // Search the addresses returned.
   // If any of them match the loopback address, then
   // are not connected to an outside network.
   //
   // Note: This will not tell you how many networks you
   // are connected to.  If one or more networks are present,
   // then the loopback addresses will not be included in the
   // list returned from getaddrinfo.
   //

   bFoundLocalAddr = TRUE;
   for(pAI = pAddrInfo; pAI != NULL && bFoundLocalAddr; pAI = pAI->ai_next)
   {
      if(pAI->ai_family == PF_INET)
      {
         if(memcmp(&(((SOCKADDR_IN *)(pAI->ai_addr))->sin_addr), &BIN_IPV4_ADDR_LOOPBACK, sizeof(BIN_IPV4_ADDR_LOOPBACK)) == 0)
            bFoundLocalAddr = FALSE;
      }
      else if(pAI->ai_family == PF_INET6)
      {
         if(memcmp(&(((SOCKADDR_IN6 *)(pAI->ai_addr))->sin6_addr), &BIN_IPV6_ADDR_LOOPBACK, sizeof(BIN_IPV6_ADDR_LOOPBACK)) == 0)
            bFoundLocalAddr = FALSE;
      }
   }

   freeaddrinfo(pAddrInfo);

   return bFoundLocalAddr;
}

void
Print(
   TCHAR *pFormat,
   ...)
{
   va_list ArgList;
   TCHAR   Buffer[256];

   va_start (ArgList, pFormat);

   (void)sprintf(Buffer, 256, pFormat, ArgList);

#ifndef UNDER_CE
   _putts(Buffer);
#else
   OutputDebugString(Buffer);
#endif

   va_end(ArgList);
}
