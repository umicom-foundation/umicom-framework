/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/portability_audit.c
 *
 * PURPOSE:
 *   Implement the portability audit behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | CodeGuard portability audit | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/codeguard/portability_audit.h"
#include <stdio.h>
#include <string.h>
/*
 * Provide the codeguard portability is adapter path operation used by this module and its
 * client applications.
 */
bool umi_codeguard_portability_is_adapter_path(const char *path)
{
    return path != NULL && (strstr(path,"/platform/") != NULL || strstr(path,"\\platform\\") != NULL || strstr(path,"_win32.c") != NULL || strstr(path,"_posix.c") != NULL);
}
/* Provide the emit operation used by this module and its client applications. */
static UmiStatus emit(UmiCodeGuardEvidenceStore *store,const char *rule,const char *path,size_t line,const char *summary,const char *remediation)
{
    UmiCodeGuardEvidence evidence = {0};
    int length = snprintf(evidence.id,sizeof(evidence.id),"%s-%zu",rule,line);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length < 0 || (size_t)length >= sizeof(evidence.id)) return UMI_STATUS_CAPACITY_EXCEEDED;
    evidence.kind = UMI_CODEGUARD_EVIDENCE_RULE; evidence.state = UMI_CODEGUARD_EVIDENCE_WARNING; evidence.observed = 1U; evidence.line = line;
    (void)umi_codeguard_quality_copy(evidence.path,sizeof(evidence.path),path);
    (void)umi_codeguard_quality_copy(evidence.summary,sizeof(evidence.summary),summary);
    (void)umi_codeguard_quality_copy(evidence.remediation,sizeof(evidence.remediation),remediation);
    return umi_codeguard_evidence_add(store,&evidence);
}
/*
 * Provide the codeguard portability audit line operation used by this module and its
 * client applications.
 */
UmiStatus umi_codeguard_portability_audit_line(const char *path,size_t line_number,const char *line,UmiCodeGuardEvidenceStore *evidence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || line == NULL || evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_codeguard_portability_is_adapter_path(path)) return UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(line,"#include <windows.h>") != NULL || strstr(line,"DWORD ") != NULL || strstr(line,"Sleep(") != NULL) return emit(evidence,"PORT-WIN32",path,line_number,"Windows-specific API leaked outside a platform adapter","Move operating-system calls behind the Umicom platform service");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(line,"#include <unistd.h>") != NULL || strstr(line,"usleep(") != NULL) return emit(evidence,"PORT-POSIX",path,line_number,"POSIX-specific API leaked outside a platform adapter","Move operating-system calls behind the Umicom platform service");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(line,"C:\\") != NULL) return emit(evidence,"PORT-PATH",path,line_number,"Hard-coded Windows path detected","Use the platform path and workspace URI services");
    return UMI_STATUS_OK;
}
