/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/form_history.c
 *
 * PURPOSE:
 *   Record form changes and apply deterministic undo or redo operations with
 *   stale-revision protection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/form_history.h"

#include <string.h>

/*
 * Initialise application experience form history from caller-provided values so later
 * operations receive a known state.
 */
void umi_application_experience_form_history_init(UmiApplicationExperienceFormHistory *history) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (history != NULL)
    (void)memset(history, 0, sizeof(*history));
}

/*
 * Provide the application experience form history can undo operation used by this module
 * and its client applications.
 */
int umi_application_experience_form_history_can_undo(
    const UmiApplicationExperienceFormHistory *history) {
  return history != NULL && history->count <= UMI_APPLICATION_EXPERIENCE_FORM_HISTORY_CAPACITY &&
         history->cursor > 0U && history->cursor <= history->count;
}

/*
 * Provide the application experience form history can redo operation used by this module
 * and its client applications.
 */
int umi_application_experience_form_history_can_redo(
    const UmiApplicationExperienceFormHistory *history) {
  return history != NULL && history->count <= UMI_APPLICATION_EXPERIENCE_FORM_HISTORY_CAPACITY &&
         history->cursor < history->count;
}

/*
 * Provide the application experience form history record operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_experience_form_history_record(
    UmiApplicationExperienceFormHistory *history,
    const UmiApplicationExperienceFormChangeset *changeset) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (history == NULL || changeset == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (history->count > UMI_APPLICATION_EXPERIENCE_FORM_HISTORY_CAPACITY ||
      changeset->count > UMI_APPLICATION_EXPERIENCE_CHANGE_CAPACITY)
    return UMI_STATUS_INVALID_STATE;
  history->count = history->cursor;
  /* Keep the operation inside its valid bounds before reading, writing or adding data. */
  if (history->count >= UMI_APPLICATION_EXPERIENCE_FORM_HISTORY_CAPACITY) {
    (void)memmove(&history->entries[0], &history->entries[1],
                  (UMI_APPLICATION_EXPERIENCE_FORM_HISTORY_CAPACITY - 1U) *
                      sizeof(history->entries[0]));
    history->count -= 1U;
    history->cursor -= 1U;
  }
  history->entries[history->count++] = *changeset;
  history->cursor = history->count;
  return UMI_STATUS_OK;
}

/*
 * Provide the application experience form history undo operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_experience_form_history_undo(UmiApplicationExperienceFormHistory *history,
                                                       UmiApplicationExperienceFormModel *model) {
  UmiStatus status;
  /* Apply this branch only when its contract condition is satisfied. */
  if (!umi_application_experience_form_history_can_undo(history))
    return UMI_STATUS_NOT_FOUND;
  status = umi_application_experience_form_changeset_apply(
      model, &history->entries[history->cursor - 1U], 1);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    history->cursor -= 1U;
  return status;
}

/*
 * Provide the application experience form history redo operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_experience_form_history_redo(UmiApplicationExperienceFormHistory *history,
                                                       UmiApplicationExperienceFormModel *model) {
  UmiStatus status;
  /* Apply this branch only when its contract condition is satisfied. */
  if (!umi_application_experience_form_history_can_redo(history))
    return UMI_STATUS_NOT_FOUND;
  status =
      umi_application_experience_form_changeset_apply(model, &history->entries[history->cursor], 0);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    history->cursor += 1U;
  return status;
}
