/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/developer_operations/projection.h
 *
 * PURPOSE:
 *   Expose workbench projections beside executable developer operations without changing either legacy snapshot ABI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEVELOPER_OPERATIONS_PROJECTION_H
#define UMICOM_DEVELOPER_OPERATIONS_PROJECTION_H
#include <stdint.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C" {
#endif

/* The old compact header and executable-operation header independently used
 * UmiDeveloperOperationSnapshot for different records. Neither old layout is
 * renamed or merged. Consumers of both services use this explicit facade. */
#define UMI_DEVELOPER_PROJECTION_ID_CAPACITY 96U
#define UMI_DEVELOPER_PROJECTION_LABEL_CAPACITY 128U
typedef struct UmiDeveloperOperationProjectionInput {
    const char *label;
    uint64_t primary, secondary, total;
    double baseline, threshold;
    int active, blocked;
    uint64_t revision;
} UmiDeveloperOperationProjectionInput;

typedef struct UmiDeveloperOperationProjectionSnapshot {
    char id[UMI_DEVELOPER_PROJECTION_ID_CAPACITY];
    char label[UMI_DEVELOPER_PROJECTION_LABEL_CAPACITY];
    uint64_t primary, secondary, total, missed;
    double percent, delta;
    int active, blocked, ready, attention;
    uint64_t revision;
} UmiDeveloperOperationProjectionSnapshot;

/** Initialise a caller-owned projection request. label, when provided, remains
 * borrowed for the duration of Evaluate; no process or GUI is started. */
void UmiDeveloperOperationProjectionInputInit(UmiDeveloperOperationProjectionInput *input);
/** Delegate to the existing Framework catalogue using field-by-field copies.
 * Output is zeroed on failure. This header can be included before or after
 * umicom/developer/operation.h and does not leak the conflicting compact type. */
UmiStatus UmiDeveloperOperationsProjectionEvaluate(const char *id,
    const UmiDeveloperOperationProjectionInput *input,
    UmiDeveloperOperationProjectionSnapshot *outSnapshot);
#ifdef __cplusplus
}
#endif
#endif
