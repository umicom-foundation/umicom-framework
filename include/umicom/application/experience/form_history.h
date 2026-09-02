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

/**
 * Represent the application experience form history data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationExperienceFormHistory {
  UmiApplicationExperienceFormChangeset entries[UMI_APPLICATION_EXPERIENCE_FORM_HISTORY_CAPACITY];
  size_t count;
  size_t cursor;
} UmiApplicationExperienceFormHistory;

/**
 * Initialise application experience form history from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_experience_form_history_init(UmiApplicationExperienceFormHistory *history);
/**
 * Provide the application experience form history record operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_experience_form_history_record(
    UmiApplicationExperienceFormHistory *history,
    const UmiApplicationExperienceFormChangeset *changeset);
/**
 * Provide the application experience form history undo operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_experience_form_history_undo(UmiApplicationExperienceFormHistory *history,
                                                       UmiApplicationExperienceFormModel *model);
/**
 * Provide the application experience form history redo operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_experience_form_history_redo(UmiApplicationExperienceFormHistory *history,
                                                       UmiApplicationExperienceFormModel *model);
/**
 * Provide the application experience form history can undo operation used by this module
 * and its client applications.
 */
int umi_application_experience_form_history_can_undo(
    const UmiApplicationExperienceFormHistory *history);
/**
 * Provide the application experience form history can redo operation used by this module
 * and its client applications.
 */
int umi_application_experience_form_history_can_redo(
    const UmiApplicationExperienceFormHistory *history);

#ifdef __cplusplus
}
#endif

#endif
