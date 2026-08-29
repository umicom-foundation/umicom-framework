/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/state_transition.c
 *
 * PURPOSE:
 *   Enforce sensible UI-state transitions, including recovery from errors and
 *   preventing a completed state from jumping directly back to busy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/experience/state_transition.h"

#include <string.h>

#include "umicom/base/text.h"

int umi_application_experience_state_transition_allowed(
    UmiApplicationExperienceUiStateKind from_kind, UmiApplicationExperienceUiStateKind to_kind) {
  if (from_kind < UMI_APPLICATION_EXPERIENCE_UI_READY ||
      from_kind > UMI_APPLICATION_EXPERIENCE_UI_PERMISSION_REQUIRED ||
      to_kind < UMI_APPLICATION_EXPERIENCE_UI_READY ||
      to_kind > UMI_APPLICATION_EXPERIENCE_UI_PERMISSION_REQUIRED)
    return 0;
  if (from_kind == to_kind)
    return 1;
  if (from_kind == UMI_APPLICATION_EXPERIENCE_UI_SUCCESS &&
      to_kind == UMI_APPLICATION_EXPERIENCE_UI_BUSY)
    return 0;
  return 1;
}

UmiStatus umi_application_experience_state_transition_create(
    UmiApplicationExperienceStateTransition *transition,
    UmiApplicationExperienceUiStateKind from_kind, UmiApplicationExperienceUiStateKind to_kind,
    const char *reason, uint64_t sequence) {
  if (transition == NULL || reason == NULL || reason[0] == '\0' || sequence == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  if (!umi_application_experience_state_transition_allowed(from_kind, to_kind))
    return UMI_STATUS_INVALID_STATE;
  (void)memset(transition, 0, sizeof(*transition));
  transition->from_kind = from_kind;
  transition->to_kind = to_kind;
  transition->sequence = sequence;
  return umi_text_copy(transition->reason, sizeof(transition->reason), reason);
}
