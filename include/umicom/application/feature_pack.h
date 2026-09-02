/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/feature_pack.h
 *
 * PURPOSE:
 *   Define reusable bundles of Framework capabilities and professional
 *   components so future products can remain thin composition layers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_FEATURE_PACK_H
#define UMICOM_APPLICATION_FEATURE_PACK_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_FEATURE_PACK_API_VERSION 1U
#define UMI_APPLICATION_FEATURE_PACK_MAX_CAPABILITIES 32U
#define UMI_APPLICATION_FEATURE_PACK_MAX_COMPONENTS 32U

typedef uint32_t UmiApplicationFeaturePackFlags;
enum {
    UMI_APPLICATION_FEATURE_PACK_HEADLESS = 1U << 0,
    UMI_APPLICATION_FEATURE_PACK_PRESENTATION = 1U << 1,
    UMI_APPLICATION_FEATURE_PACK_DATA = 1U << 2,
    UMI_APPLICATION_FEATURE_PACK_DEVELOPMENT = 1U << 3,
    UMI_APPLICATION_FEATURE_PACK_AI = 1U << 4,
    UMI_APPLICATION_FEATURE_PACK_DELIVERY = 1U << 5,
    UMI_APPLICATION_FEATURE_PACK_ENTERPRISE = 1U << 6,
    UMI_APPLICATION_FEATURE_PACK_EXTENSIBLE = 1U << 7
};

/**
 * Represent the application feature pack definition data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationFeaturePackDefinition {
    uint32_t structure_size;
    uint32_t api_version;
    const char *pack_id;
    const char *title;
    const char *description;
    const char *const *required_capabilities;
    size_t required_capability_count;
    const char *const *optional_capabilities;
    size_t optional_capability_count;
    const char *const *component_ids;
    size_t component_count;
    UmiApplicationFeaturePackFlags flags;
} UmiApplicationFeaturePackDefinition;

/**
 * Return the number of records represented by application feature pack catalogue without
 * changing their state.
 */
size_t umi_application_feature_pack_catalogue_count(void);

/**
 * Find application feature pack catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationFeaturePackDefinition *
umi_application_feature_pack_catalogue_at(size_t index);

/**
 * Find application feature pack catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationFeaturePackDefinition *
umi_application_feature_pack_catalogue_find(const char *pack_id);

/**
 * Check that application feature pack satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_application_feature_pack_validate(
    const UmiApplicationFeaturePackDefinition *pack);

#ifdef __cplusplus
}
#endif

#endif
