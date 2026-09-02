/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/platform_condition.c
 *
 * PURPOSE:
 *   Implement the platform condition behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/platform_condition.c
 *
 * PURPOSE:
 *   Evaluate platform identifiers for conditional runtime composition.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/platform_condition.h"


#include <string.h>
/*
 * Provide the bootstrap platform condition match operation used by this module and its
 * client applications.
 */
bool umi_bootstrap_platform_condition_match(const char *platform_id,
                                            const char *expected_platform_id) {
    return platform_id != NULL && expected_platform_id != NULL &&
           strcmp(platform_id, expected_platform_id) == 0;
}
