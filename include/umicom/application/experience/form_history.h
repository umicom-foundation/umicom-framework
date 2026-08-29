/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/form_history.h
 *
 * PURPOSE:
 *   Maintain a bounded undo and redo history of meaningful form changes rather
 *   than storing toolkit-specific input events.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_FORM_HISTORY_H
#define UMICOM_APPLICATION_EXPERIENCE_FORM_HISTORY_H

#include "umicom/application/experience/form_changeset.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_EXPERIENCE_FORM_HISTORY_CAPACITY 16U

typedef struct UmiApplicationExperienceFormHistory {
  UmiApplicationExperienceFormChangeset entries[UMI_APPLICATION_EXPERIENCE_FORM_HISTORY_CAPACITY];
  size_t count;
  size_t cursor;
} UmiApplicationExperienceFormHistory;

void umi_application_experience_form_history_init(UmiApplicationExperienceFormHistory *history);
UmiStatus umi_application_experience_form_history_record(
    UmiApplicationExperienceFormHistory *history,
    const UmiApplicationExperienceFormChangeset *changeset);
UmiStatus umi_application_experience_form_history_undo(UmiApplicationExperienceFormHistory *history,
                                                       UmiApplicationExperienceFormModel *model);
UmiStatus umi_application_experience_form_history_redo(UmiApplicationExperienceFormHistory *history,
                                                       UmiApplicationExperienceFormModel *model);
int umi_application_experience_form_history_can_undo(
    const UmiApplicationExperienceFormHistory *history);
int umi_application_experience_form_history_can_redo(
    const UmiApplicationExperienceFormHistory *history);

#ifdef __cplusplus
}
#endif

#endif
