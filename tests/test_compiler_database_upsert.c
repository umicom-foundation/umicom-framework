/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_compiler_database_upsert.c
 *
 * PURPOSE:
 *   Refresh compiler command records without stale views, lost capacity or partial database mutation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/compiler/compilation_database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define REQUIRE(condition) do { if (!(condition)) { fprintf(stderr,"database upsert line %d\n",__LINE__); return 1; } } while (0)
static UmiPolyglotCompilationEntry entry;
int main(void)
{
    UmiPolyglotCompilationDatabase *database = calloc(1U,sizeof(*database)), *before = malloc(sizeof(*before));
    REQUIRE(database != NULL && before != NULL);
    (void)strcpy(entry.unit_id,"main"); (void)strcpy(entry.source,"main.c"); entry.language = UMI_COMPILER_LANGUAGE_C;
    umi_compiler_plan_init(&entry.plan);
    REQUIRE(umi_compiler_plan_add_argument(&entry.plan,"-O0") == UMI_STATUS_OK);
    REQUIRE(umi_polyglot_compilation_database_upsert(database,&entry) == UMI_STATUS_OK);
    REQUIRE(database->count == 1U && database->revision == 1U);
    (void)strcpy(entry.plan.arguments[0],"-O2");
    REQUIRE(umi_polyglot_compilation_database_upsert(database,&entry) == UMI_STATUS_OK);
    REQUIRE(database->count == 1U && database->revision == 2U);
    REQUIRE(strcmp(database->items[0].plan.arguments[0],"-O2") == 0);
    REQUIRE(database->items[0].plan.argument_view[0] == database->items[0].plan.arguments[0]);
    (void)strcpy(entry.plan.arguments[0],"changed caller");
    REQUIRE(strcmp(database->items[0].plan.argument_view[0],"-O2") == 0);
    (void)memcpy(before,database,sizeof(*database));
    REQUIRE(umi_polyglot_compilation_database_add(database,&entry) == UMI_STATUS_ALREADY_EXISTS);
    REQUIRE(memcmp(database,before,sizeof(*database)) == 0);
    REQUIRE(umi_polyglot_compilation_database_upsert(database,&database->items[0]) == UMI_STATUS_OK);
    REQUIRE(database->count == 1U && database->revision == 3U);
    for (size_t index = 1U; index < UMI_POLYGLOT_DATABASE_MAX; ++index) {
        (void)snprintf(entry.source,sizeof(entry.source),"source-%zu.c",index);
        REQUIRE(umi_polyglot_compilation_database_upsert(database,&entry) == UMI_STATUS_OK);
    }
    (void)strcpy(entry.plan.arguments[0],"-O3");
    uint64_t revision = database->revision;
    REQUIRE(umi_polyglot_compilation_database_upsert(database,&entry) == UMI_STATUS_OK);
    REQUIRE(database->count == UMI_POLYGLOT_DATABASE_MAX && database->revision == revision+1U);
    REQUIRE(strcmp(database->items[database->count-1U].plan.arguments[0],"-O3") == 0);
    (void)strcpy(entry.source,"new.c"); (void)memcpy(before,database,sizeof(*database));
    REQUIRE(umi_polyglot_compilation_database_upsert(database,&entry) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(memcmp(database,before,sizeof(*database)) == 0);
    (void)strcpy(entry.source,"main.c"); database->revision = UINT64_MAX; (void)memcpy(before,database,sizeof(*database));
    REQUIRE(umi_polyglot_compilation_database_upsert(database,&entry) == UMI_STATUS_CAPACITY_EXCEEDED);
    REQUIRE(memcmp(database,before,sizeof(*database)) == 0);
    database->revision = 1U;
    (void)memset(database->items[database->count-1U].output,'x',sizeof(entry.output));
    (void)memcpy(before,database,sizeof(*database));
    REQUIRE(umi_polyglot_compilation_database_upsert(database,&entry) == UMI_STATUS_INVALID_STATE);
    REQUIRE(memcmp(database,before,sizeof(*database)) == 0);
    entry.source[0] = '\0';
    REQUIRE(umi_polyglot_compilation_database_upsert(database,&entry) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(umi_polyglot_compilation_database_upsert(NULL,&entry) == UMI_STATUS_INVALID_ARGUMENT);
    free(before); free(database);
    return 0;
}
