/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/query_session.c
 *
 * PURPOSE:
 *   Implement the query session behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework - governed query editor execution
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/query_session.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
typedef struct PreviewWriter { char *text; size_t capacity; size_t length; size_t rows; size_t maximum_rows; } PreviewWriter;
/* Provide the write record operation used by this module and its client applications. */
static UmiStatus write_record(const char *key,const char *value,void *user){PreviewWriter *w=(PreviewWriter *)user;int n;/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(w->rows>=w->maximum_rows)return UMI_STATUS_CANCELLED;n=snprintf(w->text+w->length,w->capacity-w->length,"%s=%s\n",key,value);/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(n<0||(size_t)n>=w->capacity-w->length)return UMI_STATUS_CAPACITY_EXCEEDED;w->length+=(size_t)n;w->rows+=1U;return UMI_STATUS_OK;}
/* Provide the after word operation used by this module and its client applications. */
static const char *after_word(const char *s){/* Continue only while work remains available; the loop body advances the state on each pass. */ while(*s!='\0'&&!isspace((unsigned char)*s))++s;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(*s!='\0'&&isspace((unsigned char)*s))++s;return s;}
/* Provide the starts operation used by this module and its client applications. */
static int starts(const char *s,const char *word){size_t i;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;word[i]!='\0';++i){/* Apply this branch only when its contract condition is satisfied. */ if(s[i]=='\0'||toupper((unsigned char)s[i])!=(unsigned char)word[i])return 0;}return s[i]=='\0'||isspace((unsigned char)s[i]);}
/* Provide the record result operation used by this module and its client applications. */
static UmiStatus record_result(UmiDatabaseQuerySession *s,const char *statement,const UmiDatabaseQueryResult *result){UmiDatabaseQueryRecord r={0};UmiStatus st=umi_database_copy_text(r.statement,sizeof(r.statement),statement);/* Preserve the original failure result so the caller can respond to the correct cause. */ if(st!=UMI_STATUS_OK)return st;r.kind=result->kind;r.status=result->status;r.affected_rows=result->row_count;st=umi_database_copy_text(r.message,sizeof(r.message),result->message);return st==UMI_STATUS_OK?umi_database_query_history_append(&s->history,&r):st;}
/*
 * Initialise database query session from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_database_query_session_init(UmiDatabaseQuerySession *s,UmiDataServer *server,const UmiDatabaseQueryPolicy *policy){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||server==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(s,0,sizeof(*s));s->server=server;s->policy=policy!=NULL?*policy:umi_database_query_policy_default();s->revision=1U;return umi_database_query_history_init(&s->history);}
/*
 * Perform database query session through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_database_query_session_execute(UmiDatabaseQuerySession *s,const char *statement,UmiDatabaseQueryResult *out){UmiDatabaseQueryDecision d;UmiStatus st;const char *arg;const char *policy_statement;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||statement==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out,0,sizeof(*out));policy_statement=starts(statement,"SQL")?after_word(statement):statement;st=umi_database_query_policy_evaluate(&s->policy,policy_statement,umi_data_server_in_transaction(s->server),&d);out->kind=d.kind;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st!=UMI_STATUS_OK)return st;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!d.allowed){out->status=UMI_STATUS_PERMISSION_DENIED;(void)umi_database_copy_text(out->message,sizeof(out->message),d.reason);(void)record_result(s,statement,out);return out->status;}st=UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(starts(statement,"LIST")){PreviewWriter w={out->preview,sizeof(out->preview),0U,0U,s->policy.maximum_result_rows};st=umi_data_server_visit(s->server,write_record,&w);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_CANCELLED)st=UMI_STATUS_OK;out->row_count=w.rows;}else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(starts(statement,"GET")){arg=after_word(statement);st=umi_data_server_get(s->server,arg,out->preview,sizeof(out->preview));/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)out->row_count=1U;}else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(starts(statement,"SET")){const char *value;char key[192];size_t n;arg=after_word(statement);value=after_word(arg);n=(size_t)(value-arg);/* Continue only while work remains available; the loop body advances the state on each pass. */ while(n>0U&&isspace((unsigned char)arg[n-1U]))--n;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(n==0U||n>=sizeof(key)||value[0]=='\0')st=UMI_STATUS_INVALID_ARGUMENT;/* Use this fallback path when the earlier condition does not apply. */ else{(void)memcpy(key,arg,n);key[n]='\0';st=umi_data_server_set(s->server,key,value);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)out->row_count=1U;}}else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(starts(statement,"DELETE")){arg=after_word(statement);st=umi_data_server_delete(s->server,arg);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(st==UMI_STATUS_OK)out->row_count=1U;}else /* Protect caller-owned memory by checking that required state is available before it is used. */ if(starts(statement,"SQL")){arg=after_word(statement);st=umi_data_server_execute(s->server,arg);}out->status=st;(void)snprintf(out->message,sizeof(out->message),"%s query: %s",umi_database_query_kind_text(out->kind),umi_status_text(st));(void)record_result(s,statement,out);s->revision+=1U;return st;}
