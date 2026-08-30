/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/unsafe_api.c
 *
 * PURPOSE:
 *   Implement the unsafe api behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | CodeGuard unsafe API audit | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/codeguard/unsafe_api.h"
#include <stdio.h>
#include <string.h>
static const UmiCodeGuardUnsafeApiRule RULES[] = {
    {"strcpy(","UNSAFE-STRCPY","Use a capacity-aware copy contract",UMI_CODEGUARD_EVIDENCE_FAIL},
    {"strcat(","UNSAFE-STRCAT","Use a bounded append contract",UMI_CODEGUARD_EVIDENCE_FAIL},
    {"sprintf(","UNSAFE-SPRINTF","Use snprintf and validate truncation",UMI_CODEGUARD_EVIDENCE_FAIL},
    {"gets(","UNSAFE-GETS","Use fgets with an explicit buffer capacity",UMI_CODEGUARD_EVIDENCE_FAIL},
    {"tmpnam(","UNSAFE-TMPNAM","Use a secure exclusive temporary-file provider",UMI_CODEGUARD_EVIDENCE_FAIL},
    {"system(","UNSAFE-SYSTEM","Use the governed process service with separated arguments",UMI_CODEGUARD_EVIDENCE_WARNING}
};
size_t umi_codeguard_unsafe_api_rule_count(void) { return sizeof(RULES) / sizeof(RULES[0]); }
const UmiCodeGuardUnsafeApiRule *umi_codeguard_unsafe_api_rule_at(size_t index) { return index < umi_codeguard_unsafe_api_rule_count() ? &RULES[index] : NULL; }
UmiStatus umi_codeguard_unsafe_api_scan_line(const char *path,size_t line_number,const char *line,UmiCodeGuardEvidenceStore *evidence)
{
    size_t index;
    if (path == NULL || line == NULL || evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < umi_codeguard_unsafe_api_rule_count(); ++index) {
        const UmiCodeGuardUnsafeApiRule *rule = &RULES[index];
        if (strstr(line,rule->token) != NULL) {
            UmiCodeGuardEvidence item = {0};
            int length = snprintf(item.id,sizeof(item.id),"%s-%zu",rule->rule_id,line_number);
            if (length < 0 || (size_t)length >= sizeof(item.id)) return UMI_STATUS_CAPACITY_EXCEEDED;
            item.kind = UMI_CODEGUARD_EVIDENCE_RULE; item.state = rule->state; item.observed = 1U; item.line = line_number;
            (void)umi_codeguard_quality_copy(item.path,sizeof(item.path),path);
            (void)umi_codeguard_quality_copy(item.summary,sizeof(item.summary),"Unsafe or weakly governed C API usage detected");
            (void)umi_codeguard_quality_copy(item.remediation,sizeof(item.remediation),rule->replacement);
            if (umi_codeguard_evidence_add(evidence,&item) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
        }
    }
    return UMI_STATUS_OK;
}
