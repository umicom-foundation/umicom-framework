/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience.h
 *
 * PURPOSE:
 *   Define the Framework-owned, toolkit-neutral product experience contract used by all thin Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_EXPERIENCE_H
#define UMICOM_APPLICATION_EXPERIENCE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_EXPERIENCE_API_VERSION 1U
#define UMI_APPLICATION_EXPERIENCE_REGISTRY_CAPACITY 64U

typedef enum UmiExperienceFeatureState {
    UMI_EXPERIENCE_FEATURE_PLANNED = 0,
    UMI_EXPERIENCE_FEATURE_FOUNDATION = 1,
    UMI_EXPERIENCE_FEATURE_IMPLEMENTED = 2,
    UMI_EXPERIENCE_FEATURE_VERIFIED = 3
} UmiExperienceFeatureState;

typedef enum UmiExperiencePriority {
    UMI_EXPERIENCE_PRIORITY_P0 = 0,
    UMI_EXPERIENCE_PRIORITY_P1 = 1,
    UMI_EXPERIENCE_PRIORITY_P2 = 2,
    UMI_EXPERIENCE_PRIORITY_P3 = 3,
    UMI_EXPERIENCE_PRIORITY_P4 = 4
} UmiExperiencePriority;

typedef enum UmiExperienceOwnership {
    UMI_EXPERIENCE_OWNER_FRAMEWORK = 0,
    UMI_EXPERIENCE_OWNER_APPLICATION = 1,
    UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER = 2
} UmiExperienceOwnership;

typedef enum UmiExperiencePanelFlags {
    UMI_EXPERIENCE_PANEL_DOCKABLE = 1U << 0,
    UMI_EXPERIENCE_PANEL_FLOATABLE = 1U << 1,
    UMI_EXPERIENCE_PANEL_AUTO_HIDE = 1U << 2,
    UMI_EXPERIENCE_PANEL_MULTI_MONITOR = 1U << 3,
    UMI_EXPERIENCE_PANEL_CONTEXT_LINKED = 1U << 4,
    UMI_EXPERIENCE_PANEL_READ_ONLY = 1U << 5,
    UMI_EXPERIENCE_PANEL_CRITICAL = 1U << 6
} UmiExperiencePanelFlags;

typedef enum UmiExperienceLayoutFlags {
    UMI_EXPERIENCE_LAYOUT_LOCKABLE = 1U << 0,
    UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR = 1U << 1,
    UMI_EXPERIENCE_LAYOUT_RESPONSIVE = 1U << 2,
    UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED = 1U << 3
} UmiExperienceLayoutFlags;

typedef struct UmiExperiencePanelDefinition {
    uint32_t structure_size;
    const char *panel_id;
    const char *title;
    const char *summary;
    const char *required_capability;
    const char *context_group_id;
    const char *default_region;
    uint32_t flags;
} UmiExperiencePanelDefinition;

typedef struct UmiExperienceLayoutDefinition {
    uint32_t structure_size;
    const char *layout_id;
    const char *title;
    const char *description;
    const char *const *panel_ids;
    size_t panel_count;
    uint32_t flags;
} UmiExperienceLayoutDefinition;

typedef struct UmiExperienceFeatureDefinition {
    uint32_t structure_size;
    const char *feature_id;
    const char *title;
    const char *summary;
    UmiExperienceFeatureState state;
    UmiExperiencePriority priority;
    UmiExperienceOwnership owner;
    const char *required_capability;
} UmiExperienceFeatureDefinition;

typedef struct UmiApplicationExperienceDefinition {
    uint32_t structure_size;
    uint32_t api_version;
    const char *application_id;
    const char *display_name;
    const char *default_layout_id;
    const UmiExperiencePanelDefinition *panels;
    size_t panel_count;
    const UmiExperienceLayoutDefinition *layouts;
    size_t layout_count;
    const UmiExperienceFeatureDefinition *features;
    size_t feature_count;
} UmiApplicationExperienceDefinition;

UmiStatus umi_application_experience_validate(
    const UmiApplicationExperienceDefinition *definition);

const UmiExperiencePanelDefinition *umi_application_experience_panel_find(
    const UmiApplicationExperienceDefinition *definition,
    const char *panel_id);

const UmiExperienceLayoutDefinition *umi_application_experience_layout_find(
    const UmiApplicationExperienceDefinition *definition,
    const char *layout_id);

const UmiExperienceFeatureDefinition *umi_application_experience_feature_find(
    const UmiApplicationExperienceDefinition *definition,
    const char *feature_id);

size_t umi_application_experience_feature_state_count(
    const UmiApplicationExperienceDefinition *definition,
    UmiExperienceFeatureState state);

unsigned umi_application_experience_readiness_percent(
    const UmiApplicationExperienceDefinition *definition);

const char *umi_experience_feature_state_text(UmiExperienceFeatureState state);
const char *umi_experience_priority_text(UmiExperiencePriority priority);
const char *umi_experience_ownership_text(UmiExperienceOwnership owner);

#ifdef __cplusplus
}
#endif

#endif
