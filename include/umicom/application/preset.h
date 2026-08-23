/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/preset.h
 *
 * PURPOSE:
 *   Define reusable thin-application archetypes built from Framework feature
 *   packs. Studio New Project and native scaffolding can share this catalogue.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESET_H
#define UMICOM_APPLICATION_PRESET_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/application/definition.h"
#include "umicom/application/feature_pack.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_PRESET_API_VERSION 1U
#define UMI_APPLICATION_PRESET_MAX_PACKS 12U

typedef struct UmiApplicationPresetDefinition {
    uint32_t structure_size;
    uint32_t api_version;
    const char *preset_id;
    const char *title;
    const char *description;
    UmiApplicationFamily family;
    unsigned recommended_frontends;
    uint32_t application_flags;
    const char *const *feature_pack_ids;
    size_t feature_pack_count;
} UmiApplicationPresetDefinition;

size_t umi_application_preset_catalogue_count(void);

const UmiApplicationPresetDefinition *
umi_application_preset_catalogue_at(size_t index);

const UmiApplicationPresetDefinition *
umi_application_preset_catalogue_find(const char *preset_id);

UmiStatus umi_application_preset_validate(
    const UmiApplicationPresetDefinition *preset);

#ifdef __cplusplus
}
#endif

#endif
