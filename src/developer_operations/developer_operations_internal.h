/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_operations/developer_operations_internal.h
 *
 * PURPOSE:
 *   Share projection policy evaluation across focused developer workbench
 *   components while canonical test/debug/terminal/VCS services stay authoritative.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_INTERNAL_H
#define UMICOM_DEVELOPER_OPERATIONS_INTERNAL_H

#include "umicom/developer_operations/types.h"

typedef enum UmiDeveloperOperationPolicyKind {
    UMI_DEVELOPER_OPERATION_RATIO = 1,
    UMI_DEVELOPER_OPERATION_DELTA = 2,
    UMI_DEVELOPER_OPERATION_THRESHOLD = 3,
    UMI_DEVELOPER_OPERATION_ACTIVITY = 4,
    UMI_DEVELOPER_OPERATION_COUNT = 5,
    UMI_DEVELOPER_OPERATION_CAPACITY = 6,
    UMI_DEVELOPER_OPERATION_CLEANLINESS = 7,
    UMI_DEVELOPER_OPERATION_CONFLICT = 8,
    UMI_DEVELOPER_OPERATION_PROGRESS = 9,
    UMI_DEVELOPER_OPERATION_ALERT = 10
} UmiDeveloperOperationPolicyKind;

typedef struct UmiDeveloperOperationPolicy {
    const char *id;
    const char *label;
    UmiDeveloperOperationPolicyKind kind;
    double defaultThreshold;
} UmiDeveloperOperationPolicy;

UmiStatus UmiDeveloperOperationsEvaluatePolicy(
    const UmiDeveloperOperationPolicy *policy,
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);

#endif
