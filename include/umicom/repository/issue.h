/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/issue.h
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
#ifndef UMICOM_REPOSITORY_ISSUE_H
#define UMICOM_REPOSITORY_ISSUE_H
#include "umicom/repository/control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the repository issue data shared with callers of this public contract.
 */
typedef struct UmiRepositoryIssue {
    char code[UMI_REPOSITORY_CONTROL_NAME_CAPACITY];
    char path[UMI_REPOSITORY_CONTROL_PATH_CAPACITY];
    char message[UMI_REPOSITORY_CONTROL_TEXT_CAPACITY];
    UmiRepositoryControlSeverity severity;
} UmiRepositoryIssue;
/**
 * Represent the repository issue list data shared with callers of this public contract.
 */
typedef struct UmiRepositoryIssueList {
    UmiRepositoryIssue items[UMI_REPOSITORY_CONTROL_ISSUE_CAPACITY];
    size_t count;
} UmiRepositoryIssueList;
/**
 * Initialise repository issue list from caller-provided values so later operations receive
 * a known state.
 */
void umi_repository_issue_list_init(UmiRepositoryIssueList *issues);
/**
 * Add repository issue only after its inputs and available capacity have been checked.
 */
UmiStatus umi_repository_issue_add(
    UmiRepositoryIssueList *issues,
    UmiRepositoryControlSeverity severity,
    const char *code,
    const char *path,
    const char *message);
/**
 * Provide the repository issue count severity operation used by this module and its client
 * applications.
 */
size_t umi_repository_issue_count_severity(
    const UmiRepositoryIssueList *issues,
    UmiRepositoryControlSeverity severity);
#ifdef __cplusplus
}
#endif
#endif
