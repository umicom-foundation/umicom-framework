/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/pattern_scan.c
 *
 * PURPOSE:
 *   Evaluate registered pattern rules and convert matches into structured
 *   findings.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/pattern_scan.h"
#include <stdio.h>
#include <string.h>
/* Provide the copy text operation used by this module and its client applications. */
static void copy_text(char *dst,size_t cap,const char *src){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(cap>0U)(void)snprintf(dst,cap,"%s",src!=NULL?src:"");}
/*
 * Provide the codeguard pattern scan line operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_pattern_scan_line(const UmiCodeGuardRuleRegistry *reg,const char *path,size_t line_no,const char *raw,const char *code,UmiCodeGuardResult *result)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(reg==NULL||path==NULL||raw==NULL||code==NULL||result==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0U;i<umi_codeguard_rule_registry_count(reg);++i){const UmiCodeGuardRule *r=umi_codeguard_rule_registry_at(reg,i);const char *hay=(r->match_mode==UMI_CODEGUARD_MATCH_RAW)?raw:code;const char *hit;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->match_mode==UMI_CODEGUARD_MATCH_INCLUDE&&strstr(code,"#include")==NULL)continue;hit=strstr(hay,r->needle);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(hit!=NULL){UmiCodeGuardFinding f={0};copy_text(f.rule_id,sizeof(f.rule_id),r->rule_id);copy_text(f.cwe,sizeof(f.cwe),r->cwe);f.severity=r->severity;f.category=r->category;f.confidence=r->confidence;copy_text(f.path,sizeof(f.path),path);f.line=line_no;f.column=(size_t)(hit-hay)+1U;copy_text(f.message,sizeof(f.message),r->message);copy_text(f.remediation,sizeof(f.remediation),r->remediation);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_codeguard_result_add(result,&f)!=UMI_STATUS_OK)return UMI_STATUS_OUT_OF_MEMORY;}}
    return UMI_STATUS_OK;
}
