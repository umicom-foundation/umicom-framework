/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/command_surface.h
 *
 * PURPOSE:
 *   Generate deterministic panel, layout and feature command descriptors from canonical application metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_COMMAND_SURFACE_H
#define UMICOM_APPLICATION_RUNTIME_COMMAND_SURFACE_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application command descriptor data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationCommandDescriptor {
    UmiApplicationCommandKind kind;
    const char *target_id;
    const char *title;
    /* Capability metadata lets a palette explain why a command is unavailable. */
    const char *required_capability;
    UmiExperienceFeatureState feature_state;
} UmiApplicationCommandDescriptor;

/**
 * Represent the application command surface data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationCommandSurface {
    uint32_t structure_size;
    UmiApplicationCommandDescriptor commands[UMI_APPLICATION_RUNTIME_MAX_COMMANDS];
    size_t command_count;
} UmiApplicationCommandSurface;

/**
 * Describe a command-palette query without allocating or owning catalogue text.
 */
typedef struct UmiApplicationCommandQuery {
    uint32_t structure_size;
    const char *text;
    bool include_unavailable;
    UmiApplicationCapabilityProbe capability_probe;
    void *capability_probe_data;
} UmiApplicationCommandQuery;

/**
 * Return bounded command indices and availability counts for one query.
 */
typedef struct UmiApplicationCommandQueryResult {
    uint32_t structure_size;
    size_t match_count;
    size_t available_count;
    size_t unavailable_count;
    size_t command_indices[UMI_APPLICATION_RUNTIME_MAX_COMMANDS];
    bool available[UMI_APPLICATION_RUNTIME_MAX_COMMANDS];
} UmiApplicationCommandQueryResult;

/**
 * Provide the application command surface build operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_command_surface_build(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationCommandSurface *out_surface);
/**
 * Find application command surface while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiApplicationCommandDescriptor *umi_application_command_surface_find(
    const UmiApplicationCommandSurface *surface,
    UmiApplicationCommandKind kind,
    const char *target_id);

/**
 * Search command titles and IDs while applying feature and capability
 * availability rules shared by every product palette.
 */
UmiStatus umi_application_command_surface_query(
    const UmiApplicationCommandSurface *surface,
    const UmiApplicationCommandQuery *query,
    UmiApplicationCommandQueryResult *out_result);

/**
 * Check whether one command is mature and supported by the optional capability provider.
 * Keeping this rule in one Framework function prevents palettes and command invokers from
 * disagreeing about which actions the user may run.
 */
bool umi_application_command_descriptor_is_available(
    const UmiApplicationCommandDescriptor *command,
    UmiApplicationCapabilityProbe capability_probe,
    void *capability_probe_data);

#ifdef __cplusplus
}
#endif

#endif
