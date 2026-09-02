/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/ui_state.c
 *
 * PURPOSE:
 *   Initialise and validate standard UI states so every component communicates
 *   waiting, absence, failure and recovery in the same predictable way.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/ui_state.h"

#include <string.h>

#include "umicom/base/text.h"

/*
 * Initialise application experience ui state from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_experience_ui_state_init(UmiApplicationExperienceUiState *state,
                                                   UmiApplicationExperienceUiStateKind kind,
                                                   const char *title, const char *message,
                                                   const char *action_command_id) {
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (state == NULL || title == NULL || message == NULL ||
      kind < UMI_APPLICATION_EXPERIENCE_UI_READY ||
      kind > UMI_APPLICATION_EXPERIENCE_UI_PERMISSION_REQUIRED)
    return UMI_STATUS_INVALID_ARGUMENT;
  (void)memset(state, 0, sizeof(*state));
  state->kind = kind;
  state->revision = 1U;
  state->blocking =
      kind == UMI_APPLICATION_EXPERIENCE_UI_LOADING || kind == UMI_APPLICATION_EXPERIENCE_UI_BUSY;
  state->announce = kind != UMI_APPLICATION_EXPERIENCE_UI_READY;
  status = umi_text_copy(state->title, sizeof(state->title), title);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = umi_text_copy(state->message, sizeof(state->message), message);
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (status == UMI_STATUS_OK && action_command_id != NULL)
    status = umi_text_copy(state->action_command_id, sizeof(state->action_command_id),
                           action_command_id);
  return status == UMI_STATUS_OK ? umi_application_experience_ui_state_validate(state) : status;
}

/*
 * Provide the application experience ui state progress operation used by this module and
 * its client applications.
 */
UmiStatus umi_application_experience_ui_state_progress(UmiApplicationExperienceUiState *state,
                                                       uint32_t progress_percent) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (state == NULL || progress_percent > 100U)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Apply this branch only when its contract condition is satisfied. */
  if (state->kind != UMI_APPLICATION_EXPERIENCE_UI_LOADING &&
      state->kind != UMI_APPLICATION_EXPERIENCE_UI_BUSY)
    return UMI_STATUS_INVALID_STATE;
  state->progress_percent = progress_percent;
  state->revision += 1U;
  return UMI_STATUS_OK;
}

/*
 * Check that application experience ui state satisfies its contract before another service
 * relies on it.
 */
UmiStatus
umi_application_experience_ui_state_validate(const UmiApplicationExperienceUiState *state) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (state == NULL)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Apply this operation only while the related capability or state is available. */
  if (state->kind < UMI_APPLICATION_EXPERIENCE_UI_READY ||
      state->kind > UMI_APPLICATION_EXPERIENCE_UI_PERMISSION_REQUIRED || state->title[0] == '\0' ||
      state->message[0] == '\0' || state->progress_percent > 100U || state->revision == 0U ||
      (state->action_command_id[0] != '\0' &&
       !umi_application_experience_identifier_valid(state->action_command_id)))
    return UMI_STATUS_INVALID_STATE;
  return UMI_STATUS_OK;
}

/*
 * Provide the application experience ui state interactive operation used by this module
 * and its client applications.
 */
int umi_application_experience_ui_state_interactive(const UmiApplicationExperienceUiState *state) {
  return state != NULL && !state->blocking && state->kind != UMI_APPLICATION_EXPERIENCE_UI_ERROR &&
         state->kind != UMI_APPLICATION_EXPERIENCE_UI_OFFLINE &&
         state->kind != UMI_APPLICATION_EXPERIENCE_UI_PERMISSION_REQUIRED;
}
