/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/repository_operation_exit.c
 *
 * PURPOSE:
 *   Implement translate operation results into deterministic CLI-style exit codes.
 *
 * ARCHITECTURE:
 *   Extend the existing Framework-owned repository control and maintenance
 *   capabilities. This module does not duplicate VCS, source-control, lock,
 *   maintenance or doctor state already present in Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/repository/repository_operation_exit.h"

/* Translate operation results into deterministic CLI-style exit codes. */
int umi_repository_operation_exit_code(
    const UmiRepositoryOperationResult *result)
{
    /* Invalid caller state maps to the conventional CLI usage/error code. */
    if (result == NULL) {
        return 2;
    }
    /* Backend execution failure is distinct from policy findings. */
    if (result->status != UMI_STATUS_OK) {
        return 1;
    }
    /* Typed error findings make the operation unsuccessful for automation. */
    if (result->error_count > 0U) {
        return 1;
    }
    return 0;
}
