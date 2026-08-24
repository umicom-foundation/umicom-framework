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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

typedef struct UmiRepositoryDoctorIssue {
    UmiRepositoryDoctorSeverity severity;
    char code[UMI_REPOSITORY_DOCTOR_CODE_CAPACITY];
    char message[UMI_REPOSITORY_DOCTOR_MESSAGE_CAPACITY];
} UmiRepositoryDoctorIssue;

typedef struct UmiRepositoryDoctorIssueList {
    UmiRepositoryDoctorIssue items[UMI_REPOSITORY_DOCTOR_ISSUE_CAPACITY];
    size_t count;
} UmiRepositoryDoctorIssueList;

void umi_repository_doctor_issue_list_init(UmiRepositoryDoctorIssueList *issues);
UmiStatus umi_repository_doctor_issue_add(UmiRepositoryDoctorIssueList *issues,
                                          UmiRepositoryDoctorSeverity severity,
                                          const char *code,
                                          const char *message);

#ifdef __cplusplus
}
#endif
#endif
