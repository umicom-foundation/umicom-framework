/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/memory_audit.c
 *
 * PURPOSE:
 *   Implement the memory audit behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | CodeGuard memory audit | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/codeguard/memory_audit.h"
#include <stdio.h>
#include <string.h>
/* Provide the emit operation used by this module and its client applications. */
static UmiStatus emit(UmiCodeGuardEvidenceStore *store,const char *id,const char *path,size_t line,const char *summary,const char *remediation)
{
    UmiCodeGuardEvidence evidence = {0};
    int length = snprintf(evidence.id,sizeof(evidence.id),"%s-%zu",id,line);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length < 0 || (size_t)length >= sizeof(evidence.id)) return UMI_STATUS_CAPACITY_EXCEEDED;
    evidence.kind = UMI_CODEGUARD_EVIDENCE_RULE; evidence.state = UMI_CODEGUARD_EVIDENCE_WARNING; evidence.observed = 1U; evidence.line = line;
    (void)umi_codeguard_quality_copy(evidence.path,sizeof(evidence.path),path);
    (void)umi_codeguard_quality_copy(evidence.summary,sizeof(evidence.summary),summary);
    (void)umi_codeguard_quality_copy(evidence.remediation,sizeof(evidence.remediation),remediation);
    return umi_codeguard_evidence_add(store,&evidence);
}
/*
 * Provide the codeguard memory audit line operation used by this module and its client
 * applications.
 */
UmiStatus umi_codeguard_memory_audit_line(const char *path,size_t line_number,const char *line,UmiCodeGuardEvidenceStore *evidence)
{
    UmiStatus status = UMI_STATUS_OK;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (path == NULL || line == NULL || evidence == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (strstr(line,"realloc(") != NULL && strchr(line,'=') != NULL) status = emit(evidence,"MEM-REALLOC",path,line_number,"Direct realloc assignment can lose the original allocation on failure","Assign realloc to a temporary pointer, validate it, then replace the owner");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && strstr(line,"malloc(") != NULL && strstr(line,"sizeof") == NULL) status = emit(evidence,"MEM-SIZE",path,line_number,"Allocation expression has no visible sizeof operand","Derive allocation size from the destination object type and validate multiplication overflow");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (status == UMI_STATUS_OK && strstr(line,"free(") != NULL && strstr(line,"= NULL") == NULL && strstr(line,"=NULL") == NULL) status = emit(evidence,"MEM-OWNER",path,line_number,"Released owner is not visibly cleared on this line","Clear the owning pointer after release when its scope continues");
    return status;
}
