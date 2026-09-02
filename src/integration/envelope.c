/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/envelope.c
 *
 * PURPOSE:
 *   Implement the corresponding public Suite and Inter-Application Runtime contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/envelope.h"

#include <string.h>

/*
 * Initialise integration envelope from caller-provided values so later operations receive
 * a known state.
 */
void umi_integration_envelope_init(UmiIntegrationEnvelope *envelope)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (envelope != NULL) {
        (void)memset(envelope, 0, sizeof(*envelope));
    }
}

/*
 * Check that integration envelope satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_integration_envelope_validate(
    const UmiIntegrationEnvelope *envelope)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (envelope == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (envelope->source_application[0] == '\0' ||
        envelope->target_application[0] == '\0' ||
        envelope->topic[0] == '\0') {
        return UMI_STATUS_PARSE_ERROR;
    }
    return UMI_STATUS_OK;
}
