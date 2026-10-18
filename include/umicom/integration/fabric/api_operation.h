/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/api_operation.h
 *
 * PURPOSE:
 *   Describe one API operation including method, route, schemas and idempotency semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_API_OPERATION_H
#define UMICOM_INTEGRATION_FABRIC_API_OPERATION_H

#include <stddef.h>
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/integration/fabric/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the fabric api operation data shared with callers of this public contract.
 */
typedef struct UmiFabricApiOperation {
    char operation_id[UMI_FABRIC_ID_CAPACITY];
    char method[UMI_FABRIC_ID_CAPACITY];
    char path[UMI_FABRIC_URI_CAPACITY];
    char request_schema[UMI_FABRIC_ID_CAPACITY];
    char response_schema[UMI_FABRIC_ID_CAPACITY];
    bool idempotent;
} UmiFabricApiOperation;

/**
 * Initialise fabric api operation from caller-provided values so later operations receive
 * a known state.
 */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_fabric_api_operation_init(UmiFabricApiOperation *item, const char *operation_id, const char *method, const char *path, const char *request_schema, const char *response_schema, bool idempotent);
/**
 * Check that fabric api operation satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fabric_api_operation_validate(const UmiFabricApiOperation *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fabric_api_operation_archive_encode(const UmiFabricApiOperation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fabric_api_operation_archive_decode(const void *bytes, size_t byte_count,
    UmiFabricApiOperation *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiFabricApiOperation object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_fabric_api_operation_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiFabricApiOperation *workspace, UmiFabricApiOperation *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_fabric_api_operation_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiFabricApiOperation *expected, UmiFabricApiOperation *value);


#ifdef __cplusplus
}
#endif
#endif
