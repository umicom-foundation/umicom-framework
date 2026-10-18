/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/binding_endpoint.h
 *
 * PURPOSE:
 *   Represent one source or destination property endpoint in the visual binding editor.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_BINDING_ENDPOINT_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_BINDING_ENDPOINT_H
#include "umicom/designer/visual_designer/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer binding endpoint data shared with callers of this public contract.
 */
typedef struct UmiRadBindingEndpoint {
    char node_id[UMI_RAD_ID_CAPACITY];
    char property_path[UMI_RAD_PATH_CAPACITY];
    bool output;
} UmiRadBindingEndpoint;
/**
 * Initialise visual designer binding endpoint from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_rad_binding_endpoint_init(UmiRadBindingEndpoint *item);
/**
 * Check that visual designer binding endpoint satisfies its contract before another service relies on
 * it.
 */
int umi_rad_binding_endpoint_is_valid(const UmiRadBindingEndpoint *item);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_rad_binding_endpoint_archive_encode(const UmiRadBindingEndpoint *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_rad_binding_endpoint_archive_decode(const void *bytes, size_t byte_count,
    UmiRadBindingEndpoint *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiRadBindingEndpoint object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_rad_binding_endpoint_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiRadBindingEndpoint *workspace, UmiRadBindingEndpoint *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_rad_binding_endpoint_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiRadBindingEndpoint *expected, UmiRadBindingEndpoint *value);


#ifdef __cplusplus
}
#endif
#endif
