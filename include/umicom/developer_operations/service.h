/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/service.h
 *
 * PURPOSE:
 *   Discover and evaluate the operational projections that complete the
 *   Build/Test/Debug/Terminal/VCS developer workbench.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_SERVICE_H
#define UMICOM_DEVELOPER_OPERATIONS_SERVICE_H

#include <stddef.h>

#include "umicom/developer_operations/types.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t UmiDeveloperOperationsServiceCount(void);

UmiStatus UmiDeveloperOperationsServiceIdAt(
    size_t index,
    const char **outId);

UmiStatus UmiDeveloperOperationsServiceEvaluate(
    const char *id,
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);

#ifdef __cplusplus
}
#endif

#endif
