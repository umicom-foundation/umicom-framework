/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/ui_state.h
 *
 * PURPOSE:
 *   Represent ready, empty, loading, busy, success, warning, error, offline and
 *   permission states with portable text and progress information.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_UI_STATE_H
#define UMICOM_APPLICATION_EXPERIENCE_UI_STATE_H

#include "umicom/application/experience/form_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationExperienceUiState {
  UmiApplicationExperienceUiStateKind kind;
  char title[UMI_APPLICATION_EXPERIENCE_TITLE_CAPACITY];
  char message[UMI_APPLICATION_EXPERIENCE_TEXT_CAPACITY];
  char action_command_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  uint32_t progress_percent;
  uint64_t revision;
  int blocking;
  int announce;
} UmiApplicationExperienceUiState;

UmiStatus umi_application_experience_ui_state_init(UmiApplicationExperienceUiState *state,
                                                   UmiApplicationExperienceUiStateKind kind,
                                                   const char *title, const char *message,
                                                   const char *action_command_id);
UmiStatus umi_application_experience_ui_state_progress(UmiApplicationExperienceUiState *state,
                                                       uint32_t progress_percent);
UmiStatus
umi_application_experience_ui_state_validate(const UmiApplicationExperienceUiState *state);
int umi_application_experience_ui_state_interactive(const UmiApplicationExperienceUiState *state);

#ifdef __cplusplus
}
#endif

#endif
