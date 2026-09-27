/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Compile/link the real SDK declarations that caused the supplied failures.
 * No network query, folder picker, file or device write is executed. */

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601
#endif
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2ipdef.h>
#include <windows.h>
#include <iphlpapi.h>
#include <netioapi.h>
#include <shlobj.h>
int main(void)
{
    PMIB_IF_TABLE2 table=NULL;
    NETIO_STATUS (WINAPI *query)(PMIB_IF_TABLE2*)=GetIfTable2;
    void (WINAPI *release)(PVOID)=FreeMibTable;
    Folder *sdkFolder=NULL;
    (void)table;
    (void)sdkFolder;
    return query&&release?0:1;
}
