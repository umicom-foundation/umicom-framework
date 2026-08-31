/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience.h
 *
 * PURPOSE:
 *   Define the Framework-owned, toolkit-neutral product experience contract
 *   used by all thin Umicom applications.
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

/* Feature states form a truthful progression. Foundation means usable contracts
 * exist, while verified means implementation and acceptance evidence exist. */
typedef enum UmiExperienceFeatureState {
    UMI_EXPERIENCE_FEATURE_PLANNED = 0,
    UMI_EXPERIENCE_FEATURE_FOUNDATION = 1,
    UMI_EXPERIENCE_FEATURE_IMPLEMENTED = 2,
    UMI_EXPERIENCE_FEATURE_VERIFIED = 3
} UmiExperienceFeatureState;

/* Priorities communicate delivery order; a lower number is more urgent. */
typedef enum UmiExperiencePriority {
    UMI_EXPERIENCE_PRIORITY_P0 = 0,
    UMI_EXPERIENCE_PRIORITY_P1 = 1,
    UMI_EXPERIENCE_PRIORITY_P2 = 2,
    UMI_EXPERIENCE_PRIORITY_P3 = 3,
    UMI_EXPERIENCE_PRIORITY_P4 = 4
} UmiExperiencePriority;

/* Ownership tells contributors where missing behaviour must be implemented. */
typedef enum UmiExperienceOwnership {
    UMI_EXPERIENCE_OWNER_FRAMEWORK = 0,
    UMI_EXPERIENCE_OWNER_APPLICATION = 1,
    UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER = 2
} UmiExperienceOwnership;

/* Panel flags describe optional workspace behaviours supported by the panel. */
typedef enum UmiExperiencePanelFlags {
    UMI_EXPERIENCE_PANEL_DOCKABLE = 1U << 0,
    UMI_EXPERIENCE_PANEL_FLOATABLE = 1U << 1,
    UMI_EXPERIENCE_PANEL_AUTO_HIDE = 1U << 2,
    UMI_EXPERIENCE_PANEL_MULTI_MONITOR = 1U << 3,
    UMI_EXPERIENCE_PANEL_CONTEXT_LINKED = 1U << 4,
    UMI_EXPERIENCE_PANEL_READ_ONLY = 1U << 5,
    UMI_EXPERIENCE_PANEL_CRITICAL = 1U << 6
} UmiExperiencePanelFlags;

/* Layout flags describe user customisation and placement capabilities. */
typedef enum UmiExperienceLayoutFlags {
    UMI_EXPERIENCE_LAYOUT_LOCKABLE = 1U << 0,
    UMI_EXPERIENCE_LAYOUT_MULTI_MONITOR = 1U << 1,
    UMI_EXPERIENCE_LAYOUT_RESPONSIVE = 1U << 2,
    UMI_EXPERIENCE_LAYOUT_CONTEXT_LINKED = 1U << 3
} UmiExperienceLayoutFlags;

/* A panel definition is immutable catalogue metadata. All string pointers are
 * borrowed from Framework and remain valid for the life of the process. */
typedef struct UmiExperiencePanelDefinition {
    uint32_t structure_size;          /* Size allows compatible contract growth. */
    const char *panel_id;             /* Stable ID used by saved layouts. */
    const char *title;                /* Human-readable panel name. */
    const char *summary;              /* Beginner-readable panel purpose. */
    const char *required_capability;  /* Capability needed before activation. */
    const char *context_group_id;     /* Optional linked-selection group. */
    const char *default_region;       /* Suggested starting dock region. */
    uint32_t flags;                   /* Combined UmiExperiencePanelFlags bits. */
} UmiExperiencePanelDefinition;

/* A layout is a starting panel recipe. Applications must not treat it as a
 * permanent restriction on user docking, floating or multi-monitor placement. */
typedef struct UmiExperienceLayoutDefinition {
    uint32_t structure_size;          /* Size allows fields to be appended safely. */
    const char *layout_id;            /* Stable ID used by preferences. */
    const char *title;                /* Human-readable layout name. */
    const char *description;          /* Work pattern supported by the recipe. */
    const char *const *panel_ids;     /* Borrowed array of stable panel IDs. */
    size_t panel_count;               /* Number of entries in panel_ids. */
    uint32_t flags;                   /* Combined UmiExperienceLayoutFlags bits. */
} UmiExperienceLayoutDefinition;

/* A feature definition records roadmap truth separately from panel presence.
 * A visible placeholder panel therefore cannot imply production readiness. */
typedef struct UmiExperienceFeatureDefinition {
    uint32_t structure_size;             /* Size allows compatible extension. */
    const char *feature_id;              /* Stable roadmap and evidence ID. */
    const char *title;                   /* Human-readable feature name. */
    const char *summary;                 /* User outcome, not implementation detail. */
    UmiExperienceFeatureState state;     /* Current truthful maturity. */
    UmiExperiencePriority priority;      /* Agreed delivery importance. */
    UmiExperienceOwnership owner;        /* Framework, application or adapter. */
    const char *required_capability;     /* Capability needed by the feature. */
} UmiExperienceFeatureDefinition;

/* The complete experience joins one thin application's panels, layouts and
 * feature roadmap. All nested arrays are borrowed immutable catalogue memory. */
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

/* Validate structure sizes, identifiers, panel references and feature values.
 * Validation never takes ownership of the supplied definition. */
UmiStatus umi_application_experience_validate(
    const UmiApplicationExperienceDefinition *definition);

/* Find a borrowed panel by stable ID, or return NULL when it is unavailable. */
const UmiExperiencePanelDefinition *umi_application_experience_panel_find(
    const UmiApplicationExperienceDefinition *definition,
    const char *panel_id);

/* Find a borrowed layout recipe by stable ID, or return NULL when unknown. */
const UmiExperienceLayoutDefinition *umi_application_experience_layout_find(
    const UmiApplicationExperienceDefinition *definition,
    const char *layout_id);

/* Find borrowed roadmap metadata by stable feature ID. */
const UmiExperienceFeatureDefinition *umi_application_experience_feature_find(
    const UmiApplicationExperienceDefinition *definition,
    const char *feature_id);

/* Count features in one maturity state for dashboards and release evidence. */
size_t umi_application_experience_feature_state_count(
    const UmiApplicationExperienceDefinition *definition,
    UmiExperienceFeatureState state);

/* Return a bounded zero-to-one-hundred readiness summary from feature states. */
unsigned umi_application_experience_readiness_percent(
    const UmiApplicationExperienceDefinition *definition);

/* Return immutable text suitable for logs, tables and generated references. */
const char *umi_experience_feature_state_text(UmiExperienceFeatureState state);
/* Return immutable priority text without allocating caller-owned memory. */
const char *umi_experience_priority_text(UmiExperiencePriority priority);
/* Return immutable ownership text for roadmap and diagnostic views. */
const char *umi_experience_ownership_text(UmiExperienceOwnership owner);

#ifdef __cplusplus
}
#endif

#endif
