/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai/helix/role_profile.c
 *
 * PURPOSE:
 *   Provide role-specific reasoning boundaries for the five existing Helix agents.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ai/helix/role_profile.h"
#include <stddef.h>

#define ACTION_READ (UINT32_C(1) << 0)
#define ACTION_FILESYSTEM (UINT32_C(1) << 1)
#define ACTION_BUILD (UINT32_C(1) << 2)
#define ACTION_TEST (UINT32_C(1) << 3)
#define ACTION_REVIEW (UINT32_C(1) << 4)
#define ACTION_SOURCE_CONTROL (UINT32_C(1) << 5)

static const UmiAiHelixRoleProfile profiles[] = {
    {UMI_HELIX_AGENT_ROLE_DESIGNER,
     "Designer",
     "analyse architecture, dependencies and bounded design choices without editing files",
     ACTION_READ | ACTION_REVIEW},
    {UMI_HELIX_AGENT_ROLE_BUILDER,
     "Builder",
     "implement approved changes, build them and prepare source-control work for separate approval",
     ACTION_READ | ACTION_FILESYSTEM | ACTION_BUILD | ACTION_TEST | ACTION_SOURCE_CONTROL},
    {UMI_HELIX_AGENT_ROLE_SUGGESTION,
     "Suggestion",
     "surface evidence-based improvements and alternatives without mutating the workspace",
     ACTION_READ | ACTION_REVIEW},
    {UMI_HELIX_AGENT_ROLE_REVIEWER,
     "Reviewer",
     "inspect changes, tests and evidence independently from the builder",
     ACTION_READ | ACTION_TEST | ACTION_REVIEW},
    {UMI_HELIX_AGENT_ROLE_TEST,
     "Test",
     "build and execute validation while preserving test evidence",
     ACTION_READ | ACTION_BUILD | ACTION_TEST}
};

const UmiAiHelixRoleProfile *umi_ai_helix_role_profile_find(
    UmiHelixAgentRole role)
{
    size_t index;
    for (index = 0U; index < sizeof(profiles) / sizeof(profiles[0]); ++index) {
        if (profiles[index].role == role) {
            return &profiles[index];
        }
    }
    return NULL;
}

int umi_ai_helix_role_allows_action(UmiHelixAgentRole role,
                                    UmiHelixActionKind kind)
{
    const UmiAiHelixRoleProfile *profile = umi_ai_helix_role_profile_find(role);
    const uint32_t mask = umi_ai_helix_action_mask(kind);
    return profile != NULL && mask != 0U && (profile->action_mask & mask) != 0U;
}
