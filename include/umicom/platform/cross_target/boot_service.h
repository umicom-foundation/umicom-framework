/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/platform/cross_target/boot_service.h
 *
 * PURPOSE:
 *   Bind OS services to boot phases and validate dependency phase ordering.
 *
 * ARCHITECTURE:
 *   Framework owns reusable cross-target and Umicom OS semantics. Existing
 *   compiler/toolchain discovery, platform services and application runtimes
 *   remain authoritative and are composed rather than duplicated here.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_PLATFORM_CROSS_TARGET_BOOT_SERVICE_H
#define UMICOM_PLATFORM_CROSS_TARGET_BOOT_SERVICE_H

#include <stdbool.h>
#include "umicom/base/value_archive.h"
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/platform/cross_target/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/platform/cross_target/boot_phase.h"
/**
 * Represent the ct boot service data shared with callers of this public contract.
 */
typedef struct UmiCtBootService { char service_id[UMI_CT_ID_CAPACITY]; UmiCtBootPhase phase; bool essential; uint32_t timeout_ms; } UmiCtBootService;
/**
 * Check that ct boot service satisfies its contract before another service relies on it.
 */
UmiStatus umi_ct_boot_service_validate(const UmiCtBootService *service);
/**
 * Check that ct boot dependency phase satisfies its contract before another service relies
 * on it.
 */
bool umi_ct_boot_dependency_phase_valid(const UmiCtBootService *service,const UmiCtBootService *dependency);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_ct_boot_service_archive_encode(const UmiCtBootService *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_ct_boot_service_archive_decode(const void *bytes, size_t byte_count,
    UmiCtBootService *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_ct_boot_service_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiCtBootService *expected, UmiCtBootService *value);


#ifdef __cplusplus
}
#endif

#endif
