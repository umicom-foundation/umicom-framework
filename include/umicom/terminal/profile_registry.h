/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/terminal/profile_registry.h
 *
 * PURPOSE:
 *   Own terminal profiles, default selection and deterministic platform
 *   defaults for every Umicom application.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TERMINAL_PROFILE_REGISTRY_H
#define UMICOM_TERMINAL_PROFILE_REGISTRY_H

#include "umicom/terminal/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the terminal profile registry data shared with callers of this public
 * contract.
 */
typedef struct UmiTerminalProfileRegistry UmiTerminalProfileRegistry;

/**
 * Initialise terminal profile registry from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_terminal_profile_registry_create(
    UmiTerminalProfileRegistry **out_registry);
/**
 * Release or reset state held by terminal profile registry so the same storage can be
 * reused safely.
 */
void umi_terminal_profile_registry_destroy(UmiTerminalProfileRegistry *registry);
/**
 * Add terminal profile registry only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_terminal_profile_registry_register(
    UmiTerminalProfileRegistry *registry,
    const UmiTerminalProfile *profile);
/**
 * Remove terminal profile registry while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_terminal_profile_registry_remove(
    UmiTerminalProfileRegistry *registry,
    const char *profile_id);
/**
 * Find terminal profile registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_terminal_profile_registry_find(
    const UmiTerminalProfileRegistry *registry,
    const char *profile_id,
    UmiTerminalProfile *out_profile);
/**
 * Find terminal profile registry while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_terminal_profile_registry_at(
    const UmiTerminalProfileRegistry *registry,
    size_t index,
    UmiTerminalProfile *out_profile);
/**
 * Return the number of records represented by terminal profile registry without changing
 * their state.
 */
size_t umi_terminal_profile_registry_count(
    const UmiTerminalProfileRegistry *registry);
/**
 * Provide the terminal profile registry revision operation used by this module and its
 * client applications.
 */
uint64_t umi_terminal_profile_registry_revision(
    const UmiTerminalProfileRegistry *registry);
/**
 * Provide the terminal profile registry set default operation used by this module and its
 * client applications.
 */
UmiStatus umi_terminal_profile_registry_set_default(
    UmiTerminalProfileRegistry *registry,
    const char *profile_id);
/**
 * Provide the terminal profile registry default id operation used by this module and its
 * client applications.
 */
const char *umi_terminal_profile_registry_default_id(
    const UmiTerminalProfileRegistry *registry);
/**
 * Provide the terminal profile registry add platform defaults operation used by this
 * module and its client applications.
 */
UmiStatus umi_terminal_profile_registry_add_platform_defaults(
    UmiTerminalProfileRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
