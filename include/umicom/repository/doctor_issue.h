/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/repository/doctor_issue.h
 *
 * PURPOSE:
 *   Store bounded repository-doctor findings for CLI, Studio and future applications.
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
#ifndef INCLUDE_UMICOM_REPOSITORY_DOCTOR_ISSUE_H
#define INCLUDE_UMICOM_REPOSITORY_DOCTOR_ISSUE_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/repository/maintenance_types.h"
#ifdef __cplusplus
extern "C" {
#endif

#define UMI_REPOSITORY_DOCTOR_ISSUE_CAPACITY 32U
#define UMI_REPOSITORY_DOCTOR_CODE_CAPACITY 128U
#define UMI_REPOSITORY_DOCTOR_MESSAGE_CAPACITY 512U

/**
 * Represent the repository doctor issue data shared with callers of this public contract.
 */
typedef struct UmiRepositoryDoctorIssue {
    UmiRepositoryDoctorSeverity severity;
    char code[UMI_REPOSITORY_DOCTOR_CODE_CAPACITY];
    char message[UMI_REPOSITORY_DOCTOR_MESSAGE_CAPACITY];
} UmiRepositoryDoctorIssue;

/**
 * Represent the repository doctor issue list data shared with callers of this public
 * contract.
 */
typedef struct UmiRepositoryDoctorIssueList {
    UmiRepositoryDoctorIssue items[UMI_REPOSITORY_DOCTOR_ISSUE_CAPACITY];
    size_t count;
} UmiRepositoryDoctorIssueList;

/**
 * Initialise repository doctor issue list from caller-provided values so later operations
 * receive a known state.
 */
void umi_repository_doctor_issue_list_init(UmiRepositoryDoctorIssueList *issues);
/**
 * Add repository doctor issue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_repository_doctor_issue_add(UmiRepositoryDoctorIssueList *issues,
                                          UmiRepositoryDoctorSeverity severity,
                                          const char *code,
                                          const char *message);

#ifdef __cplusplus
}
#endif
#endif
