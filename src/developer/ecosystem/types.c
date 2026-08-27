/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer/ecosystem/types.c
 * PURPOSE: Implement shared developer-ecosystem type and text helpers.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer/ecosystem/types.h"

#include <ctype.h>
#include <string.h>

UmiStatus umi_ecosystem_copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;
    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

bool umi_ecosystem_text_contains(const char *text, const char *query)
{
    size_t query_length;
    size_t index;
    if (text == NULL || query == NULL) return false;
    query_length = strlen(query);
    if (query_length == 0U) return true;
    for (index = 0U; text[index] != '\0'; ++index) {
        size_t offset = 0U;
        while (offset < query_length && text[index + offset] != '\0' &&
               tolower((unsigned char)text[index + offset]) ==
               tolower((unsigned char)query[offset])) {
            ++offset;
        }
        if (offset == query_length) return true;
    }
    return false;
}

const char *umi_ecosystem_source_kind_text(UmiEcosystemSourceKind kind)
{
    switch (kind) {
        case UMI_ECOSYSTEM_SOURCE_REMOTE: return "remote";
        case UMI_ECOSYSTEM_SOURCE_LOCAL: return "local";
        case UMI_ECOSYSTEM_SOURCE_OFFLINE: return "offline";
        default: return "unknown";
    }
}

const char *umi_ecosystem_package_kind_text(UmiEcosystemPackageKind kind)
{
    switch (kind) {
        case UMI_ECOSYSTEM_PACKAGE_EXTENSION: return "extension";
        case UMI_ECOSYSTEM_PACKAGE_SDK: return "sdk";
        case UMI_ECOSYSTEM_PACKAGE_COMPONENT: return "component";
        case UMI_ECOSYSTEM_PACKAGE_TEMPLATE: return "template";
        case UMI_ECOSYSTEM_PACKAGE_TOOLCHAIN: return "toolchain";
        default: return "unknown";
    }
}

const char *umi_ecosystem_install_state_text(UmiEcosystemInstallState state)
{
    switch (state) {
        case UMI_ECOSYSTEM_AVAILABLE: return "available";
        case UMI_ECOSYSTEM_INSTALLED: return "installed";
        case UMI_ECOSYSTEM_DISABLED: return "disabled";
        case UMI_ECOSYSTEM_ACTIVE: return "active";
        case UMI_ECOSYSTEM_UPDATE_AVAILABLE: return "update-available";
        default: return "unknown";
    }
}

const char *umi_ecosystem_compatibility_text(UmiEcosystemCompatibility value)
{
    switch (value) {
        case UMI_ECOSYSTEM_COMPATIBILITY_UNKNOWN: return "unknown";
        case UMI_ECOSYSTEM_COMPATIBLE: return "compatible";
        case UMI_ECOSYSTEM_INCOMPATIBLE: return "incompatible";
        case UMI_ECOSYSTEM_REQUIRES_MIGRATION: return "requires-migration";
        default: return "unknown";
    }
}

const char *umi_ecosystem_evidence_state_text(UmiEcosystemEvidenceState state)
{
    switch (state) {
        case UMI_ECOSYSTEM_EVIDENCE_UNKNOWN: return "unknown";
        case UMI_ECOSYSTEM_EVIDENCE_VERIFIED: return "verified";
        case UMI_ECOSYSTEM_EVIDENCE_WARNING: return "warning";
        case UMI_ECOSYSTEM_EVIDENCE_REJECTED: return "rejected";
        default: return "unknown";
    }
}

const char *umi_ecosystem_action_kind_text(UmiEcosystemActionKind kind)
{
    switch (kind) {
        case UMI_ECOSYSTEM_ACTION_DOWNLOAD: return "download";
        case UMI_ECOSYSTEM_ACTION_VERIFY: return "verify";
        case UMI_ECOSYSTEM_ACTION_RESOLVE_DEPENDENCIES: return "resolve-dependencies";
        case UMI_ECOSYSTEM_ACTION_REVIEW_PERMISSIONS: return "review-permissions";
        case UMI_ECOSYSTEM_ACTION_INSTALL: return "install";
        case UMI_ECOSYSTEM_ACTION_UPDATE: return "update";
        case UMI_ECOSYSTEM_ACTION_ACTIVATE: return "activate";
        case UMI_ECOSYSTEM_ACTION_MIGRATE: return "migrate";
        case UMI_ECOSYSTEM_ACTION_RESTART: return "restart";
        case UMI_ECOSYSTEM_ACTION_ROLLBACK: return "rollback";
        default: return "unknown";
    }
}

const char *umi_ecosystem_migration_kind_text(UmiEcosystemMigrationKind kind)
{
    switch (kind) {
        case UMI_ECOSYSTEM_MIGRATION_MANIFEST: return "manifest";
        case UMI_ECOSYSTEM_MIGRATION_PROJECT: return "project";
        case UMI_ECOSYSTEM_MIGRATION_SETTINGS: return "settings";
        case UMI_ECOSYSTEM_MIGRATION_COMPONENT: return "component";
        case UMI_ECOSYSTEM_MIGRATION_CACHE: return "cache";
        default: return "unknown";
    }
}
