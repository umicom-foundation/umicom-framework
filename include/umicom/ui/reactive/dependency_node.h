/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/dependency_node.h
 *
 * PURPOSE:
 *   Represent one property/computed-state node in the dependency graph.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_DEPENDENCY_NODE_H
#define UMICOM_UI_REACTIVE_DEPENDENCY_NODE_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive dependency node data shared with callers of this public
 * contract.
 */
typedef struct UmiUiReactiveDependencyNode {
    char node_id[UMI_UI_REACTIVE_ID_CAPACITY];
    bool computed;
    uint64_t revision;
} UmiUiReactiveDependencyNode;
/**
 * Initialise ui reactive dependency node from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_dependency_node_init(UmiUiReactiveDependencyNode *item);
/**
 * Check that ui reactive dependency node satisfies its contract before another service
 * relies on it.
 */
int umi_ui_reactive_dependency_node_valid(const UmiUiReactiveDependencyNode *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_dependency_node_archive_encode(const UmiUiReactiveDependencyNode *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_dependency_node_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveDependencyNode *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ui_reactive_dependency_node_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiUiReactiveDependencyNode *expected, UmiUiReactiveDependencyNode *value);


#ifdef __cplusplus
}
#endif
#endif
