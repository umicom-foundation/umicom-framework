/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/consumer.h
 *
 * PURPOSE:
 *   Describe one external application that consumes the installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * SDK APIs verify that an application can consume an installed Umicom Framework package without depending on private source-tree details.
 */
#ifndef UMICOM_SDK_CONSUMER_H
#define UMICOM_SDK_CONSUMER_H
#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/sdk/compatibility.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk consumer data shared with callers of this public contract.
 */
typedef struct UmiSdkConsumer { const char *application_id; UmiSdkRequirement requirement; const char *const *required_targets; size_t target_count; } UmiSdkConsumer;
/**
 * Check that sdk consumer satisfies its contract before another service relies on it.
 */
/**
 * All input strings must be NUL-terminated and readable for the call. A nonzero
 * target_count requires that many readable pointer entries. Validation rejects
 * null storage and unrepresentable byte extents, but cannot discover allocation
 * lengths from C pointers. No input is modified or retained.
 */
UmiStatus umi_sdk_consumer_validate(const UmiSdkConsumer *consumer);

/** A complete comparison of declarations, independent of files or libraries.
 * Counts refer to required entries, including duplicates. first_missing_index is
 * SIZE_MAX when none are absent. The summary owns only scalar values; it remains
 * valid after input storage is released. Existing consumer/manifest layouts are
 * unchanged by this separate result type.
 */
typedef struct UmiSdkConsumerCheck {
    size_t targets_checked;
    size_t targets_missing;
    size_t first_missing_index;
    int version_abi_compatible;
    int declarations_satisfied;
} UmiSdkConsumerCheck;

/** Compare validated declarations using exact, case-sensitive target names.
 * A target is present if any manifest component declares its name; the component
 * required flag does not remove it from that inventory. Extra declared targets
 * are allowed. Duplicate required entries are checked and counted separately.
 *
 * OK means the comparison completed, even when declarations_satisfied is false.
 * Invalid input returns INVALID_ARGUMENT without changing any output byte.
 * The caller supplies nonoverlapping output and stable, readable inputs for this
 * synchronous call. No filesystem access, allocation, process launch, loading,
 * compilation or actual ABI verification is performed. Worst-case work is
 * target_count * component_count string comparisons; bound untrusted inventories
 * before calling. Serialise access to caller-owned declarations when necessary.
 */
UmiStatus umi_sdk_consumer_check(const UmiSdkConsumer *consumer,
    const UmiSdkManifest *manifest, UmiSdkConsumerCheck *out_result);
#ifdef __cplusplus
}
#endif
#endif
