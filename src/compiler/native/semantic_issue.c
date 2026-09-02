/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/compiler/native/semantic_issue.c
 *
 * PURPOSE:
 *   Capture compiler semantic diagnostics with stable codes, severity, spans and fingerprints.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/compiler/native/semantic_issue.h"
#include <string.h>
/*
 * Initialise nc semantic issue from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_nc_semantic_issue_init(UmiNativeSemanticIssue *i,UmiNativeSemanticSeverity sev,uint32_t code,UmiNativeSourceSpan span,const char *message){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==NULL||message==NULL||code==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(i,0,sizeof(*i));i->severity=sev;i->code=code;i->span=span;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_nc_copy_text(i->message,sizeof(i->message),message)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;i->fingerprint=umi_nc_hash_text(i->message)^((uint64_t)code<<32U)^(uint64_t)span.offset;return UMI_STATUS_OK;}
/*
 * Provide the nc semantic issue blocks codegen operation used by this module and its
 * client applications.
 */
bool umi_nc_semantic_issue_blocks_codegen(const UmiNativeSemanticIssue *i){return i!=NULL&&i->severity>=UMI_NC_SEM_ERROR;}
