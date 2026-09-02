/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/toolchain/operation/requirement.c
 *
 * PURPOSE:
 *   Implement validation and defaults for scoped tool requirements.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable capability. Applications remain thin clients
 *   and must not duplicate discovery, repository policy or operational state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/toolchain/requirement.h"

/*
 * Initialise toolchain requirement from caller-provided values so later operations receive
 * a known state.
 */
void umi_toolchain_requirement_init(UmiToolchainRequirement *requirement,
                                    UmiToolKind kind,
                                    int required)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (requirement == NULL) return;
    requirement->kind = kind;
    requirement->required = required != 0;
    requirement->validate_version = 1;
}

/*
 * Check that toolchain requirement satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_toolchain_requirement_validate(const UmiToolchainRequirement *requirement)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (requirement == NULL || requirement->kind < 0 ||
        requirement->kind >= UMI_TOOL_COUNT) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}
