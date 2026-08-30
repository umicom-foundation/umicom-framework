/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/control/issue.c
 *
 * PURPOSE:
 *   Capture bounded repository-control validation issues.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable repository-control capability. Applications
 *   remain thin consumers and must not duplicate this policy or state model.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/repository/issue.h"

#include <string.h>

static UmiStatus copy_text(char *out, size_t capacity, const char *text)
{
    size_t length;
    if (out == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(text);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

void umi_repository_issue_list_init(UmiRepositoryIssueList *issues)
{
    if (issues == NULL) return;
    (void)memset(issues, 0, sizeof(*issues));
}

UmiStatus umi_repository_issue_add(
    UmiRepositoryIssueList *issues,
    UmiRepositoryControlSeverity severity,
    const char *code,
    const char *path,
    const char *message)
{
    UmiRepositoryIssue *issue;
    UmiStatus status;
    if (issues == NULL || code == NULL || message == NULL ||
        severity < UMI_REPOSITORY_CONTROL_INFO ||
        severity > UMI_REPOSITORY_CONTROL_ERROR) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (issues->count >= UMI_REPOSITORY_CONTROL_ISSUE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    issue = &issues->items[issues->count];
    status = copy_text(issue->code, sizeof(issue->code), code);
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(issue->path, sizeof(issue->path), path != NULL ? path : "");
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(issue->message, sizeof(issue->message), message);
    if (status != UMI_STATUS_OK) return status;
    issue->severity = severity;
    issues->count += 1U;
    return UMI_STATUS_OK;
}

size_t umi_repository_issue_count_severity(
    const UmiRepositoryIssueList *issues,
    UmiRepositoryControlSeverity severity)
{
    size_t index;
    size_t count = 0U;
    if (issues == NULL) return 0U;
    for (index = 0U; index < issues->count; ++index) {
        if (issues->items[index].severity == severity) count += 1U;
    }
    return count;
}
