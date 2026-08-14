/* Umicom Framework Batch 58 test | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/compiler/compiler.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
int main(void) { UmiCompilerPlan plan; UmiPolyglotCompilationDatabase *database = calloc(1U,sizeof(*database)); UmiPolyglotCompilationEntry entry = {0}; char response[2048],json[8192]; assert(database != NULL); umi_compiler_plan_init(&plan); assert(umi_compiler_plan_add_argument(&plan,"-DNAME=Umicom Foundation") == UMI_STATUS_OK); assert(umi_compiler_plan_add_argument(&plan,"main.c") == UMI_STATUS_OK); assert(umi_compiler_response_file_encode(&plan,response,sizeof(response)) == UMI_STATUS_OK); assert(strstr(response,"\"-DNAME=Umicom Foundation\"") != NULL); (void)strcpy(entry.unit_id,"main"); (void)strcpy(entry.directory,"."); (void)strcpy(entry.source,"main.c"); (void)strcpy(entry.output,"main.o"); entry.language = UMI_COMPILER_LANGUAGE_C; entry.plan = plan; assert(umi_polyglot_compilation_database_add(database,&entry) == UMI_STATUS_OK); assert(umi_polyglot_compilation_database_export_json(database,json,sizeof(json)) == UMI_STATUS_OK); assert(strstr(json,"main.c") != NULL); free(database); return 0; }
