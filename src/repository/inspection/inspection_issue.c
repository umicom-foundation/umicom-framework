/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/inspection/inspection_issue.c
 *
 * PURPOSE:
 *   Implement define bounded inspection findings without replacing the existing repository doctor issue model.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/inspection_issue.h"

#include <string.h>
/* Store one finding through bounded copy helpers so UI and CLI consumers share identical evidence. */
UmiStatus umi_repository_inspection_issue_set(UmiRepositoryInspectionIssue *issue, UmiRepositoryInspectionIssueKind kind, UmiRepositoryInspectionSeverity severity, const char *summary, const char *hint)
{
    UmiStatus status; if(issue==NULL||summary==NULL||hint==NULL) return UMI_STATUS_INVALID_ARGUMENT; (void)memset(issue,0,sizeof(*issue)); issue->kind=kind; issue->severity=severity;
    status=umi_repository_inspection_copy_text(issue->summary,sizeof(issue->summary),summary); if(status!=UMI_STATUS_OK) return status; return umi_repository_inspection_copy_text(issue->remediation_hint,sizeof(issue->remediation_hint),hint);
}

