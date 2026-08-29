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

typedef struct UmiApplicationExperienceFormModel {
  const UmiApplicationExperienceFormSchema *schema;
  UmiApplicationExperienceValue values[UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY];
  int present[UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY];
  uint64_t revision;
  uint64_t saved_revision;
} UmiApplicationExperienceFormModel;

UmiStatus
umi_application_experience_form_model_init(UmiApplicationExperienceFormModel *model,
                                           const UmiApplicationExperienceFormSchema *schema);
UmiStatus umi_application_experience_form_model_set(UmiApplicationExperienceFormModel *model,
                                                    const char *field_id,
                                                    const UmiApplicationExperienceValue *value);
const UmiApplicationExperienceValue *
umi_application_experience_form_model_get(const UmiApplicationExperienceFormModel *model,
                                          const char *field_id);
void umi_application_experience_form_model_mark_saved(UmiApplicationExperienceFormModel *model);
int umi_application_experience_form_model_dirty(const UmiApplicationExperienceFormModel *model);

#ifdef __cplusplus
}
#endif

#endif
