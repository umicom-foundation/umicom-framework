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

/**
 * List the named plugin extension cardinality values accepted by this public contract.
 */
typedef enum UmiPluginExtensionCardinality {
    UMI_PLUGIN_EXTENSION_ZERO_OR_ONE = 0,
    UMI_PLUGIN_EXTENSION_ONE = 1,
    UMI_PLUGIN_EXTENSION_MANY = 2
} UmiPluginExtensionCardinality;

/**
 * Represent the plugin extension point data shared with callers of this public contract.
 */
typedef struct UmiPluginExtensionPoint {
    char extension_id[UMI_PLUGIN_CONTRIBUTION_TYPE_CAPACITY];
    char owner_id[UMI_PLUGIN_ID_CAPACITY];
    char schema[UMI_PLUGIN_EXTENSION_POINT_SCHEMA_CAPACITY];
    UmiPluginExtensionCardinality cardinality;
    int dynamic;
} UmiPluginExtensionPoint;

/**
 * Represent the plugin extension point registry data shared with callers of this public
 * contract.
 */
typedef struct UmiPluginExtensionPointRegistry UmiPluginExtensionPointRegistry;

/**
 * Initialise plugin extension point registry from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_plugin_extension_point_registry_create(
    UmiPluginExtensionPointRegistry **out_registry);
/**
 * Release or reset state held by plugin extension point registry so the same storage can
 * be reused safely.
 */
void umi_plugin_extension_point_registry_destroy(
    UmiPluginExtensionPointRegistry *registry);
/**
 * Add plugin extension point registry only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_plugin_extension_point_registry_add(
    UmiPluginExtensionPointRegistry *registry,
    const UmiPluginExtensionPoint *point);
/**
 * Provide the plugin extension point registry get operation used by this module and its
 * client applications.
 */
UmiStatus umi_plugin_extension_point_registry_get(
    const UmiPluginExtensionPointRegistry *registry,
    const char *extension_id,
    UmiPluginExtensionPoint *out_point);
/**
 * Provide the plugin extension point validate contribution operation used by this module
 * and its client applications.
 */
UmiStatus umi_plugin_extension_point_validate_contribution(
    const UmiPluginExtensionPointRegistry *registry,
    const UmiPluginContributionRegistry *contributions,
    const UmiPluginContribution *candidate,
    char *out_reason,
    size_t reason_capacity);
/**
 * Return the number of records represented by plugin extension point registry without
 * changing their state.
 */
size_t umi_plugin_extension_point_registry_count(
    const UmiPluginExtensionPointRegistry *registry);

#ifdef __cplusplus
}
#endif

#endif
