/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/repository/operations/repository_operation_result.c
 *
 * PURPOSE:
 *   Implement represent operation output and structured counts for CLI and workbench consumers.
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

#include "umicom/repository/repository_operation_result.h"
#include <string.h>
/* Initialise result storage to a deterministic successful empty value. */
void umi_repository_operation_result_init(UmiRepositoryOperationResult *result){/* Preserve the original failure result so the caller can respond to the correct cause. */ if(!result)return;(void)memset(result,0,sizeof(*result));result->status=UMI_STATUS_OK;}
