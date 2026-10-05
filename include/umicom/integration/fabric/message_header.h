/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/integration/fabric/message_header.h
 *
 * PURPOSE:
 *   Represent immutable message identity, correlation and tenant metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_INTEGRATION_FABRIC_MESSAGE_HEADER_H
#define UMICOM_INTEGRATION_FABRIC_MESSAGE_HEADER_H

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
 * Represent the fabric message header data shared with callers of this public contract.
 */
typedef struct UmiFabricMessageHeader {
    char message_id[UMI_FABRIC_ID_CAPACITY];
    char correlation_id[UMI_FABRIC_ID_CAPACITY];
    char causation_id[UMI_FABRIC_ID_CAPACITY];
    char tenant_id[UMI_FABRIC_ID_CAPACITY];
    char content_type[UMI_FABRIC_TEXT_CAPACITY];
    uint64_t created_ms;
} UmiFabricMessageHeader;

/**
 * Initialise fabric message header from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_fabric_message_header_init(UmiFabricMessageHeader *item, const char *message_id, const char *correlation_id, const char *tenant_id, const char *content_type, uint64_t created_ms);
/**
 * Check that fabric message header satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_fabric_message_header_validate(const UmiFabricMessageHeader *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_fabric_message_header_archive_encode(const UmiFabricMessageHeader *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_fabric_message_header_archive_decode(const void *bytes, size_t byte_count,
    UmiFabricMessageHeader *value);

#ifdef __cplusplus
}
#endif
#endif
