/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/resource_broker.c
 *
 * PURPOSE:
 *   Implement bounded Framework-owned resource leasing, immutable shared
 *   resource discovery and application presentation metadata.
 *
 * ARCHITECTURAL BOUNDARY:
 *   Runtime leases protect scarce resources such as devices and providers.
 *   The static catalogue resolves common branding, icons, themes and schemas.
 *   Applications consume logical identifiers rather than exchanging handles or
 *   embedding repository-relative filesystem paths.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/resource_broker.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "umicom/application/presentation.h"
#include "umicom/application/resource_catalogue.h"
#include "umicom/runtime/capability_catalogue.h"

#ifndef UMICOM_FRAMEWORK_RESOURCE_ROOT_DEFAULT
#define UMICOM_FRAMEWORK_RESOURCE_ROOT_DEFAULT ""
#endif

#define COUNT_OF(values) (sizeof(values) / sizeof((values)[0]))

struct UmiApplicationResourceBroker {
    UmiResourceLease leases[UMI_RESOURCE_BROKER_MAX_LEASES];
    uint64_t next_id;
    uint64_t revision;
};

/*
 * This catalogue intentionally contains identifiers, media information and
 * relative locators only. It does not own mutable application state and never
 * exposes a GTK, Qt, Wt, Windows or Linux object through the stable C ABI.
 */
static const UmiApplicationResourceDescriptor STATIC_RESOURCES[] = {
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.brand.logo.primary", "brand/umicom-logo.svg", "image/svg+xml",
     UMI_APPLICATION_RESOURCE_FILE, UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED |
         UMI_APPLICATION_RESOURCE_TRADEMARK | UMI_APPLICATION_RESOURCE_REQUIRED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.brand.logo.on-dark", "brand/umicom-logo-on-dark.svg",
     "image/svg+xml", UMI_APPLICATION_RESOURCE_FILE,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED |
         UMI_APPLICATION_RESOURCE_TRADEMARK | UMI_APPLICATION_RESOURCE_REQUIRED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.brand.logo.raster", "brand/umicom-logo.png", "image/png",
     UMI_APPLICATION_RESOURCE_FILE, UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED |
         UMI_APPLICATION_RESOURCE_TRADEMARK | UMI_APPLICATION_RESOURCE_DERIVED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.brand.icon.primary", "brand/umicom-icon.svg", "image/svg+xml",
     UMI_APPLICATION_RESOURCE_FILE, UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED |
         UMI_APPLICATION_RESOURCE_TRADEMARK | UMI_APPLICATION_RESOURCE_REQUIRED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.brand.icon.on-dark", "brand/umicom-icon-on-dark.svg",
     "image/svg+xml", UMI_APPLICATION_RESOURCE_FILE,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED |
         UMI_APPLICATION_RESOURCE_TRADEMARK | UMI_APPLICATION_RESOURCE_REQUIRED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.brand.icon.raster", "brand/umicom-icon.png", "image/png",
     UMI_APPLICATION_RESOURCE_FILE, UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED |
         UMI_APPLICATION_RESOURCE_TRADEMARK | UMI_APPLICATION_RESOURCE_DERIVED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.brand.icon.windows", "brand/umicom.ico", "image/vnd.microsoft.icon",
     UMI_APPLICATION_RESOURCE_FILE, UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED |
         UMI_APPLICATION_RESOURCE_TRADEMARK | UMI_APPLICATION_RESOURCE_DERIVED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.windows.application-resource-template",
     "windows/umicom-application.rc.in", "text/plain",
     UMI_APPLICATION_RESOURCE_WINDOWS_TEMPLATE,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED |
         UMI_APPLICATION_RESOURCE_REQUIRED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.linux.application-desktop-template",
     "linux/umicom-application.desktop.in", "text/plain",
     UMI_APPLICATION_RESOURCE_FILE,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED |
         UMI_APPLICATION_RESOURCE_REQUIRED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.theme.dark.tokens", "themes/umicom-dark.tokens.json",
     "application/json", UMI_APPLICATION_RESOURCE_THEME_TOKENS,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.theme.light.tokens", "themes/umicom-light.tokens.json",
     "application/json", UMI_APPLICATION_RESOURCE_THEME_TOKENS,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.theme.high-contrast.tokens",
     "themes/umicom-high-contrast.tokens.json", "application/json",
     UMI_APPLICATION_RESOURCE_THEME_TOKENS,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.schema.resource-catalogue",
     "schemas/resource-catalogue.schema.json", "application/schema+json",
     UMI_APPLICATION_RESOURCE_SCHEMA, UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.schema.application-presentation",
     "schemas/application-presentation.schema.json", "application/schema+json",
     UMI_APPLICATION_RESOURCE_SCHEMA, UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.application.presentations", "application-presentations.json",
     "application/json", UMI_APPLICATION_RESOURCE_FILE,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.schema.layout", "schemas/layout.schema.json",
     "application/schema+json", UMI_APPLICATION_RESOURCE_SCHEMA,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.layout.blank", "layouts/templates/blank.umilayout",
     "application/vnd.umicom.layout+json",
     UMI_APPLICATION_RESOURCE_LAYOUT_TEMPLATE,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.layout.mosaic", "layouts/templates/mosaic.umilayout",
     "application/vnd.umicom.layout+json",
     UMI_APPLICATION_RESOURCE_LAYOUT_TEMPLATE,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.layout.standard-workbench",
     "layouts/templates/standard-workbench.umilayout",
     "application/vnd.umicom.layout+json",
     UMI_APPLICATION_RESOURCE_LAYOUT_TEMPLATE,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_INSTALLABLE | UMI_APPLICATION_RESOURCE_SHARED},

    /* Frontend theme-icon names remain logical Framework resources. */
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.icon.application.generic", "application-x-executable-symbolic",
     "image/x-icon-name", UMI_APPLICATION_RESOURCE_THEME_ICON,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_SHARED | UMI_APPLICATION_RESOURCE_APPLICATION_ICON |
         UMI_APPLICATION_RESOURCE_THEME_DEPENDENT},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.icon.application.studio", "applications-development-symbolic",
     "image/x-icon-name", UMI_APPLICATION_RESOURCE_THEME_ICON,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_SHARED | UMI_APPLICATION_RESOURCE_APPLICATION_ICON |
         UMI_APPLICATION_RESOURCE_THEME_DEPENDENT},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.icon.application.trader", "view-statistics-symbolic",
     "image/x-icon-name", UMI_APPLICATION_RESOURCE_THEME_ICON,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_SHARED | UMI_APPLICATION_RESOURCE_APPLICATION_ICON |
         UMI_APPLICATION_RESOURCE_THEME_DEPENDENT},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.icon.application.tms", "x-office-spreadsheet-symbolic",
     "image/x-icon-name", UMI_APPLICATION_RESOURCE_THEME_ICON,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_SHARED | UMI_APPLICATION_RESOURCE_APPLICATION_ICON |
         UMI_APPLICATION_RESOURCE_THEME_DEPENDENT},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.icon.application.llm", "applications-science-symbolic",
     "image/x-icon-name", UMI_APPLICATION_RESOURCE_THEME_ICON,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_SHARED | UMI_APPLICATION_RESOURCE_APPLICATION_ICON |
         UMI_APPLICATION_RESOURCE_THEME_DEPENDENT},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.icon.application.bank", "folder-documents-symbolic",
     "image/x-icon-name", UMI_APPLICATION_RESOURCE_THEME_ICON,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_SHARED | UMI_APPLICATION_RESOURCE_APPLICATION_ICON |
         UMI_APPLICATION_RESOURCE_THEME_DEPENDENT},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.icon.application.exchange", "network-workgroup-symbolic",
     "image/x-icon-name", UMI_APPLICATION_RESOURCE_THEME_ICON,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_SHARED | UMI_APPLICATION_RESOURCE_APPLICATION_ICON |
         UMI_APPLICATION_RESOURCE_THEME_DEPENDENT},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.icon.application.desktop", "user-desktop-symbolic",
     "image/x-icon-name", UMI_APPLICATION_RESOURCE_THEME_ICON,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_SHARED | UMI_APPLICATION_RESOURCE_APPLICATION_ICON |
         UMI_APPLICATION_RESOURCE_THEME_DEPENDENT},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.icon.application.os", "preferences-system-symbolic",
     "image/x-icon-name", UMI_APPLICATION_RESOURCE_THEME_ICON,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_SHARED | UMI_APPLICATION_RESOURCE_APPLICATION_ICON |
         UMI_APPLICATION_RESOURCE_THEME_DEPENDENT},
    {sizeof(UmiApplicationResourceDescriptor),
     "umicom.icon.application.marketplace", "system-software-install-symbolic",
     "image/x-icon-name", UMI_APPLICATION_RESOURCE_THEME_ICON,
     UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK,
     UMI_APPLICATION_RESOURCE_SHARED | UMI_APPLICATION_RESOURCE_APPLICATION_ICON |
         UMI_APPLICATION_RESOURCE_THEME_DEPENDENT},
};

static const UmiApplicationPresentation APPLICATION_PRESENTATIONS[] = {
    {sizeof(UmiApplicationPresentation), "org.umicom.studio",
     "umicom.icon.application.studio", "develop", "development",
     UMI_APPLICATION_ENTRY_WORKBENCH, true, false},
    {sizeof(UmiApplicationPresentation), "org.umicom.ide",
     "umicom.icon.application.studio", "develop", "development",
     UMI_APPLICATION_ENTRY_WORKBENCH, false, false},
    {sizeof(UmiApplicationPresentation), "org.umicom.trader",
     "umicom.icon.application.trader", "trading", "finance",
     UMI_APPLICATION_ENTRY_WORKBENCH, false, false},
    {sizeof(UmiApplicationPresentation), "org.umicom.tms",
     "umicom.icon.application.tms", "treasury", "finance",
     UMI_APPLICATION_ENTRY_WORKBENCH, false, false},
    {sizeof(UmiApplicationPresentation), "org.umicom.llm",
     "umicom.icon.application.llm", "ai", "artificial-intelligence",
     UMI_APPLICATION_ENTRY_WORKBENCH, false, false},
    {sizeof(UmiApplicationPresentation), "org.umicom.rag",
     "umicom.icon.application.llm", "rag", "artificial-intelligence",
     UMI_APPLICATION_ENTRY_WORKBENCH, false, false},
    {sizeof(UmiApplicationPresentation), "org.umicom.bank",
     "umicom.icon.application.bank", "banking", "finance",
     UMI_APPLICATION_ENTRY_WORKBENCH, false, false},
    {sizeof(UmiApplicationPresentation), "org.umicom.exchange",
     "umicom.icon.application.exchange", "exchange", "finance",
     UMI_APPLICATION_ENTRY_WORKBENCH, false, false},
    {sizeof(UmiApplicationPresentation), "org.umicom.marketplace",
     "umicom.icon.application.marketplace", "applications", "platform",
     UMI_APPLICATION_ENTRY_UTILITY, true, false},
    {sizeof(UmiApplicationPresentation), "org.umicom.operations",
     "umicom.icon.application.os", "operations", "platform",
     UMI_APPLICATION_ENTRY_UTILITY, false, false},
    {sizeof(UmiApplicationPresentation), "org.umicom.security-centre",
     "umicom.icon.application.os", "security", "platform",
     UMI_APPLICATION_ENTRY_UTILITY, false, false},
    {sizeof(UmiApplicationPresentation), "org.umicom.os",
     "umicom.icon.application.os", "system", "system",
     UMI_APPLICATION_ENTRY_SYSTEM, true, false},
    {sizeof(UmiApplicationPresentation), "org.umicom.desktop",
     "umicom.icon.application.desktop", "mosaic", "platform",
     UMI_APPLICATION_ENTRY_SYSTEM, true, false},
};

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *destination, size_t capacity,
                           const char *source)
{
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || source == NULL ||
        source[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    length = strlen(source);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

/* Provide the is non empty operation used by this module and its client applications. */
static int is_non_empty(const char *value)
{
    return value != NULL && value[0] != '\0';
}

/* Provide the locator is file operation used by this module and its client applications. */
static int locator_is_file(const char *path)
{
    FILE *stream;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!is_non_empty(path)) return 0;
    stream = fopen(path, "rb");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (stream == NULL) return 0;
    (void)fclose(stream);
    return 1;
}

/* Provide the lease conflicts operation used by this module and its client applications. */
static int lease_conflicts(const UmiResourceLease *lease,
                           const UmiResourceLeaseRequest *request)
{
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!lease->active || strcmp(lease->resource_id, request->resource_id) != 0)
        return 0;
    return lease->mode == UMI_RESOURCE_LEASE_EXCLUSIVE ||
           request->mode == UMI_RESOURCE_LEASE_EXCLUSIVE;
}

/*
 * Return the number of records represented by application resource catalogue without
 * changing their state.
 */
size_t umi_application_resource_catalogue_count(void)
{
    return COUNT_OF(STATIC_RESOURCES);
}

/*
 * Find application resource catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationResourceDescriptor *umi_application_resource_catalogue_at(
    size_t index)
{
    return index < COUNT_OF(STATIC_RESOURCES) ? &STATIC_RESOURCES[index] : NULL;
}

/*
 * Find application resource catalogue while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationResourceDescriptor *umi_application_resource_catalogue_find(
    const char *resource_id)
{
    size_t index;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!is_non_empty(resource_id)) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < COUNT_OF(STATIC_RESOURCES); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(STATIC_RESOURCES[index].resource_id, resource_id) == 0)
            return &STATIC_RESOURCES[index];
    }
    return NULL;
}

/*
 * Check that application resource descriptor satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_resource_descriptor_validate(
    const UmiApplicationResourceDescriptor *descriptor)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL ||
        descriptor->structure_size < sizeof(UmiApplicationResourceDescriptor) ||
        !is_non_empty(descriptor->resource_id) ||
        !is_non_empty(descriptor->locator) ||
        !is_non_empty(descriptor->media_type) ||
        descriptor->kind < UMI_APPLICATION_RESOURCE_FILE ||
        descriptor->kind > UMI_APPLICATION_RESOURCE_TRANSLATION ||
        descriptor->scope < UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK ||
        descriptor->scope > UMI_APPLICATION_RESOURCE_SCOPE_OPERATING_SYSTEM)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/*
 * Check that application resource catalogue satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_resource_catalogue_validate(void)
{
    size_t first;
    size_t second;
    /* Visit each bounded item once so every record receives the same rule. */
    for (first = 0U; first < COUNT_OF(STATIC_RESOURCES); ++first) {
        /* Apply this operation only while the related capability or state is available. */
        if (umi_application_resource_descriptor_validate(
                &STATIC_RESOURCES[first]) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_ARGUMENT;
        /* Visit each bounded item once so every record receives the same rule. */
        for (second = first + 1U; second < COUNT_OF(STATIC_RESOURCES); ++second) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(STATIC_RESOURCES[first].resource_id,
                       STATIC_RESOURCES[second].resource_id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the application resource resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_resource_resolve(
    const char *resource_root,
    const char *resource_id,
    UmiApplicationResourceLocation *out_location)
{
    const UmiApplicationResourceDescriptor *descriptor;
    const char *effective_root;
    int written;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (!is_non_empty(resource_id) || out_location == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    descriptor = umi_application_resource_catalogue_find(resource_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (descriptor == NULL) return UMI_STATUS_NOT_FOUND;
    (void)memset(out_location, 0, sizeof(*out_location));
    status = copy_text(out_location->resource_id,
                       sizeof(out_location->resource_id), resource_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = copy_text(out_location->media_type,
                       sizeof(out_location->media_type), descriptor->media_type);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_location->kind = descriptor->kind;
    out_location->scope = descriptor->scope;
    out_location->flags = descriptor->flags;

    /* Apply this branch only when its contract condition is satisfied. */
    if (descriptor->kind == UMI_APPLICATION_RESOURCE_THEME_ICON) {
        status = copy_text(out_location->locator,
                           sizeof(out_location->locator), descriptor->locator);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status == UMI_STATUS_OK) out_location->available = true;
        return status;
    }

    effective_root = is_non_empty(resource_root)
        ? resource_root
        : umi_application_resource_default_root();
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!is_non_empty(effective_root)) return UMI_STATUS_INVALID_STATE;
    written = snprintf(out_location->locator, sizeof(out_location->locator),
                       "%s/%s", effective_root, descriptor->locator);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(out_location->locator))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    out_location->available = locator_is_file(out_location->locator) != 0;
    return UMI_STATUS_OK;
}

/*
 * Provide the application resource default root operation used by this module and its
 * client applications.
 */
const char *umi_application_resource_default_root(void)
{
    const char *environment_root = getenv("UMICOM_FRAMEWORK_RESOURCE_ROOT");
    return is_non_empty(environment_root)
        ? environment_root
        : UMICOM_FRAMEWORK_RESOURCE_ROOT_DEFAULT;
}

/*
 * Provide the application resource kind text operation used by this module and its client
 * applications.
 */
const char *umi_application_resource_kind_text(
    UmiApplicationResourceKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
    case UMI_APPLICATION_RESOURCE_FILE: return "file";
    case UMI_APPLICATION_RESOURCE_THEME_ICON: return "theme-icon";
    case UMI_APPLICATION_RESOURCE_THEME_TOKENS: return "theme-tokens";
    case UMI_APPLICATION_RESOURCE_WINDOWS_TEMPLATE: return "windows-template";
    case UMI_APPLICATION_RESOURCE_SCHEMA: return "schema";
    case UMI_APPLICATION_RESOURCE_LAYOUT_TEMPLATE: return "layout-template";
    case UMI_APPLICATION_RESOURCE_TRANSLATION: return "translation";
    default: return "unknown";
    }
}

/*
 * Provide the application resource scope text operation used by this module and its client
 * applications.
 */
const char *umi_application_resource_scope_text(
    UmiApplicationResourceScope scope)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (scope) {
    case UMI_APPLICATION_RESOURCE_SCOPE_FRAMEWORK: return "framework";
    case UMI_APPLICATION_RESOURCE_SCOPE_PRODUCT: return "product";
    case UMI_APPLICATION_RESOURCE_SCOPE_OPERATING_SYSTEM: return "operating-system";
    default: return "unknown";
    }
}

/*
 * Return the number of records represented by application presentation without changing
 * their state.
 */
size_t umi_application_presentation_count(void)
{
    return COUNT_OF(APPLICATION_PRESENTATIONS);
}

/*
 * Find application presentation while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiApplicationPresentation *umi_application_presentation_at(size_t index)
{
    return index < COUNT_OF(APPLICATION_PRESENTATIONS)
        ? &APPLICATION_PRESENTATIONS[index]
        : NULL;
}

/*
 * Find application presentation while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiApplicationPresentation *umi_application_presentation_find(
    const char *application_id)
{
    size_t index;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!is_non_empty(application_id)) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < COUNT_OF(APPLICATION_PRESENTATIONS); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(APPLICATION_PRESENTATIONS[index].application_id,
                   application_id) == 0)
            return &APPLICATION_PRESENTATIONS[index];
    }
    return NULL;
}

/*
 * Check that application presentation satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_application_presentation_validate(
    const UmiApplicationPresentation *presentation)
{
    const UmiApplicationResourceDescriptor *icon;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (presentation == NULL ||
        presentation->structure_size < sizeof(UmiApplicationPresentation) ||
        !is_non_empty(presentation->application_id) ||
        !is_non_empty(presentation->icon_resource_id) ||
        !is_non_empty(presentation->default_layout_id) ||
        !is_non_empty(presentation->taskbar_group) ||
        presentation->entry_kind < UMI_APPLICATION_ENTRY_WORKBENCH ||
        presentation->entry_kind > UMI_APPLICATION_ENTRY_SERVICE)
        return UMI_STATUS_INVALID_ARGUMENT;
    icon = umi_application_resource_catalogue_find(
        presentation->icon_resource_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (icon == NULL || icon->kind != UMI_APPLICATION_RESOURCE_THEME_ICON ||
        (icon->flags & UMI_APPLICATION_RESOURCE_APPLICATION_ICON) == 0U)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}

/*
 * Check that application presentation catalogue satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_application_presentation_catalogue_validate(void)
{
    size_t first;
    size_t second;
    /* Visit each bounded item once so every record receives the same rule. */
    for (first = 0U; first < COUNT_OF(APPLICATION_PRESENTATIONS); ++first) {
        /* Apply this operation only while the related capability or state is available. */
        if (umi_application_presentation_validate(
                &APPLICATION_PRESENTATIONS[first]) != UMI_STATUS_OK)
            return UMI_STATUS_INVALID_ARGUMENT;
        /* Visit each bounded item once so every record receives the same rule. */
        for (second = first + 1U;
             second < COUNT_OF(APPLICATION_PRESENTATIONS);
             ++second) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(APPLICATION_PRESENTATIONS[first].application_id,
                       APPLICATION_PRESENTATIONS[second].application_id) == 0)
                return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the application entry kind text operation used by this module and its client
 * applications.
 */
const char *umi_application_entry_kind_text(UmiApplicationEntryKind kind)
{
    /* Select the behaviour associated with the requested command or state value. */
    switch (kind) {
    case UMI_APPLICATION_ENTRY_WORKBENCH: return "workbench";
    case UMI_APPLICATION_ENTRY_UTILITY: return "utility";
    case UMI_APPLICATION_ENTRY_SYSTEM: return "system";
    case UMI_APPLICATION_ENTRY_SERVICE: return "service";
    default: return "unknown";
    }
}

/*
 * Initialise application resource broker from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_resource_broker_create(
    UmiApplicationResourceBroker **out_broker)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_broker == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_broker = (UmiApplicationResourceBroker *)calloc(1U,
                                                         sizeof(**out_broker));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (*out_broker == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    (*out_broker)->next_id = 1U;
    (*out_broker)->revision = 1U;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by application resource broker so the same storage can be
 * reused safely.
 */
void umi_application_resource_broker_destroy(
    UmiApplicationResourceBroker *broker)
{
    free(broker);
}

/*
 * Provide the application resource broker acquire operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_resource_broker_acquire(
    UmiApplicationResourceBroker *broker,
    const UmiResourceLeaseRequest *request,
    UmiResourceLease *out_lease)
{
    size_t index;
    UmiResourceLease *slot = NULL;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (broker == NULL || request == NULL || out_lease == NULL ||
        umi_application_definition_validate(request->application) !=
            UMI_STATUS_OK ||
        request->resource_id == NULL || request->capability_id == NULL ||
        request->mode < UMI_RESOURCE_LEASE_SHARED_READ ||
        request->mode > UMI_RESOURCE_LEASE_EXCLUSIVE ||
        request->quota_units == 0U ||
        umi_framework_capability_catalogue_find(request->capability_id) == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_application_definition_declares_capability(
            request->application, request->capability_id))
        return UMI_STATUS_PERMISSION_DENIED;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_RESOURCE_BROKER_MAX_LEASES; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (lease_conflicts(&broker->leases[index], request))
            return UMI_STATUS_BUSY;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (!broker->leases[index].active && slot == NULL)
            slot = &broker->leases[index];
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (slot == NULL) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(slot, 0, sizeof(*slot));
    (void)snprintf(slot->lease_id, sizeof(slot->lease_id), "lease-%llu",
                   (unsigned long long)broker->next_id++);
    status = copy_text(slot->application_id, sizeof(slot->application_id),
                       request->application->application_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(slot->resource_id, sizeof(slot->resource_id),
                           request->resource_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK)
        status = copy_text(slot->capability_id, sizeof(slot->capability_id),
                           request->capability_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        (void)memset(slot, 0, sizeof(*slot));
        return status;
    }
    slot->mode = request->mode;
    slot->quota_units = request->quota_units;
    slot->expires_at = request->expires_at;
    slot->revision = ++broker->revision;
    slot->active = 1;
    *out_lease = *slot;
    return UMI_STATUS_OK;
}

/*
 * Provide the application resource broker renew operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_resource_broker_renew(
    UmiApplicationResourceBroker *broker,
    const char *application_id,
    const char *lease_id,
    uint64_t expires_at,
    UmiResourceLease *out_lease)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (broker == NULL || application_id == NULL || lease_id == NULL ||
        out_lease == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_RESOURCE_BROKER_MAX_LEASES; ++index) {
        UmiResourceLease *lease = &broker->leases[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (lease->active && strcmp(lease->lease_id, lease_id) == 0) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(lease->application_id, application_id) != 0)
                return UMI_STATUS_PERMISSION_DENIED;
            lease->expires_at = expires_at;
            lease->revision = ++broker->revision;
            *out_lease = *lease;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Release or reset state held by application resource broker so the same storage can be
 * reused safely.
 */
UmiStatus umi_application_resource_broker_release(
    UmiApplicationResourceBroker *broker,
    const char *application_id,
    const char *lease_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (broker == NULL || application_id == NULL || lease_id == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_RESOURCE_BROKER_MAX_LEASES; ++index) {
        UmiResourceLease *lease = &broker->leases[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (lease->active && strcmp(lease->lease_id, lease_id) == 0) {
            /* Use the stable identifier comparison to choose the matching record or policy. */
            if (strcmp(lease->application_id, application_id) != 0)
                return UMI_STATUS_PERMISSION_DENIED;
            lease->active = 0;
            lease->revision = ++broker->revision;
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Find application resource broker while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiStatus umi_application_resource_broker_find(
    const UmiApplicationResourceBroker *broker,
    const char *lease_id,
    UmiResourceLease *out_lease)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (broker == NULL || lease_id == NULL || out_lease == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_RESOURCE_BROKER_MAX_LEASES; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (broker->leases[index].active &&
            strcmp(broker->leases[index].lease_id, lease_id) == 0) {
            *out_lease = broker->leases[index];
            return UMI_STATUS_OK;
        }
    }
    return UMI_STATUS_NOT_FOUND;
}

/*
 * Return the number of records represented by application resource broker active without
 * changing their state.
 */
size_t umi_application_resource_broker_active_count(
    const UmiApplicationResourceBroker *broker)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (broker == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < UMI_RESOURCE_BROKER_MAX_LEASES; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (broker->leases[index].active) count += 1U;
    }
    return count;
}
