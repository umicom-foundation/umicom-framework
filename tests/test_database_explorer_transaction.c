/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_database_explorer_transaction.c
 *
 * PURPOSE:
 *   Implement the test database explorer transaction behavior for
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
#include "umicom/data/explorer_transaction.h"
int main(void){UmiDataServer *s=NULL;UmiDatabaseExplorerTransaction t;char value[32];assert(umi_data_server_create_memory(&s)==UMI_STATUS_OK);assert(umi_database_explorer_transaction_init(&t,s)==UMI_STATUS_OK);assert(umi_database_explorer_transaction_begin(&t)==UMI_STATUS_OK);assert(umi_data_server_set(s,"temporary","value")==UMI_STATUS_OK);assert(umi_database_explorer_transaction_rollback(&t)==UMI_STATUS_OK);assert(umi_data_server_get(s,"temporary",value,sizeof(value))==UMI_STATUS_NOT_FOUND);umi_data_server_destroy(s);return 0;}
