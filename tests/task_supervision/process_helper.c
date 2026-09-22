/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/task_supervision/process_helper.c
 * PURPOSE: A real, single child process for cooperative task cancellation tests.
 * AUTHOR AND ORGANISATION: Sammy Hegab | Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif
int main(int argc,char **argv)
{
    if(argc!=2) return 2;
    puts("Umicom Notes helper ready");fflush(stdout);
    if(strcmp(argv[1],"complete")==0) return 0;
    if(strcmp(argv[1],"fail")==0) return 7;
    if(strcmp(argv[1],"wait")!=0) return 3;
#ifdef _WIN32
    Sleep(30000U);
#else
    (void)sleep(30U);
#endif
    return 0;
}
