/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/transfer_plan.c
 *
 * PURPOSE:
 *   Implement the transfer plan behavior for
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
#include "umicom/data/transfer_plan.h"
#include <stdio.h>
#include <string.h>
/*
 * Initialise database transfer plan from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_database_transfer_plan_init(UmiDatabaseTransferPlan *p,const char *id,UmiDatabaseTransferDirection d,UmiDatabaseTransferFormat f,const char *table,const char *path){UmiStatus s;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||id==NULL||table==NULL||path==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(p,0,sizeof(*p));s=umi_database_copy_text(p->id,sizeof(p->id),id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_database_copy_text(p->table_name,sizeof(p->table_name),table);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_database_copy_text(p->path,sizeof(p->path),path);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==UMI_STATUS_OK)s=umi_database_copy_text(p->delimiter,sizeof(p->delimiter),",");p->direction=d;p->format=f;p->include_header=1;p->transactional=1;p->dry_run=1;p->status=UMI_STATUS_OK;return s;}
/*
 * Check that database transfer plan satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_database_transfer_plan_validate(const UmiDatabaseTransferPlan *p,char *m,size_t c){const char *reason="valid";UmiStatus s=UMI_STATUS_OK;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||m==NULL||c==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->id[0]=='\0'||p->table_name[0]=='\0'||p->path[0]=='\0'){reason="missing identity, table or path";s=UMI_STATUS_INVALID_ARGUMENT;}else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->format<UMI_DATABASE_FORMAT_CSV||p->format>UMI_DATABASE_FORMAT_SQL){reason="unsupported format";s=UMI_STATUS_INVALID_ARGUMENT;}(void)snprintf(m,c,"Database transfer plan is %s",reason);return s;}
