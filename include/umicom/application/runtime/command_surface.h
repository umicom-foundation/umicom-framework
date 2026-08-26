/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/command_surface.h
 *
 * PURPOSE:
 *   Generate deterministic panel, layout and feature command descriptors from canonical application metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_COMMAND_SURFACE_H
#define UMICOM_APPLICATION_RUNTIME_COMMAND_SURFACE_H

#include "umicom/application/runtime/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiApplicationCommandDescriptor {
    UmiApplicationCommandKind kind;
    const char *target_id;
    const char *title;
    UmiExperienceFeatureState feature_state;
} UmiApplicationCommandDescriptor;

typedef struct UmiApplicationCommandSurface {
    uint32_t structure_size;
    UmiApplicationCommandDescriptor commands[UMI_APPLICATION_RUNTIME_MAX_COMMANDS];
    size_t command_count;
} UmiApplicationCommandSurface;

UmiStatus umi_application_command_surface_build(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationCommandSurface *out_surface);
const UmiApplicationCommandDescriptor *umi_application_command_surface_find(
    const UmiApplicationCommandSurface *surface,
    UmiApplicationCommandKind kind,
    const char *target_id);

#ifdef __cplusplus
}
#endif

#endif
