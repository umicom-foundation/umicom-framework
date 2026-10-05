/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/compatibility.h
 *
 * PURPOSE:
 *   Record deterministic compatibility decisions and explanatory evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_COMPATIBILITY
#define UMICOM_SDK_RUNTIME_COMPATIBILITY
#include "umicom/sdk_runtime/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime compatibility data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeCompatibility {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t compatible;
    uint64_t reason_code;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeCompatibility;
/**
 * Initialise sdk runtime compatibility from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_compatibility_init(UmiSdkRuntimeCompatibility *value, const char *id);
/**
 * Check that sdk runtime compatibility satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_compatibility_validate(const UmiSdkRuntimeCompatibility *value);
/**
 * Provide the sdk runtime compatibility set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_compatibility_set_path(UmiSdkRuntimeCompatibility *value, const char *path);
/**
 * Provide the sdk runtime compatibility set detail operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_compatibility_set_detail(UmiSdkRuntimeCompatibility *value, const char *detail);
/**
 * Provide the sdk runtime compatibility set compatible operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_compatibility_set_compatible(UmiSdkRuntimeCompatibility *value, uint64_t number);
/**
 * Provide the sdk runtime compatibility set reason code operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_compatibility_set_reason_code(UmiSdkRuntimeCompatibility *value, uint64_t number);
/**
 * Provide the sdk runtime compatibility set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_compatibility_set_state(UmiSdkRuntimeCompatibility *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime compatibility same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_compatibility_same_identity(const UmiSdkRuntimeCompatibility *left, const UmiSdkRuntimeCompatibility *right);

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
UmiStatus umi_sdk_runtime_compatibility_replace_if_current(UmiSdkRuntimeCompatibility *value,
    uint64_t expected_revision, const UmiSdkRuntimeCompatibility *proposal);

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
UmiStatus umi_sdk_runtime_compatibility_init_checked(UmiSdkRuntimeCompatibility *value, const char *id);

/** Encode this value using Framework's portable archive ownership rules in
 * value_archive.h. NULL bytes with zero capacity measures the exact size.
 * Decoding checks the complete schema, checksum, text bounds and domain
 * validator before publishing. Structure size is rebuilt for the local host;
 * a saved revision is evidence, not authority to replace a live owner.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_sdk_runtime_compatibility_archive_encode(const UmiSdkRuntimeCompatibility *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_sdk_runtime_compatibility_archive_decode(const void *bytes, size_t byte_count,
    UmiSdkRuntimeCompatibility *value);

#ifdef __cplusplus
}
#endif
#endif
