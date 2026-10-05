/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/health.h
 *
 * PURPOSE:
 *   Summarise installed SDK/runtime readiness for applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_HEALTH
#define UMICOM_SDK_RUNTIME_HEALTH
#include "umicom/sdk_runtime/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime health data shared with callers of this public contract.
 */
typedef struct UmiSdkRuntimeHealth {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t ready_components;
    uint64_t blocked_components;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeHealth;
/**
 * Initialise sdk runtime health from caller-provided values so later operations receive a
 * known state.
 */
void umi_sdk_runtime_health_init(UmiSdkRuntimeHealth *value, const char *id);
/**
 * Check that sdk runtime health satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_sdk_runtime_health_validate(const UmiSdkRuntimeHealth *value);
/**
 * Provide the sdk runtime health set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_health_set_path(UmiSdkRuntimeHealth *value, const char *path);
/**
 * Provide the sdk runtime health set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_health_set_detail(UmiSdkRuntimeHealth *value, const char *detail);
/**
 * Provide the sdk runtime health set ready components operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_health_set_ready_components(UmiSdkRuntimeHealth *value, uint64_t number);
/**
 * Provide the sdk runtime health set blocked components operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_health_set_blocked_components(UmiSdkRuntimeHealth *value, uint64_t number);
/**
 * Provide the sdk runtime health set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_health_set_state(UmiSdkRuntimeHealth *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime health same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_health_same_identity(const UmiSdkRuntimeHealth *left, const UmiSdkRuntimeHealth *right);

/** Publish a complete reviewed value while expected_revision still matches.
 * Copy the live record, edit that copy with the ordinary setters, and retain
 * the revision observed before editing. Both values must validate and have
 * the same ID. A stale review returns INVALID_STATE; exhausted revision space
 * returns CAPACITY_EXCEEDED. All failures leave the live value unchanged.
 * Success copies every proposal field and assigns one new live revision;
 * the proposal's own revision does not control publication. Self-assignment
 * is allowed and also advances once. Other overlapping storage is unsupported.
 * Serialize access on the owner. This value operation allocates nothing and
 * does not perform I/O, authenticate evidence or run the described workflow. */
UmiStatus umi_sdk_runtime_health_replace_if_current(UmiSdkRuntimeHealth *value,
    uint64_t expected_revision, const UmiSdkRuntimeHealth *proposal);

/** Construct the usual default value and report invalid input.
 * A null or empty identity returns INVALID_ARGUMENT. An identity without a
 * terminator in sizeof(value->id) readable bytes returns CAPACITY_EXCEEDED;
 * a shorter C string is read only through its terminator. The existing domain
 * validator checks the staged defaults before publication. Any refusal leaves
 * the destination unchanged. id may refer to the destination's own text.
 * This initializes a new value, resetting its fields and revision to the
 * established defaults; do not use it as a live edit while observers retain
 * that identity. It owns no resources, allocates nothing and performs no I/O.
 * Existing void initialization remains available for compatibility. */
UmiStatus umi_sdk_runtime_health_init_checked(UmiSdkRuntimeHealth *value, const char *id);

/** Encode this value using Framework's portable archive ownership rules in
 * value_archive.h. NULL bytes with zero capacity measures the exact size.
 * Decoding checks the complete schema, checksum, text bounds and domain
 * validator before publishing. Structure size is rebuilt for the local host;
 * a saved revision is evidence, not authority to replace a live owner.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_sdk_runtime_health_archive_encode(const UmiSdkRuntimeHealth *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_sdk_runtime_health_archive_decode(const void *bytes, size_t byte_count,
    UmiSdkRuntimeHealth *value);

#ifdef __cplusplus
}
#endif
#endif
