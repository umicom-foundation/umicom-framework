/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/vcs/repository.h
 *
 * PURPOSE:
 *   Project canonical VCS Repository state into a bounded workbench snapshot.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_VCS_REPOSITORY_H
#define UMICOM_DEVELOPER_OPERATIONS_VCS_REPOSITORY_H

#include "umicom/developer_operations/types.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus UmiDeveloperOperationsEvaluateVcsRepository(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif
#endif
