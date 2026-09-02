/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/form_validation.h
 *
 * PURPOSE:
 *   Return structured field-level validation issues that every frontend can
 *   present inline, in a summary or through accessibility announcements.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_FORM_VALIDATION_H
#define UMICOM_APPLICATION_EXPERIENCE_FORM_VALIDATION_H

#include "umicom/application/experience/form_model.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application experience validation issue data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationExperienceValidationIssue {
  UmiApplicationExperienceIssueSeverity severity;
  char field_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  char message[UMI_APPLICATION_EXPERIENCE_TEXT_CAPACITY];
} UmiApplicationExperienceValidationIssue;

/**
 * Represent the application experience validation report data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationExperienceValidationReport {
  UmiApplicationExperienceValidationIssue issues[UMI_APPLICATION_EXPERIENCE_ISSUE_CAPACITY];
  size_t count;
  size_t error_count;
  size_t warning_count;
} UmiApplicationExperienceValidationReport;

/**
 * Initialise application experience validation report from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_experience_validation_report_init(
    UmiApplicationExperienceValidationReport *report);
/**
 * Check that application experience form satisfies its contract before another service
 * relies on it.
 */
UmiStatus
umi_application_experience_form_validate(const UmiApplicationExperienceFormModel *model,
                                         UmiApplicationExperienceValidationReport *report);
/**
 * Find application experience validation report while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationExperienceValidationIssue *umi_application_experience_validation_report_at(
    const UmiApplicationExperienceValidationReport *report, size_t index);
/**
 * Provide the application experience validation report passed operation used by this
 * module and its client applications.
 */
int umi_application_experience_validation_report_passed(
    const UmiApplicationExperienceValidationReport *report);

#ifdef __cplusplus
}
#endif

#endif
