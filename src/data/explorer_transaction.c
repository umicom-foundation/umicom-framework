/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/explorer_transaction.c
 *
 * PURPOSE:
 *   Implement the explorer transaction behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/data/explorer_transaction.h"
#include <string.h>
UmiStatus umi_database_explorer_transaction_init(UmiDatabaseExplorerTransaction *t,UmiDataServer *s){if(t==NULL||s==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(t,0,sizeof(*t));t->server=s;t->revision=1U;return UMI_STATUS_OK;}
UmiStatus umi_database_explorer_transaction_begin(UmiDatabaseExplorerTransaction *t){UmiStatus s;if(t==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_data_server_begin(t->server);if(s==UMI_STATUS_OK){t->active=1;t->revision+=1U;}return s;}
UmiStatus umi_database_explorer_transaction_commit(UmiDatabaseExplorerTransaction *t){UmiStatus s;if(t==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_data_server_commit(t->server);if(s==UMI_STATUS_OK){t->active=0;t->revision+=1U;}return s;}
UmiStatus umi_database_explorer_transaction_rollback(UmiDatabaseExplorerTransaction *t){UmiStatus s;if(t==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_data_server_rollback(t->server);if(s==UMI_STATUS_OK){t->active=0;t->revision+=1U;}return s;}
