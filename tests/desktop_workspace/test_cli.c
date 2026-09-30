/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/desktop_workspace/test_cli.c
 * PURPOSE:
 *   Exercise the real CLI adapter in-process. All file operations are restricted to the
 *   private scratch directory provided by the test runner.
 * ORGANISATION: Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Exercise the real CLI adapter in-process. All file operations are restricted
 * to the private scratch directory provided by the test runner. */
#include "umicom/desktop_workspace/workspace.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#define CHECK(x) do { if(!(x)){fprintf(stderr,"line %d: %s\n",__LINE__,#x);return 1;} }while(0)
int main(int argc,char **argv)
{
    if(argc!=3)return 2;
    if(!strcmp(argv[1],"arguments")) {
        char *a[]={"workspace","put","--directory",argv[2],"--expected","-1"};
        CHECK(UmiDesktopWorkspaceMain(6,a)==2);
        char *b[]={"workspace","show","--directory",argv[2],"--directory",argv[2]};
        CHECK(UmiDesktopWorkspaceMain(6,b)==2);
        char *c[]={"workspace","settings","--directory",argv[2],"--expected","1","--theme","blue","--font","12"};
        CHECK(UmiDesktopWorkspaceMain(10,c)==2);
        char *d[]={"workspace","show","--directory",argv[2],"--unexpected","value"};
        CHECK(UmiDesktopWorkspaceMain(6,d)==2);
        return 0;
    }
    char unique[1024];
#ifdef _WIN32
    unsigned long pid=(unsigned long)GetCurrentProcessId();
#else
    unsigned long pid=(unsigned long)getpid();
#endif
    int n=snprintf(unique,sizeof unique,"%s-%lu-%llu",argv[2],pid,(unsigned long long)time(NULL));
    CHECK(n>0&&(size_t)n<sizeof unique);argv[2]=unique;
    UmiDesktopWorkspace *w=NULL;
    UmiStatus status=UmiDesktopWorkspaceOpenDirectory(argv[2],&w);
    if(status==UMI_STATUS_UNAVAILABLE){puts("NOT RUN: SQLite unavailable.");return 77;}
    CHECK(status==UMI_STATUS_OK);CHECK(UmiDesktopWorkspaceCloseClean(w)==UMI_STATUS_OK);UmiDesktopWorkspaceDestroy(w);
    char *a[]={"workspace","put","--directory",argv[2],"--expected","1","--id","notes","--title","Workshop","--text","Prepare Notes."};
    CHECK(UmiDesktopWorkspaceMain(12,a)==0);
    char *b[]={"workspace","settings","--directory",argv[2],"--expected","2","--theme","dark","--font","16"};
    CHECK(UmiDesktopWorkspaceMain(10,b)==0);
    char *c[]={"workspace","restore","--directory",argv[2],"--expected","3","--checkpoint","2"};
    CHECK(UmiDesktopWorkspaceMain(8,c)==0);
    CHECK(UmiDesktopWorkspaceMain(12,a)!=0); /* revision 1 is stale */
    CHECK(UmiDesktopWorkspaceOpenDirectory(argv[2],&w)==UMI_STATUS_OK);
    UmiDesktopWorkspaceSnapshot *s=malloc(sizeof *s);CHECK(s);
    CHECK(UmiDesktopWorkspaceRead(w,s)==UMI_STATUS_OK);
    CHECK(s->revision==4&&s->fontPoints==12&&s->theme==UMI_DESKTOP_WORKSPACE_SYSTEM&&!strcmp(s->notes[0].body,"Prepare Notes."));
    CHECK(UmiDesktopWorkspaceCloseClean(w)==UMI_STATUS_OK);UmiDesktopWorkspaceDestroy(w);free(s);return 0;
}
