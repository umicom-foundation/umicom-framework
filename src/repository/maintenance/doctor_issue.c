/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/maintenance/doctor_issue.c
 *
 * PURPOSE:
 *   Implement bounded repository doctor issue retention.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/doctor_issue.h"

#include <stdio.h>
#include <string.h>

void umi_repository_doctor_issue_list_init(UmiRepositoryDoctorIssueList *issues)
{
    if (issues != NULL) (void)memset(issues, 0, sizeof(*issues));
}

UmiStatus umi_repository_doctor_issue_add(UmiRepositoryDoctorIssueList *issues,
                                          UmiRepositoryDoctorSeverity severity,
                                          const char *code,
                                          const char *message)
{
    UmiRepositoryDoctorIssue *item;
    if (issues == NULL || code == NULL || code[0] == '\0' || message == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (issues->count >= UMI_REPOSITORY_DOCTOR_ISSUE_CAPACITY) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (strlen(code) >= sizeof(issues->items[0].code) ||
        strlen(message) >= sizeof(issues->items[0].message)) return UMI_STATUS_CAPACITY_EXCEEDED;
    item = &issues->items[issues->count++];
    item->severity = severity;
    (void)snprintf(item->code, sizeof(item->code), "%s", code);
    (void)snprintf(item->message, sizeof(item->message), "%s", message);
    return UMI_STATUS_OK;
}
