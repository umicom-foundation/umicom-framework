/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/preset.h
 *
 * PURPOSE:
 *   Define reusable thin-application archetypes built from Framework feature
 *   packs. Studio New Project and native scaffolding can share this catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the application preset definition data shared with callers of this public
 * contract.
 */
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

/**
 * Return the number of records represented by application preset catalogue without
 * changing their state.
 */
size_t umi_application_preset_catalogue_count(void);

/**
 * Find application preset catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiApplicationPresetDefinition *
umi_application_preset_catalogue_at(size_t index);

/**
 * Find application preset catalogue while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiApplicationPresetDefinition *
umi_application_preset_catalogue_find(const char *preset_id);

/**
 * Check that application preset satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_application_preset_validate(
    const UmiApplicationPresetDefinition *preset);

#ifdef __cplusplus
}
#endif

#endif
