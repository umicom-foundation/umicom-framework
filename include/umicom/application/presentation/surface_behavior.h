/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/presentation/surface_behavior.h
 *
 * PURPOSE:
 *   Define portable runtime behavior for reusable application panels after a
 *   workspace recipe has been validated and projected.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRESENTATION_SURFACE_BEHAVIOR_H
#define UMICOM_APPLICATION_PRESENTATION_SURFACE_BEHAVIOR_H

#include "umicom/application/presentation/runtime_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_APPLICATION_PRESENTATION_BEHAVIOR_API_VERSION 2U

/**
 * List the named application presentation refresh policy values accepted by this public
 * contract.
 */
typedef enum UmiApplicationPresentationRefreshPolicy {
    UMI_APPLICATION_PRESENTATION_REFRESH_MANUAL = 1,
    UMI_APPLICATION_PRESENTATION_REFRESH_ON_FOCUS = 2,
    UMI_APPLICATION_PRESENTATION_REFRESH_INTERVAL = 3,
    UMI_APPLICATION_PRESENTATION_REFRESH_STREAMING = 4
} UmiApplicationPresentationRefreshPolicy;

/**
 * List the named application presentation selection policy values accepted by this public
 * contract.
 */
typedef enum UmiApplicationPresentationSelectionPolicy {
    UMI_APPLICATION_PRESENTATION_SELECTION_NONE = 1,
    UMI_APPLICATION_PRESENTATION_SELECTION_SINGLE = 2,
    UMI_APPLICATION_PRESENTATION_SELECTION_MULTIPLE = 3,
    UMI_APPLICATION_PRESENTATION_SELECTION_RANGE = 4
} UmiApplicationPresentationSelectionPolicy;

/**
 * List the named application presentation persistence policy values accepted by this
 * public contract.
 */
typedef enum UmiApplicationPresentationPersistencePolicy {
    UMI_APPLICATION_PRESENTATION_PERSISTENCE_TRANSIENT = 1,
    UMI_APPLICATION_PRESENTATION_PERSISTENCE_SESSION = 2,
    UMI_APPLICATION_PRESENTATION_PERSISTENCE_WORKSPACE = 3
} UmiApplicationPresentationPersistencePolicy;

/**
 * List the named application presentation command mode values accepted by this public
 * contract.
 */
typedef enum UmiApplicationPresentationCommandMode {
    UMI_APPLICATION_PRESENTATION_COMMAND_READ_ONLY = 1,
    UMI_APPLICATION_PRESENTATION_COMMAND_EDITABLE = 2,
    UMI_APPLICATION_PRESENTATION_COMMAND_TRANSACTIONAL = 3,
    UMI_APPLICATION_PRESENTATION_COMMAND_GUARDED = 4
} UmiApplicationPresentationCommandMode;

/** Describe the network access a surface needs before it can become active. */
typedef enum UmiApplicationPresentationConnectivity {
    UMI_APPLICATION_PRESENTATION_CONNECTIVITY_OFFLINE = 1,
    UMI_APPLICATION_PRESENTATION_CONNECTIVITY_OPTIONAL = 2,
    UMI_APPLICATION_PRESENTATION_CONNECTIVITY_REQUIRED = 3,
    UMI_APPLICATION_PRESENTATION_CONNECTIVITY_STREAMING = 4
} UmiApplicationPresentationConnectivity;

/** Describe the most sensitive data a surface is designed to display. */
typedef enum UmiApplicationPresentationDataClassification {
    UMI_APPLICATION_PRESENTATION_DATA_PUBLIC = 1,
    UMI_APPLICATION_PRESENTATION_DATA_INTERNAL = 2,
    UMI_APPLICATION_PRESENTATION_DATA_CONFIDENTIAL = 3,
    UMI_APPLICATION_PRESENTATION_DATA_RESTRICTED = 4
} UmiApplicationPresentationDataClassification;

/**
 * Represent the application presentation surface behavior data shared with callers of this
 * public contract.
 */
typedef struct UmiApplicationPresentationSurfaceBehavior {
    uint32_t struct_size;
    uint32_t api_version;
    const char *component_id;
    UmiApplicationPresentationSurfaceState initial_state;
    UmiApplicationPresentationRefreshPolicy refresh_policy;
    uint32_t refresh_interval_seconds;
    UmiApplicationPresentationSelectionPolicy selection_policy;
    UmiApplicationPresentationPersistencePolicy persistence_policy;
    UmiApplicationPresentationCommandMode command_mode;
    const char *context_group_id;
    int lazy_data;
    int retain_when_hidden;
    int publish_context;
    int accept_context;
    UmiApplicationPresentationConnectivity connectivity;
    UmiApplicationPresentationDataClassification data_classification;
} UmiApplicationPresentationSurfaceBehavior;

/**
 * Check that application presentation surface behavior satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_application_presentation_surface_behavior_validate(
    const UmiApplicationPresentationSurfaceBehavior *behavior);
/**
 * Provide the application presentation refresh policy text operation used by this module
 * and its client applications.
 */
const char *umi_application_presentation_refresh_policy_text(
    UmiApplicationPresentationRefreshPolicy policy);
/**
 * Provide the application presentation selection policy text operation used by this module
 * and its client applications.
 */
const char *umi_application_presentation_selection_policy_text(
    UmiApplicationPresentationSelectionPolicy policy);
/**
 * Provide the application presentation persistence policy text operation used by this
 * module and its client applications.
 */
const char *umi_application_presentation_persistence_policy_text(
    UmiApplicationPresentationPersistencePolicy policy);
/**
 * Provide the application presentation command mode text operation used by this module and
 * its client applications.
 */
const char *umi_application_presentation_command_mode_text(
    UmiApplicationPresentationCommandMode mode);
/** Return stable text for settings, audit reports and generated documentation. */
const char *umi_application_presentation_connectivity_text(
    UmiApplicationPresentationConnectivity connectivity);
/** Return stable text for settings, audit reports and generated documentation. */
const char *umi_application_presentation_data_classification_text(
    UmiApplicationPresentationDataClassification classification);
/**
 * Decide whether runtime conditions permit a surface to activate. A surface
 * cannot silently cross either its network or data-sensitivity boundary.
 */
UmiStatus umi_application_presentation_surface_behavior_can_activate(
    const UmiApplicationPresentationSurfaceBehavior *behavior,
    int network_available,
    UmiApplicationPresentationDataClassification maximum_classification);

#ifdef __cplusplus
}
#endif

#endif
