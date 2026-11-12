/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_response_database.c
 *
 * PURPOSE:
 *   Implement the test compiler response database behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "support/assertions.h"
#include "umicom/compiler/compiler.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* The response-file assertions remain, while the database fixture now supplies its explicit executable and checks executable-first JSON plus receiving-owner pointers. The previous implementation is retained for review. */
#if 0
int main(void) { UmiCompilerPlan plan; UmiPolyglotCompilationDatabase *database = calloc(1U,sizeof(*database)); UmiPolyglotCompilationEntry entry = {0}; char response[2048],json[8192]; assert(database != NULL); umi_compiler_plan_init(&plan); assert(umi_compiler_plan_add_argument(&plan,"-DNAME=Umicom Foundation") == UMI_STATUS_OK); assert(umi_compiler_plan_add_argument(&plan,"main.c") == UMI_STATUS_OK); assert(umi_compiler_response_file_encode(&plan,response,sizeof(response)) == UMI_STATUS_OK); assert(strstr(response,"\"-DNAME=Umicom Foundation\"") != NULL); (void)strcpy(entry.unit_id,"main"); (void)strcpy(entry.directory,"."); (void)strcpy(entry.source,"main.c"); (void)strcpy(entry.output,"main.o"); entry.language = UMI_COMPILER_LANGUAGE_C; entry.plan = plan; assert(umi_polyglot_compilation_database_add(database,&entry) == UMI_STATUS_OK); assert(umi_polyglot_compilation_database_export_json(database,json,sizeof(json)) == UMI_STATUS_OK); assert(strstr(json,"main.c") != NULL); free(database); return 0; }
#endif
int main(void)
{
    UmiCompilerPlan plan;
    UmiPolyglotCompilationDatabase *database = calloc(1U,sizeof(*database));
    UmiPolyglotCompilationEntry entry = {0};
    char response[2048],json[8192];
    assert(database != NULL);
    umi_compiler_plan_init(&plan);
    (void)strcpy(plan.executable,"compiler");
    assert(umi_compiler_plan_add_argument(&plan,"-DNAME=Umicom Foundation") == UMI_STATUS_OK);
    assert(umi_compiler_plan_add_argument(&plan,"main.c") == UMI_STATUS_OK);
    assert(umi_compiler_response_file_encode(&plan,response,sizeof(response)) == UMI_STATUS_OK);
    assert(strstr(response,"\"-DNAME=Umicom Foundation\"") != NULL);
    (void)strcpy(entry.unit_id,"main");
    (void)strcpy(entry.directory,".");
    (void)strcpy(entry.source,"main.c");
    (void)strcpy(entry.output,"main.o");
    entry.language = UMI_COMPILER_LANGUAGE_C;
    entry.plan = plan;
    assert(umi_polyglot_compilation_database_add(database,&entry) == UMI_STATUS_OK);
    assert(database->items[0].plan.argument_view[0] == database->items[0].plan.arguments[0]);
    assert(umi_polyglot_compilation_database_export_json(database,json,sizeof(json)) == UMI_STATUS_OK);
    assert(strstr(json,"main.c") != NULL);
    assert(strstr(json,"\"arguments\":[\"compiler\",\"-DNAME=Umicom Foundation\",\"main.c\"]") != NULL);
    free(database);
    return 0;
}
