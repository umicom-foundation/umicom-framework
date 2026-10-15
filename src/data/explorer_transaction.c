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
#include <stdint.h>
#include "umicom/data/explorer_transaction.h"
#include <string.h>
/*
 * Initialise database explorer transaction from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_database_explorer_transaction_init(UmiDatabaseExplorerTransaction *t,UmiDataServer *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||s==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(t,0,sizeof(*t));t->server=s;t->revision=1U;return UMI_STATUS_OK;}
/*
 * Provide the database explorer transaction begin operation used by this module and its
 * client applications.
 */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_database_explorer_transaction_begin(UmiDatabaseExplorerTransaction *t){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_data_server_begin(t->server);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK){t->active=1;t->revision+=1U;}return s;}
#endif

UmiStatus umi_database_explorer_transaction_begin(UmiDatabaseExplorerTransaction *t){
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (t != NULL && t->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_data_server_begin(t->server);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK){t->active=1;t->revision+=1U;}return s;}
/*
 * Provide the database explorer transaction commit operation used by this module and its
 * client applications.
 */
UmiStatus umi_database_explorer_transaction_commit(UmiDatabaseExplorerTransaction *t){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_data_server_commit(t->server);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK){t->active=0;t->revision+=1U;}return s;}
/*
 * Provide the database explorer transaction rollback operation used by this module and its
 * client applications.
 */
/* Retained for review: the former mutation could wrap its owner revision.
 * The active implementation keeps its behaviour and adds a refusal before any
 * write when no newer revision token can be represented. */
#if 0
UmiStatus umi_database_explorer_transaction_rollback(UmiDatabaseExplorerTransaction *t){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_data_server_rollback(t->server);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK){t->active=0;t->revision+=1U;}return s;}
#endif

UmiStatus umi_database_explorer_transaction_rollback(UmiDatabaseExplorerTransaction *t){
    /* A revision is an observation token, not a wrapping counter. Refuse before
     * any nested operation or field write can make the last token ambiguous. */
    if (t != NULL && t->revision == UINT64_MAX)
        return UMI_STATUS_CAPACITY_EXCEEDED;
UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_data_server_rollback(t->server);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK){t->active=0;t->revision+=1U;}return s;}
