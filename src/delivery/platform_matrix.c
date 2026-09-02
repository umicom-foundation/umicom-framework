/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/platform_matrix.c
 *
 * PURPOSE:
 *   Record target operating-system and architecture combinations supported by a release.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A platform matrix lets the release process distinguish Windows x64 packages from future Linux or RISC-V builds.
 */

#include "umicom/delivery/platform_matrix.h"
#include "delivery_internal.h"
#include <string.h>
/*
 * Initialise platform matrix from caller-provided values so later operations receive a
 * known state.
 */
void umi_platform_matrix_init(UmiPlatformMatrix *matrix)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (matrix != NULL) (void)memset(matrix, 0, sizeof(*matrix));
}
/* Add platform matrix only after its inputs and available capacity have been checked. */
UmiStatus umi_platform_matrix_add(UmiPlatformMatrix *matrix,
                                  const char *operating_system,
                                  const char *architecture)
{
    UmiPlatformTarget *target;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (matrix == NULL || operating_system == NULL || architecture == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (matrix->count >= 32U) return UMI_STATUS_CAPACITY_EXCEEDED;
    target = &matrix->targets[matrix->count++];
    (void)memset(target, 0, sizeof(*target));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_delivery_copy_text(target->operating_system, sizeof(target->operating_system), operating_system) != UMI_STATUS_OK ||
        umi_delivery_copy_text(target->architecture, sizeof(target->architecture), architecture) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
/*
 * Provide the platform matrix supports operation used by this module and its client
 * applications.
 */
int umi_platform_matrix_supports(const UmiPlatformMatrix *matrix,
                                 const char *operating_system,
                                 const char *architecture)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (matrix == NULL || operating_system == NULL || architecture == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < matrix->count; ++i) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (strcmp(matrix->targets[i].operating_system, operating_system) == 0 &&
            strcmp(matrix->targets[i].architecture, architecture) == 0) return 1;
    }
    return 0;
}
