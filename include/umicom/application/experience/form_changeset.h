/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/form_changeset.h
 *
 * PURPOSE:
 *   Capture typed before-and-after form values so edits can be reviewed,
 *   applied, reversed and tested without copying frontend widgets.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_FORM_CHANGESET_H
#define UMICOM_APPLICATION_EXPERIENCE_FORM_CHANGESET_H

#include "umicom/application/experience/form_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application experience field change data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationExperienceFieldChange {
  char field_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  UmiApplicationExperienceValue before_value;
  UmiApplicationExperienceValue after_value;
  int before_present;
  int after_present;
} UmiApplicationExperienceFieldChange;

/**
 * Represent the application experience form changeset data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationExperienceFormChangeset {
  UmiApplicationExperienceFieldChange changes[UMI_APPLICATION_EXPERIENCE_CHANGE_CAPACITY];
  size_t count;
  uint64_t base_revision;
  uint64_t target_revision;
} UmiApplicationExperienceFormChangeset;

/**
 * Initialise application experience form changeset from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus
umi_application_experience_form_changeset_create(const UmiApplicationExperienceFormModel *before,
                                                 const UmiApplicationExperienceFormModel *after,
                                                 UmiApplicationExperienceFormChangeset *changeset);
/**
 * Perform application experience form changeset through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_application_experience_form_changeset_apply(
    UmiApplicationExperienceFormModel *model,
    const UmiApplicationExperienceFormChangeset *changeset, int reverse);
/**
 * Find application experience form changeset while leaving the underlying catalogue or
 * model owned by this module.
 */
const UmiApplicationExperienceFieldChange *
umi_application_experience_form_changeset_at(const UmiApplicationExperienceFormChangeset *changeset,
                                             size_t index);

#ifdef __cplusplus
}
#endif

#endif
