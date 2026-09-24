/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/types.h
 *
 * PURPOSE:
 *   Define compact workbench projections for coverage, debugging, terminal/task
 *   execution and VCS workflows without replacing their canonical services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_TYPES_H
#define UMICOM_DEVELOPER_OPERATIONS_TYPES_H

#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DEVELOPER_OPERATION_ID_CAPACITY 96U
#define UMI_DEVELOPER_OPERATION_LABEL_CAPACITY 128U

typedef struct UmiDeveloperOperationInput {
    const char *label;
    uint64_t primary;
    uint64_t secondary;
    uint64_t total;
    double baseline;
    double threshold;
    int active;
    int blocked;
    uint64_t revision;
} UmiDeveloperOperationInput;

typedef struct UmiDeveloperOperationSnapshot {
    char id[UMI_DEVELOPER_OPERATION_ID_CAPACITY];
    char label[UMI_DEVELOPER_OPERATION_LABEL_CAPACITY];
    uint64_t primary;
    uint64_t secondary;
    uint64_t total;
    uint64_t missed;
    double percent;
    double delta;
    int active;
    int blocked;
    int ready;
    int attention;
    uint64_t revision;
} UmiDeveloperOperationSnapshot;

typedef UmiStatus (*UmiDeveloperOperationEvaluator)(
    const UmiDeveloperOperationInput *input,
    UmiDeveloperOperationSnapshot *outSnapshot);

void UmiDeveloperOperationInputInit(UmiDeveloperOperationInput *input);
void UmiDeveloperOperationSnapshotInit(UmiDeveloperOperationSnapshot *snapshot);
UmiStatus UmiDeveloperOperationSnapshotValidate(
    const UmiDeveloperOperationSnapshot *snapshot);

#ifdef __cplusplus
}
#endif
#endif
