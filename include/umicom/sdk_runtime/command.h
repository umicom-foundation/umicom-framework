/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk_runtime/command.h
 *
 * PURPOSE:
 *   Describe typed Master Controller SDK/runtime commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_SDK_RUNTIME_COMMAND
#define UMICOM_SDK_RUNTIME_COMMAND
#include "umicom/sdk_runtime/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the sdk runtime command data shared with callers of this public contract.
 */
typedef struct UmiSdkRuntimeCommand {
    uint32_t structure_size;
    char id[UMI_SDK_RUNTIME_ID_CAPACITY];
    char path[UMI_SDK_RUNTIME_PATH_CAPACITY];
    char detail[UMI_SDK_RUNTIME_TEXT_CAPACITY];
    uint64_t kind;
    uint64_t sequence;
    uint64_t revision;
    UmiSdkRuntimeState state;
    bool enabled;
} UmiSdkRuntimeCommand;
/**
 * Initialise sdk runtime command from caller-provided values so later operations receive a
 * known state.
 */
void umi_sdk_runtime_command_init(UmiSdkRuntimeCommand *value, const char *id);
/**
 * Check that sdk runtime command satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_sdk_runtime_command_validate(const UmiSdkRuntimeCommand *value);
/**
 * Provide the sdk runtime command set path operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_command_set_path(UmiSdkRuntimeCommand *value, const char *path);
/**
 * Provide the sdk runtime command set detail operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_command_set_detail(UmiSdkRuntimeCommand *value, const char *detail);
/**
 * Provide the sdk runtime command set kind operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_command_set_kind(UmiSdkRuntimeCommand *value, uint64_t number);
/**
 * Provide the sdk runtime command set sequence operation used by this module and its
 * client applications.
 */
UmiStatus umi_sdk_runtime_command_set_sequence(UmiSdkRuntimeCommand *value, uint64_t number);
/**
 * Provide the sdk runtime command set state operation used by this module and its client
 * applications.
 */
UmiStatus umi_sdk_runtime_command_set_state(UmiSdkRuntimeCommand *value, UmiSdkRuntimeState state);
/**
 * Provide the sdk runtime command same identity operation used by this module and its
 * client applications.
 */
bool umi_sdk_runtime_command_same_identity(const UmiSdkRuntimeCommand *left, const UmiSdkRuntimeCommand *right);

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
UmiStatus umi_sdk_runtime_command_replace_if_current(UmiSdkRuntimeCommand *value,
    uint64_t expected_revision, const UmiSdkRuntimeCommand *proposal);

#ifdef __cplusplus
}
#endif
#endif
