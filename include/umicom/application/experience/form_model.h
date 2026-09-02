/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/form_model.h
 *
 * PURPOSE:
 *   Hold a bounded form document, dirty state and revision independently from
 *   the widgets that display or edit it.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_FORM_MODEL_H
#define UMICOM_APPLICATION_EXPERIENCE_FORM_MODEL_H

#include "umicom/application/experience/form_schema.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application experience form model data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationExperienceFormModel {
  const UmiApplicationExperienceFormSchema *schema;
  UmiApplicationExperienceValue values[UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY];
  int present[UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY];
  uint64_t revision;
  uint64_t saved_revision;
} UmiApplicationExperienceFormModel;

/**
 * Initialise application experience form model from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus
umi_application_experience_form_model_init(UmiApplicationExperienceFormModel *model,
                                           const UmiApplicationExperienceFormSchema *schema);
/**
 * Copy application experience form model into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_application_experience_form_model_set(UmiApplicationExperienceFormModel *model,
                                                    const char *field_id,
                                                    const UmiApplicationExperienceValue *value);
/**
 * Provide the application experience form model get operation used by this module and its
 * client applications.
 */
const UmiApplicationExperienceValue *
umi_application_experience_form_model_get(const UmiApplicationExperienceFormModel *model,
                                          const char *field_id);
/**
 * Provide the application experience form model mark saved operation used by this module
 * and its client applications.
 */
void umi_application_experience_form_model_mark_saved(UmiApplicationExperienceFormModel *model);
/**
 * Provide the application experience form model dirty operation used by this module and
 * its client applications.
 */
int umi_application_experience_form_model_dirty(const UmiApplicationExperienceFormModel *model);

#ifdef __cplusplus
}
#endif

#endif
