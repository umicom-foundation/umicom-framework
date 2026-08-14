/* Umicom Framework | CodeGuard unsafe API audit | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_CODEGUARD_UNSAFE_API_H
#define UMICOM_CODEGUARD_UNSAFE_API_H
#include "umicom/codeguard/evidence.h"
typedef struct UmiCodeGuardUnsafeApiRule {
    const char *token;
    const char *rule_id;
    const char *replacement;
    UmiCodeGuardEvidenceState state;
} UmiCodeGuardUnsafeApiRule;
size_t umi_codeguard_unsafe_api_rule_count(void);
const UmiCodeGuardUnsafeApiRule *umi_codeguard_unsafe_api_rule_at(size_t index);
UmiStatus umi_codeguard_unsafe_api_scan_line(const char *path,size_t line_number,const char *line,UmiCodeGuardEvidenceStore *evidence);
#endif
