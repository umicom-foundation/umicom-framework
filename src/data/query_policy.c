/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/query_policy.c
 *
 * PURPOSE:
 *   Implement the query policy behavior for
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
#include "umicom/data/query_policy.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
/* Provide the skip space operation used by this module and its client applications. */
static const char *skip_space(const char *s){/* Continue only while work remains available; the loop body advances the state on each pass. */ while(s!=NULL&&*s!='\0'&&isspace((unsigned char)*s))++s;return s;}
/* Provide the prefix operation used by this module and its client applications. */
static int prefix(const char *s,const char *word){size_t i;s=skip_space(s);/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;word[i]!='\0';++i){/* Apply this branch only when its contract condition is satisfied. */ if(s[i]=='\0'||toupper((unsigned char)s[i])!=(unsigned char)word[i])return 0;}return s[i]=='\0'||isspace((unsigned char)s[i])||s[i]==';';}
/*
 * Provide the database query policy default operation used by this module and its client
 * applications.
 */
UmiDatabaseQueryPolicy umi_database_query_policy_default(void){UmiDatabaseQueryPolicy p={0};p.allow_reads=1;p.require_transaction_for_writes=1;p.maximum_result_rows=500U;p.maximum_statement_bytes=UMI_DATABASE_SQL_CAPACITY-1U;return p;}
/*
 * Provide the database query classify operation used by this module and its client
 * applications.
 */
UmiDatabaseQueryKind umi_database_query_classify(const char *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_DATABASE_QUERY_UNKNOWN;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(prefix(s,"SELECT")||prefix(s,"PRAGMA")||prefix(s,"EXPLAIN")||prefix(s,"GET")||prefix(s,"LIST"))return UMI_DATABASE_QUERY_READ;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(prefix(s,"INSERT")||prefix(s,"UPDATE")||prefix(s,"DELETE")||prefix(s,"SET"))return UMI_DATABASE_QUERY_WRITE;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(prefix(s,"CREATE")||prefix(s,"ALTER")||prefix(s,"DROP")||prefix(s,"TRUNCATE"))return UMI_DATABASE_QUERY_SCHEMA;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(prefix(s,"BEGIN")||prefix(s,"COMMIT")||prefix(s,"ROLLBACK"))return UMI_DATABASE_QUERY_TRANSACTION;return UMI_DATABASE_QUERY_UNKNOWN;}
/*
 * Provide the database query policy evaluate operation used by this module and its client
 * applications.
 */
UmiStatus umi_database_query_policy_evaluate(const UmiDatabaseQueryPolicy *p,const char *s,int tx,UmiDatabaseQueryDecision *out){size_t n;UmiDatabaseQueryKind k;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||s==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out,0,sizeof(*out));n=strlen(s);k=umi_database_query_classify(s);out->kind=k;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n==0U||n>p->maximum_statement_bytes){(void)snprintf(out->reason,sizeof(out->reason),"Statement is empty or exceeds the configured capacity");return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(k==UMI_DATABASE_QUERY_READ)out->allowed=p->allow_reads;else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(k==UMI_DATABASE_QUERY_WRITE){out->allowed=p->allow_writes&&(!p->require_transaction_for_writes||tx);out->approval_required=1;}else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(k==UMI_DATABASE_QUERY_SCHEMA){out->allowed=p->allow_schema_changes&&tx;out->approval_required=1;}else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(k==UMI_DATABASE_QUERY_TRANSACTION)out->allowed=1;/* Use this fallback path when the earlier condition does not apply. */ else out->allowed=0;(void)snprintf(out->reason,sizeof(out->reason),"%s query is %s by policy",umi_database_query_kind_text(k),out->allowed?"allowed":"blocked");return UMI_STATUS_OK;}
