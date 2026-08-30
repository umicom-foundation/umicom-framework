/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compilation_database.c
 *
 * PURPOSE:
 *   Implement the test compilation database behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework compilation database test. Sammy Hegab, Umicom Foundation, MIT. */
#include <assert.h>
#include <string.h>
#include "umicom/toolchain/compilation_database.h"
int main(void){const char*json="[{\"directory\":\"C:/work\",\"file\":\"main.c\",\"command\":\"cc -c main.c\",\"output\":\"main.o\"},{\"directory\":\"C:/work\",\"file\":\"util.c\",\"arguments\":[\"clang\",\"-c\",\"util.c\"]}]";UmiCompilationDatabase*d=NULL;UmiCompilationCommandSnapshot c;UmiCompilationDatabaseSnapshot s;size_t n=0U;assert(umi_compilation_database_create(&d)==UMI_STATUS_OK);assert(umi_compilation_database_import_json(d,json,"memory://test",&n)==UMI_STATUS_OK);assert(n==2U);assert(umi_compilation_database_snapshot(d,&s)==UMI_STATUS_OK&&s.command_count==2U);assert(umi_compilation_database_find_file(d,"main.c",&c)==UMI_STATUS_OK);assert(strcmp(c.output,"main.o")==0);umi_compilation_database_destroy(d);return 0;}
