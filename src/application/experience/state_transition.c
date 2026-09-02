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

/*
 * Provide the application experience state transition allowed operation used by this
 * module and its client applications.
 */
int umi_application_experience_state_transition_allowed(
    UmiApplicationExperienceUiStateKind from_kind, UmiApplicationExperienceUiStateKind to_kind) {
  /* Apply this operation only while the related capability or state is available. */
  if (from_kind < UMI_APPLICATION_EXPERIENCE_UI_READY ||
      from_kind > UMI_APPLICATION_EXPERIENCE_UI_PERMISSION_REQUIRED ||
      to_kind < UMI_APPLICATION_EXPERIENCE_UI_READY ||
      to_kind > UMI_APPLICATION_EXPERIENCE_UI_PERMISSION_REQUIRED)
    return 0;
  /* Apply this branch only when its contract condition is satisfied. */
  if (from_kind == to_kind)
    return 1;
  /* Apply this branch only when its contract condition is satisfied. */
  if (from_kind == UMI_APPLICATION_EXPERIENCE_UI_SUCCESS &&
      to_kind == UMI_APPLICATION_EXPERIENCE_UI_BUSY)
    return 0;
  return 1;
}

/*
 * Initialise application experience state transition from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_application_experience_state_transition_create(
    UmiApplicationExperienceStateTransition *transition,
    UmiApplicationExperienceUiStateKind from_kind, UmiApplicationExperienceUiStateKind to_kind,
    const char *reason, uint64_t sequence) {
  /*
   * Protect caller-owned memory by checking that required state is available before it is
   * used.
   */
  if (transition == NULL || reason == NULL || reason[0] == '\0' || sequence == 0U)
    return UMI_STATUS_INVALID_ARGUMENT;
  /* Apply this operation only while the related capability or state is available. */
  if (!umi_application_experience_state_transition_allowed(from_kind, to_kind))
    return UMI_STATUS_INVALID_STATE;
  (void)memset(transition, 0, sizeof(*transition));
  transition->from_kind = from_kind;
  transition->to_kind = to_kind;
  transition->sequence = sequence;
  return umi_text_copy(transition->reason, sizeof(transition->reason), reason);
}
