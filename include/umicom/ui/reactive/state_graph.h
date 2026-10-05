/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/reactive/state_graph.h
 *
 * PURPOSE:
 *   Represent aggregate dependency/state graph health.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_REACTIVE_STATE_GRAPH_H
#define UMICOM_UI_REACTIVE_STATE_GRAPH_H
#include "umicom/ui/reactive/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the ui reactive state graph data shared with callers of this public contract.
 */
typedef struct UmiUiReactiveStateGraph {
    size_t node_count;
    size_t edge_count;
    size_t computed_count;
    bool acyclic;
    uint64_t revision;
} UmiUiReactiveStateGraph;
/**
 * Initialise ui reactive state graph from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_reactive_state_graph_init(UmiUiReactiveStateGraph *item);
/**
 * Check that ui reactive state graph satisfies its contract before another service relies
 * on it.
 */
int umi_ui_reactive_state_graph_valid(const UmiUiReactiveStateGraph *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ui_reactive_state_graph_archive_encode(const UmiUiReactiveStateGraph *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ui_reactive_state_graph_archive_decode(const void *bytes, size_t byte_count,
    UmiUiReactiveStateGraph *value);

#ifdef __cplusplus
}
#endif
#endif
