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

/* BEGINNER NOTE:
 * Release compatibility depends on where a package is intended to run, so this metadata is explicit rather than inferred from filenames.
 */

#include "umicom/delivery/environment.h"
#include "delivery_internal.h"
#include <string.h>
UmiStatus umi_delivery_environment_init(UmiDeliveryEnvironment *environment,
                                        const char *operating_system,
                                        const char *architecture,
                                        const char *configuration)
{
    if (environment == NULL || operating_system == NULL ||
        architecture == NULL || configuration == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(environment, 0, sizeof(*environment));
    if (umi_delivery_copy_text(environment->operating_system, sizeof(environment->operating_system), operating_system) != UMI_STATUS_OK ||
        umi_delivery_copy_text(environment->architecture, sizeof(environment->architecture), architecture) != UMI_STATUS_OK ||
        umi_delivery_copy_text(environment->configuration, sizeof(environment->configuration), configuration) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
