/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/plugin/extension_point.h
 *
 * PURPOSE:
 *   Publish typed extension points and validate contribution ownership,
 *   cardinality and target contracts before activation.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *----------------------------------------------------------------------------*/
#ifndef UMICOM_PLUGIN_EXTENSION_POINT_H
#define UMICOM_PLUGIN_EXTENSION_POINT_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/plugin/contribution.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_PLUGIN_EXTENSION_POINT_MAX 256U
#define UMI_PLUGIN_EXTENSION_POINT_SCHEMA_CAPACITY 256U

typedef enum UmiPluginExtensionCardinality {
    UMI_PLUGIN_EXTENSION_ZERO_OR_ONE = 0,
    UMI_PLUGIN_EXTENSION_ONE = 1,
    UMI_PLUGIN_EXTENSION_MANY = 2
} UmiPluginExtensionCardinality;

typedef struct UmiPluginExtensionPoint {
    char extension_id[UMI_PLUGIN_CONTRIBUTION_TYPE_CAPACITY];
    char owner_id[UMI_PLUGIN_ID_CAPACITY];
    char schema[UMI_PLUGIN_EXTENSION_POINT_SCHEMA_CAPACITY];
    UmiPluginExtensionCardinality cardinality;
    int dynamic;
} UmiPluginExtensionPoint;

typedef struct UmiPluginExtensionPointRegistry UmiPluginExtensionPointRegistry;

UmiStatus umi_plugin_extension_point_registry_create(
    UmiPluginExtensionPointRegistry **out_registry);
void umi_plugin_extension_point_registry_destroy(
    UmiPluginExtensionPointRegistry *registry);
UmiStatus umi_plugin_extension_point_registry_add(
    UmiPluginExtensionPointRegistry *registry,
    const UmiPluginExtensionPoint *point);
UmiStatus umi_plugin_extension_point_registry_get(
    const UmiPluginExtensionPointRegistry *registry,
    const char *extension_id,
    UmiPluginExtensionPoint *out_point);
UmiStatus umi_plugin_extension_point_validate_contribution(
    const UmiPluginExtensionPointRegistry *registry,
    const UmiPluginContributionRegistry *contributions,
    const UmiPluginContribution *candidate,
    char *out_reason,
    size_t reason_capacity);
size_t umi_plugin_extension_point_registry_count(
    const UmiPluginExtensionPointRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
