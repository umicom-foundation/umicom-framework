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
#include <stdlib.h>
#include <string.h>
#include "umicom/data/database_explorer.h"
int main(void){UmiDataServer *s=NULL;UmiDatabaseExplorer *e;UmiDatabaseExplorerSnapshot snap;UmiDatabaseTransferPlan p;assert(umi_data_server_create_memory(&s)==UMI_STATUS_OK);assert(umi_data_server_set(s,"project","umicom")==UMI_STATUS_OK);e=calloc(1U,sizeof(*e));assert(e!=NULL);assert(umi_database_explorer_init(e,s)==UMI_STATUS_OK);assert(umi_database_transfer_plan_init(&p,"export-kv",UMI_DATABASE_TRANSFER_EXPORT,UMI_DATABASE_FORMAT_JSON,"umicom_kv","records.json")==UMI_STATUS_OK);assert(umi_database_explorer_add_transfer(e,&p)==UMI_STATUS_OK);assert(umi_database_explorer_snapshot(e,&snap)==UMI_STATUS_OK);assert(strcmp(snap.backend,"memory")==0&&snap.records==1U&&snap.tables==1U&&snap.transfers==1U);free(e);umi_data_server_destroy(s);return 0;}
