/* Umicom Framework | CodeGuard memory audit | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_CODEGUARD_MEMORY_AUDIT_H
#define UMICOM_CODEGUARD_MEMORY_AUDIT_H
#include "umicom/codeguard/evidence.h"
UmiStatus umi_codeguard_memory_audit_line(const char *path,size_t line_number,const char *line,UmiCodeGuardEvidenceStore *evidence);
#endif
