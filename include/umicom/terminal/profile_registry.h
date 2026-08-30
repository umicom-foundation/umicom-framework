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

typedef struct UmiTerminalProfileRegistry UmiTerminalProfileRegistry;

UmiStatus umi_terminal_profile_registry_create(
    UmiTerminalProfileRegistry **out_registry);
void umi_terminal_profile_registry_destroy(UmiTerminalProfileRegistry *registry);
UmiStatus umi_terminal_profile_registry_register(
    UmiTerminalProfileRegistry *registry,
    const UmiTerminalProfile *profile);
UmiStatus umi_terminal_profile_registry_remove(
    UmiTerminalProfileRegistry *registry,
    const char *profile_id);
UmiStatus umi_terminal_profile_registry_find(
    const UmiTerminalProfileRegistry *registry,
    const char *profile_id,
    UmiTerminalProfile *out_profile);
UmiStatus umi_terminal_profile_registry_at(
    const UmiTerminalProfileRegistry *registry,
    size_t index,
    UmiTerminalProfile *out_profile);
size_t umi_terminal_profile_registry_count(
    const UmiTerminalProfileRegistry *registry);
uint64_t umi_terminal_profile_registry_revision(
    const UmiTerminalProfileRegistry *registry);
UmiStatus umi_terminal_profile_registry_set_default(
    UmiTerminalProfileRegistry *registry,
    const char *profile_id);
const char *umi_terminal_profile_registry_default_id(
    const UmiTerminalProfileRegistry *registry);
UmiStatus umi_terminal_profile_registry_add_platform_defaults(
    UmiTerminalProfileRegistry *registry);

#ifdef __cplusplus
}
#endif
#endif
