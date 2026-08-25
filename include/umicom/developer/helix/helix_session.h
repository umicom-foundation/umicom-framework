/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer/helix/helix_session.h
 *
 * PURPOSE:
 *   Aggregate goal, plan, budgets, checkpoints, validation and lifecycle state for one Helix run.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_HELIX_HELIX_SESSION_H
#define UMICOM_DEVELOPER_HELIX_HELIX_SESSION_H
#include "umicom/developer/helix/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiHelixSession { uint32_t structure_size; uint32_t api_version; char id[UMI_HELIX_ID_CAPACITY]; UmiHelixState state; UmiHelixAutonomyMode autonomy; uint64_t baseline_fingerprint; uint64_t current_fingerprint; uint32_t iteration; uint32_t max_iterations; size_t blockers; bool approved; bool checkpoint_ready; } UmiHelixSession;
/* Initialise a supervised Helix session with a bounded iteration budget. */
UmiStatus umi_helix_session_init(UmiHelixSession *session,const char *id,uint32_t max_iterations);
/* Advance through planned, ready, running and terminal states only when governance preconditions hold. */
UmiStatus umi_helix_session_transition(UmiHelixSession *session,UmiHelixState next);
/* Report whether another repair/regeneration iteration is permitted. */
bool umi_helix_session_can_iterate(const UmiHelixSession *session);
#ifdef __cplusplus
}
#endif
#endif
