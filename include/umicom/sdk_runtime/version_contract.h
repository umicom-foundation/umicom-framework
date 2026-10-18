/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/version_contract.h
 *
 * PURPOSE:
 *   Retain package, schema and ABI compatibility requirements independently.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_VERSION_CONTRACT
#define UMICOM_SDK_RUNTIME_VERSION_CONTRACT
#include "umicom/sdk_runtime/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime version contract data shared with callers of this public
 * contract.
 */
typedef struct UmiSdkRuntimeVersionContract {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t major;
    uint64_t minor;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeVersionContract;
/**
 * Initialise sdk runtime version contract from caller-provided values so later operations
 * receive a known state.
 */
void umi_sdk_runtime_version_contract_init(UmiSdkRuntimeVersionContract *value, const char *id);
/**
 * Check that sdk runtime version contract satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_sdk_runtime_version_contract_validate(const UmiSdkRuntimeVersionContract *value);
/**
 * Provide the sdk runtime version contract set path operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_version_contract_set_path(UmiSdkRuntimeVersionContract *value, const char *path);
/**
 * Provide the sdk runtime version contract set detail operation used by this module and
 * its client applications.
 */
UmiStatus umi_sdk_runtime_version_contract_set_detail(UmiSdkRuntimeVersionContract *value, const char *detail);
/**
 * Provide the sdk runtime version contract set major operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_version_contract_set_major(UmiSdkRuntimeVersionContract *value, uint64_t number);
/**
 * Provide the sdk runtime version contract set minor operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_version_contract_set_minor(UmiSdkRuntimeVersionContract *value, uint64_t number);
/**
 * Provide the sdk runtime version contract set state operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_version_contract_set_state(UmiSdkRuntimeVersionContract *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime version contract same identity operation used by this module and
 * its client applications.
 */
bool umi_sdk_runtime_version_contract_same_identity(const UmiSdkRuntimeVersionContract *left, const UmiSdkRuntimeVersionContract *right);

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
UmiStatus umi_sdk_runtime_version_contract_replace_if_current(UmiSdkRuntimeVersionContract *value,
    uint64_t expected_revision, const UmiSdkRuntimeVersionContract *proposal);

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
UmiStatus umi_sdk_runtime_version_contract_init_checked(UmiSdkRuntimeVersionContract *value, const char *id);

/** Encode this value using Framework's portable archive ownership rules in
 * value_archive.h. NULL bytes with zero capacity measures the exact size.
 * Decoding checks the complete schema, checksum, text bounds and domain
 * validator before publishing. Structure size is rebuilt for the local host;
 * a saved revision is evidence, not authority to replace a live owner.
 * Keep source and destination storage separate. Neither call performs I/O. */
UmiStatus umi_sdk_runtime_version_contract_archive_encode(const UmiSdkRuntimeVersionContract *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_sdk_runtime_version_contract_archive_decode(const void *bytes, size_t byte_count,
    UmiSdkRuntimeVersionContract *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiSdkRuntimeVersionContract object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_sdk_runtime_version_contract_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiSdkRuntimeVersionContract *workspace, UmiSdkRuntimeVersionContract *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the registry publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_sdk_runtime_version_contract_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiSdkRuntimeVersionContract *expected, UmiSdkRuntimeVersionContract *value);


#ifdef __cplusplus
}
#endif
#endif
