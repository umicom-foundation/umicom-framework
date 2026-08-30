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

/* BEGINNER NOTE:
 * A platform matrix lets the release process distinguish Windows x64 packages from future Linux or RISC-V builds.
 */

#include "umicom/delivery/platform_matrix.h"
#include "delivery_internal.h"
#include <string.h>
void umi_platform_matrix_init(UmiPlatformMatrix *matrix)
{
    if (matrix != NULL) (void)memset(matrix, 0, sizeof(*matrix));
}
UmiStatus umi_platform_matrix_add(UmiPlatformMatrix *matrix,
                                  const char *operating_system,
                                  const char *architecture)
{
    UmiPlatformTarget *target;
    if (matrix == NULL || operating_system == NULL || architecture == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (matrix->count >= 32U) return UMI_STATUS_CAPACITY_EXCEEDED;
    target = &matrix->targets[matrix->count++];
    (void)memset(target, 0, sizeof(*target));
    if (umi_delivery_copy_text(target->operating_system, sizeof(target->operating_system), operating_system) != UMI_STATUS_OK ||
        umi_delivery_copy_text(target->architecture, sizeof(target->architecture), architecture) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    return UMI_STATUS_OK;
}
int umi_platform_matrix_supports(const UmiPlatformMatrix *matrix,
                                 const char *operating_system,
                                 const char *architecture)
{
    size_t i;
    if (matrix == NULL || operating_system == NULL || architecture == NULL) return 0;
    for (i = 0U; i < matrix->count; ++i) {
        if (strcmp(matrix->targets[i].operating_system, operating_system) == 0 &&
            strcmp(matrix->targets[i].architecture, architecture) == 0) return 1;
    }
    return 0;
}
