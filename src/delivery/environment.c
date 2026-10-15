/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/environment.c
 *
 * PURPOSE:
 *   Describe the target operating system, architecture and build configuration of a release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Release compatibility depends on where a package is intended to run, so this metadata is explicit rather than inferred from filenames.
 */

#include "umicom/delivery/environment.h"
#include "../base/value_construction_internal.h"
#include "delivery_internal.h"
#include <string.h>
/*
 * Initialise delivery environment from caller-provided values so later operations receive
 * a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_delivery_environment_init(UmiDeliveryEnvironment *environment,
                                        const char *operating_system,
                                        const char *architecture,
                                        const char *configuration)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || operating_system == NULL ||
        architecture == NULL || configuration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(environment, 0, sizeof(*environment));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_delivery_copy_text(environment->operating_system, sizeof(environment->operating_system), operating_system) != UMI_STATUS_OK ||
        umi_delivery_copy_text(environment->architecture, sizeof(environment->architecture), architecture) != UMI_STATUS_OK ||
        umi_delivery_copy_text(environment->configuration, sizeof(environment->configuration), configuration) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_delivery_environment_init_build_value(UmiDeliveryEnvironment *environment,
                                        const char *operating_system,
                                        const char *architecture,
                                        const char *configuration)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (environment == NULL || operating_system == NULL ||
        architecture == NULL || configuration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(environment, 0, sizeof(*environment));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_delivery_copy_text(environment->operating_system, sizeof(environment->operating_system), operating_system) != UMI_STATUS_OK ||
        umi_delivery_copy_text(environment->architecture, sizeof(environment->architecture), architecture) != UMI_STATUS_OK ||
        umi_delivery_copy_text(environment->configuration, sizeof(environment->configuration), configuration) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_delivery_environment_init, UmiDeliveryEnvironment, environment,
    (UmiDeliveryEnvironment *environment, const char *operating_system, const char *architecture, const char *configuration),
    umi_delivery_environment_init_build_value(candidate, operating_system, architecture, configuration))
