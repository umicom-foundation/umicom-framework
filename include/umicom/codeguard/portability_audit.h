/* Umicom Framework | CodeGuard portability audit | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_CODEGUARD_PORTABILITY_AUDIT_H
#define UMICOM_CODEGUARD_PORTABILITY_AUDIT_H
#include "umicom/codeguard/evidence.h"
bool umi_codeguard_portability_is_adapter_path(const char *path);
UmiStatus umi_codeguard_portability_audit_line(const char *path,size_t line_number,const char *line,UmiCodeGuardEvidenceStore *evidence);
#endif
