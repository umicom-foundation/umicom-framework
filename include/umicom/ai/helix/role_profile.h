/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ai/helix/role_profile.h
 *
 * PURPOSE:
 *   Map the existing five Helix specialist roles to bounded reasoning responsibilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_AI_HELIX_ROLE_PROFILE_H
#define INCLUDE_UMICOM_AI_HELIX_ROLE_PROFILE_H

#include <stdint.h>
#include "umicom/ai/helix/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAiHelixRoleProfile {
    UmiHelixAgentRole role;
    const char *name;
    const char *purpose;
    uint32_t action_mask;
} UmiAiHelixRoleProfile;

const UmiAiHelixRoleProfile *umi_ai_helix_role_profile_find(
    UmiHelixAgentRole role);
int umi_ai_helix_role_allows_action(UmiHelixAgentRole role,
                                    UmiHelixActionKind kind);

#ifdef __cplusplus
}
#endif

#endif
