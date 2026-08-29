/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/standard_states.h
 *
 * PURPOSE:
 *   Provide plain-language constructors for the standard states every reusable
 *   component must support before product-specific styling is applied.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_STANDARD_STATES_H
#define UMICOM_APPLICATION_EXPERIENCE_STANDARD_STATES_H

#include "umicom/application/experience/ui_state.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_application_experience_state_ready(UmiApplicationExperienceUiState *state,
                                                 const char *subject);
UmiStatus umi_application_experience_state_loading(UmiApplicationExperienceUiState *state,
                                                   const char *subject);
UmiStatus umi_application_experience_state_empty(UmiApplicationExperienceUiState *state,
                                                 const char *subject,
                                                 const char *action_command_id);
UmiStatus umi_application_experience_state_error(UmiApplicationExperienceUiState *state,
                                                 const char *message, const char *retry_command_id);
UmiStatus umi_application_experience_state_offline(UmiApplicationExperienceUiState *state,
                                                   const char *reconnect_command_id);
UmiStatus
umi_application_experience_state_permission_required(UmiApplicationExperienceUiState *state,
                                                     const char *request_access_command_id);

#ifdef __cplusplus
}
#endif

#endif
