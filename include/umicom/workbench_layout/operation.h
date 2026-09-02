/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/operation.h
 *
 * PURPOSE:
 *   Represent deterministic, auditable layout mutations that can be applied, replayed, reversed, persisted and inspected independently of a GUI.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_OPERATION_H
#define UMICOM_WORKBENCH_LAYOUT_OPERATION_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench layout operation data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutOperation {
    uint32_t structure_size;
    UmiWorkbenchLayoutOperationKind kind;
    char operation_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char actor_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char correlation_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char node_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char parent_node_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char secondary_node_id[UMI_WORKBENCH_LAYOUT_ID_CAPACITY];
    char text_value[UMI_WORKBENCH_LAYOUT_TEXT_CAPACITY];
    UmiWorkbenchLayoutNode node_value;
    UmiWorkbenchLayoutRect rect_value;
    double number_value;
    size_t index_value;
    uint32_t flags_value;
    bool bool_value;
    uint64_t expected_revision;
    uint64_t sequence;
    uint64_t timestamp_ms;
} UmiWorkbenchLayoutOperation;

/**
 * Represent the workbench layout operation result data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchLayoutOperationResult {
    uint32_t structure_size;
    UmiStatus status;
    bool changed;
    uint64_t previous_revision;
    uint64_t resulting_revision;
    char message[UMI_WORKBENCH_LAYOUT_ERROR_CAPACITY];
} UmiWorkbenchLayoutOperationResult;

/**
 * Initialise workbench layout operation from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_operation_init(
    UmiWorkbenchLayoutOperation *operation,
    UmiWorkbenchLayoutOperationKind kind,
    const char *operation_id);

/**
 * Check that workbench layout operation satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_workbench_layout_operation_validate(
    const UmiWorkbenchLayoutOperation *operation);

/**
 * Perform workbench layout operation through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_workbench_layout_operation_apply(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutOperation *operation,
    UmiWorkbenchLayoutOperationResult *out_result);

/**
 * Provide the workbench layout operation inverse operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_operation_inverse(
    const UmiWorkbenchLayoutDocument *before,
    const UmiWorkbenchLayoutOperation *operation,
    UmiWorkbenchLayoutOperation *out_inverse);

/**
 * Provide the workbench layout operation is metadata only operation used by this module
 * and its client applications.
 */
bool umi_workbench_layout_operation_is_metadata_only(
    const UmiWorkbenchLayoutOperation *operation);

/**
 * Provide the workbench layout operation requires unlocked document operation used by this
 * module and its client applications.
 */
bool umi_workbench_layout_operation_requires_unlocked_document(
    const UmiWorkbenchLayoutOperation *operation);

#ifdef __cplusplus
}
#endif

#endif
