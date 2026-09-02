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

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *out, size_t capacity, const char *text)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out == NULL || text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

/*
 * Initialise repository issue list from caller-provided values so later operations receive
 * a known state.
 */
void umi_repository_issue_list_init(UmiRepositoryIssueList *issues)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (issues == NULL) return;
    (void)memset(issues, 0, sizeof(*issues));
}

/* Add repository issue only after its inputs and available capacity have been checked. */
UmiStatus umi_repository_issue_add(
    UmiRepositoryIssueList *issues,
    UmiRepositoryControlSeverity severity,
    const char *code,
    const char *path,
    const char *message)
{
    UmiRepositoryIssue *issue;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (issues == NULL || code == NULL || message == NULL ||
        severity < UMI_REPOSITORY_CONTROL_INFO ||
        severity > UMI_REPOSITORY_CONTROL_ERROR) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (issues->count >= UMI_REPOSITORY_CONTROL_ISSUE_CAPACITY) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    issue = &issues->items[issues->count];
    status = copy_text(issue->code, sizeof(issue->code), code);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(issue->path, sizeof(issue->path), path != NULL ? path : "");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(issue->message, sizeof(issue->message), message);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    issue->severity = severity;
    issues->count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the repository issue count severity operation used by this module and its client
 * applications.
 */
size_t umi_repository_issue_count_severity(
    const UmiRepositoryIssueList *issues,
    UmiRepositoryControlSeverity severity)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (issues == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < issues->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (issues->items[index].severity == severity) count += 1U;
    }
    return count;
}
