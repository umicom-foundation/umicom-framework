/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/resource_catalogue.h
 *
 * PURPOSE:
 *   Define the Framework-owned catalogue for shared branding, icons, themes,
 *   schemas and other immutable application resources. Applications request a
 *   stable logical identifier; they never hard-code repository-relative paths.
 *
 * OWNERSHIP AND BOUNDARIES:
 *   - Umicom Framework owns resources meaningful to more than one product.
 *   - Product modules retain product-specific artwork and default content.
 *   - Umicom OS retains boot, installer, recovery and pre-Framework artwork.
 *   - The catalogue stores immutable descriptors. Callers own output buffers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_RESOURCE_CATALOGUE_H
#define UMICOM_APPLICATION_RESOURCE_CATALOGUE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_RESOURCE_ID_CAPACITY 128U
#define UMI_APPLICATION_RESOURCE_LOCATOR_CAPACITY 512U
#define UMI_APPLICATION_RESOURCE_MEDIA_TYPE_CAPACITY 96U

typedef enum UmiApplicationResourceKind {
    UMI_APPLICATION_RESOURCE_FILE = 1,
    UMI_APPLICATION_RESOURCE_THEME_ICON = 2,
    UMI_APPLICATION_RESOURCE_THEME_TOKENS = 3,
    UMI_APPLICATION_RESOURCE_WINDOWS_TEMPLATE = 4,
    UMI_APPLICATION_RESOURCE_SCHEMA = 5,
    UMI_APPLICATION_RESOURCE_LAYOUT_TEMPLATE = 6,
    UMI_APPLICATION_RESOURCE_TRANSLATION = 7
} UmiApplicationResourceKind;

typedef enum UmiApplicationResourceScope {
    UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK = 1,
    UMI_APPLICATION_RESOURCE_SCOPE_PRODUCT = 2,
    UMI_APPLICATION_RESOURCE_SCOPE_OPERATING_SYSTEM = 3
} UmiApplicationResourceScope;

typedef enum UmiApplicationResourceFlags {
    UMI_APPLICATION_RESOURCE_INSTALLABLE = 1U << 0,
    UMI_APPLICATION_RESOURCE_SHARED = 1U << 1,
    UMI_APPLICATION_RESOURCE_APPLICATION_ICON = 1U << 2,
    UMI_APPLICATION_RESOURCE_TRADEMARK = 1U << 3,
    UMI_APPLICATION_RESOURCE_REQUIRED = 1U << 4,
    UMI_APPLICATION_RESOURCE_THEME_DEPENDENT = 1U << 5
} UmiApplicationResourceFlags;

typedef struct UmiApplicationResourceDescriptor {
    uint32_t structure_size;
    const char *resource_id;
    const char *locator;
    const char *media_type;
    UmiApplicationResourceKind kind;
    UmiApplicationResourceScope scope;
    uint32_t flags;
} UmiApplicationResourceDescriptor;

typedef struct UmiApplicationResourceLocation {
    char resource_id[UMI_APPLICATION_RESOURCE_ID_CAPACITY];
    char locator[UMI_APPLICATION_RESOURCE_LOCATOR_CAPACITY];
    char media_type[UMI_APPLICATION_RESOURCE_MEDIA_TYPE_CAPACITY];
    UmiApplicationResourceKind kind;
    UmiApplicationResourceScope scope;
    uint32_t flags;
    bool available;
} UmiApplicationResourceLocation;

size_t umi_application_resource_catalogue_count(void);
const UmiApplicationResourceDescriptor *umi_application_resource_catalogue_at(
    size_t index);
const UmiApplicationResourceDescriptor *umi_application_resource_catalogue_find(
    const char *resource_id);
UmiStatus umi_application_resource_descriptor_validate(
    const UmiApplicationResourceDescriptor *descriptor);
UmiStatus umi_application_resource_catalogue_validate(void);

/*
 * Resolve a logical resource against a caller-selected resource root. File
 * resources return an absolute or rooted path. Theme icons return the frontend
 * icon name directly and do not require a filesystem root.
 */
UmiStatus umi_application_resource_resolve(
    const char *resource_root,
    const char *resource_id,
    UmiApplicationResourceLocation *out_location);

/*
 * A composition root may publish a build-tree default using
 * UMICOM_FRAMEWORK_RESOURCE_ROOT_DEFAULT. Deployments may override it through
 * UMICOM_FRAMEWORK_RESOURCE_ROOT; standalone consumers may instead pass an
 * installed package root explicitly to umi_application_resource_resolve().
 */
const char *umi_application_resource_default_root(void);

const char *umi_application_resource_kind_text(
    UmiApplicationResourceKind kind);
const char *umi_application_resource_scope_text(
    UmiApplicationResourceScope scope);

#ifdef __cplusplus
}
#endif

#endif
