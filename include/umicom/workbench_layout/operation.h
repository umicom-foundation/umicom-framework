/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_layout/operation.h
 *
 * PURPOSE:
 *   Represent deterministic, auditable layout mutations that can be applied, replayed, reversed, persisted and inspected independently of a GUI.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_LAYOUT_OPERATION_H
#define UMICOM_WORKBENCH_LAYOUT_OPERATION_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif

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

typedef struct UmiWorkbenchLayoutOperationResult {
    uint32_t structure_size;
    UmiStatus status;
    bool changed;
    uint64_t previous_revision;
    uint64_t resulting_revision;
    char message[UMI_WORKBENCH_LAYOUT_ERROR_CAPACITY];
} UmiWorkbenchLayoutOperationResult;

void umi_workbench_layout_operation_init(
    UmiWorkbenchLayoutOperation *operation,
    UmiWorkbenchLayoutOperationKind kind,
    const char *operation_id);

UmiStatus umi_workbench_layout_operation_validate(
    const UmiWorkbenchLayoutOperation *operation);

UmiStatus umi_workbench_layout_operation_apply(
    UmiWorkbenchLayoutDocument *document,
    const UmiWorkbenchLayoutOperation *operation,
    UmiWorkbenchLayoutOperationResult *out_result);

UmiStatus umi_workbench_layout_operation_inverse(
    const UmiWorkbenchLayoutDocument *before,
    const UmiWorkbenchLayoutOperation *operation,
    UmiWorkbenchLayoutOperation *out_inverse);

bool umi_workbench_layout_operation_is_metadata_only(
    const UmiWorkbenchLayoutOperation *operation);

bool umi_workbench_layout_operation_requires_unlocked_document(
    const UmiWorkbenchLayoutOperation *operation);

#ifdef __cplusplus
}
#endif

#endif
