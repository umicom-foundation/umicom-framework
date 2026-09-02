/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/semantic_context.c
 *
 * PURPOSE:
 *   Aggregate semantic diagnostics and expose a single code-generation readiness decision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/semantic_context.h"
#include <string.h>
/*
 * Initialise nc semantic context from caller-provided values so later operations receive a
 * known state.
 */
void umi_nc_semantic_context_init(UmiNativeSemanticContext *c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c!=NULL)memset(c,0,sizeof(*c));}
/*
 * Provide the nc semantic context report operation used by this module and its client
 * applications.
 */
UmiStatus umi_nc_semantic_context_report(UmiNativeSemanticContext *c,const UmiNativeSemanticIssue *i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||i==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->count>=UMI_NC_MAX_SEMANTIC_ISSUES)return UMI_STATUS_CAPACITY_EXCEEDED;c->issues[c->count++]=*i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i->severity==UMI_NC_SEM_WARNING)c->warnings++;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i->severity>=UMI_NC_SEM_ERROR)c->errors++;return UMI_STATUS_OK;}
/*
 * Provide the nc semantic context can codegen operation used by this module and its client
 * applications.
 */
bool umi_nc_semantic_context_can_codegen(const UmiNativeSemanticContext *c){return c!=NULL&&c->errors==0U;}
