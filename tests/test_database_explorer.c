/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_database_explorer.c
 *
 * PURPOSE:
 *   Implement the test database explorer behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "value_archive/construction_cases.h"
#include <stdlib.h>
#include <string.h>
#include "umicom/data/database_explorer.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted transfer plan value. */
static int UmiDatabaseTransferPlanConstructionCases(void)
{
    UmiDatabaseTransferPlan *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_database_transfer_plan_init(value, "export-kv", UMI_DATABASE_TRANSFER_EXPORT, UMI_DATABASE_FORMAT_JSON, "umicom_kv", "records.json") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_database_transfer_plan_init(value, oversized, UMI_DATABASE_TRANSFER_EXPORT, UMI_DATABASE_FORMAT_JSON, "umicom_kv", "records.json") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_database_transfer_plan_init(value, value->id, UMI_DATABASE_TRANSFER_EXPORT, UMI_DATABASE_FORMAT_JSON, "umicom_kv", "records.json") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->id, "export-kv") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->table_name) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_database_transfer_plan_init(value, "export-kv", UMI_DATABASE_TRANSFER_EXPORT, UMI_DATABASE_FORMAT_JSON, oversized, "records.json") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_database_transfer_plan_init(value, "export-kv", UMI_DATABASE_TRANSFER_EXPORT, UMI_DATABASE_FORMAT_JSON, value->table_name, "records.json") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->table_name, "umicom_kv") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->path) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_database_transfer_plan_init(value, "export-kv", UMI_DATABASE_TRANSFER_EXPORT, UMI_DATABASE_FORMAT_JSON, "umicom_kv", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_database_transfer_plan_init(value, "export-kv", UMI_DATABASE_TRANSFER_EXPORT, UMI_DATABASE_FORMAT_JSON, "umicom_kv", value->path) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->path, "records.json") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void){
    if (UmiDatabaseTransferPlanConstructionCases() != 0) return 1;
UmiDataServer *s=NULL;UmiDatabaseExplorer *e;UmiDatabaseExplorerSnapshot snap;UmiDatabaseTransferPlan p;assert(umi_data_server_create_memory(&s)==UMI_STATUS_OK);assert(umi_data_server_set(s,"project","umicom")==UMI_STATUS_OK);e=calloc(1U,sizeof(*e));assert(e!=NULL);assert(umi_database_explorer_init(e,s)==UMI_STATUS_OK);assert(umi_database_transfer_plan_init(&p,"export-kv",UMI_DATABASE_TRANSFER_EXPORT,UMI_DATABASE_FORMAT_JSON,"umicom_kv","records.json")==UMI_STATUS_OK);assert(umi_database_explorer_add_transfer(e,&p)==UMI_STATUS_OK);assert(umi_database_explorer_snapshot(e,&snap)==UMI_STATUS_OK);assert(strcmp(snap.backend,"memory")==0&&snap.records==1U&&snap.tables==1U&&snap.transfers==1U);free(e);umi_data_server_destroy(s);return 0;}
