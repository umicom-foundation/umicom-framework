/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/semantic_context.c
 *
 * PURPOSE:
 *   Aggregate semantic diagnostics and expose a single code-generation readiness decision.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/semantic_context.h"
#include <string.h>
void umi_nc_semantic_context_init(UmiNativeSemanticContext *c){if(c!=NULL)memset(c,0,sizeof(*c));}
UmiStatus umi_nc_semantic_context_report(UmiNativeSemanticContext *c,const UmiNativeSemanticIssue *i){if(c==NULL||i==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(c->count>=UMI_NC_MAX_SEMANTIC_ISSUES)return UMI_STATUS_CAPACITY_EXCEEDED;c->issues[c->count++]=*i;if(i->severity==UMI_NC_SEM_WARNING)c->warnings++;if(i->severity>=UMI_NC_SEM_ERROR)c->errors++;return UMI_STATUS_OK;}
bool umi_nc_semantic_context_can_codegen(const UmiNativeSemanticContext *c){return c!=NULL&&c->errors==0U;}
