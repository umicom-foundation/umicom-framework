/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/standard_states.c
 *
 * PURPOSE:
 *   Build consistent ready, loading, empty, error, offline and permission
 *   states with messages that explain what happened and what a user can do.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/standard_states.h"

#include "umicom/base/text.h"

/* Provide the subject state operation used by this module and its client applications. */
static UmiStatus subject_state(UmiApplicationExperienceUiState *state,
                               UmiApplicationExperienceUiStateKind kind, const char *title_format,
                               const char *message_format, const char *subject,
                               const char *action_command_id) {
  char title[UMI_APPLICATION_EXPERIENCE_TITLE_CAPACITY];
  char message[UMI_APPLICATION_EXPERIENCE_TEXT_CAPACITY];
  UmiStatus status;
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (subject == NULL || subject[0] == '\0')
    return UMI_STATUS_INVALID_ARGUMENT;
  status = umi_text_format(title, sizeof(title), title_format, subject);
  /* Preserve the original failure result so the caller can respond to the correct cause. */
  if (status == UMI_STATUS_OK)
    status = umi_text_format(message, sizeof(message), message_format, subject);
  return status == UMI_STATUS_OK ? umi_application_experience_ui_state_init(
                                       state, kind, title, message, action_command_id)
                                 : status;
}

/*
 * Provide the application experience state ready operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_experience_state_ready(UmiApplicationExperienceUiState *state,
                                                 const char *subject) {
  return subject_state(state, UMI_APPLICATION_EXPERIENCE_UI_READY, "%s is ready",
                       "You can now work with %s.", subject, NULL);
}

/*
 * Provide the application experience state loading operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_experience_state_loading(UmiApplicationExperienceUiState *state,
                                                   const char *subject) {
  return subject_state(state, UMI_APPLICATION_EXPERIENCE_UI_LOADING, "Loading %s",
                       "Please wait while %s is prepared.", subject, NULL);
}

/*
 * Provide the application experience state empty operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_experience_state_empty(UmiApplicationExperienceUiState *state,
                                                 const char *subject,
                                                 const char *action_command_id) {
  return subject_state(state, UMI_APPLICATION_EXPERIENCE_UI_EMPTY, "No %s yet",
                       "Create or open %s to get started.", subject, action_command_id);
}

/*
 * Provide the application experience state error operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_experience_state_error(UmiApplicationExperienceUiState *state,
                                                 const char *message,
                                                 const char *retry_command_id) {
  return umi_application_experience_ui_state_init(state, UMI_APPLICATION_EXPERIENCE_UI_ERROR,
                                                  "Something went wrong", message,
                                                  retry_command_id);
}

/*
 * Provide the application experience state offline operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_experience_state_offline(UmiApplicationExperienceUiState *state,
                                                   const char *reconnect_command_id) {
  return umi_application_experience_ui_state_init(
      state, UMI_APPLICATION_EXPERIENCE_UI_OFFLINE, "You are offline",
      "Reconnect before using features that need a service.", reconnect_command_id);
}

/*
 * Provide the application experience state permission required operation used by this
 * module and its client applications.
 */
UmiStatus
umi_application_experience_state_permission_required(UmiApplicationExperienceUiState *state,
                                                     const char *request_access_command_id) {
  return umi_application_experience_ui_state_init(
      state, UMI_APPLICATION_EXPERIENCE_UI_PERMISSION_REQUIRED, "Permission required",
      "Ask an administrator for access before continuing.", request_access_command_id);
}
