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

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */

#include "umicom/codeguard/pattern_scan.h"
#include <stdio.h>
#include <string.h>
static void copy_text(char *dst,size_t cap,const char *src){if(cap>0U)(void)snprintf(dst,cap,"%s",src!=NULL?src:"");}
UmiStatus umi_codeguard_pattern_scan_line(const UmiCodeGuardRuleRegistry *reg,const char *path,size_t line_no,const char *raw,const char *code,UmiCodeGuardResult *result)
{
    size_t i;
    if(reg==NULL||path==NULL||raw==NULL||code==NULL||result==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    for(i=0U;i<umi_codeguard_rule_registry_count(reg);++i){const UmiCodeGuardRule *r=umi_codeguard_rule_registry_at(reg,i);const char *hay=(r->match_mode==UMI_CODEGUARD_MATCH_RAW)?raw:code;const char *hit;if(r->match_mode==UMI_CODEGUARD_MATCH_INCLUDE&&strstr(code,"#include")==NULL)continue;hit=strstr(hay,r->needle);if(hit!=NULL){UmiCodeGuardFinding f={0};copy_text(f.rule_id,sizeof(f.rule_id),r->rule_id);copy_text(f.cwe,sizeof(f.cwe),r->cwe);f.severity=r->severity;f.category=r->category;f.confidence=r->confidence;copy_text(f.path,sizeof(f.path),path);f.line=line_no;f.column=(size_t)(hit-hay)+1U;copy_text(f.message,sizeof(f.message),r->message);copy_text(f.remediation,sizeof(f.remediation),r->remediation);if(umi_codeguard_result_add(result,&f)!=UMI_STATUS_OK)return UMI_STATUS_OUT_OF_MEMORY;}}
    return UMI_STATUS_OK;
}
