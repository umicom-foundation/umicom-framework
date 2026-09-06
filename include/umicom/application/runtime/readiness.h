/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/runtime/readiness.h
 *
 * PURPOSE:
 *   Compute detailed feature readiness and priority backlog summaries for an application experience.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_APPLICATION_RUNTIME_READINESS_H
#define UMICOM_APPLICATION_RUNTIME_READINESS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "umicom/application/runtime_catalogue.h"
#include "umicom/application/runtime/types.h"
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the application readiness report data shared with callers of this public
 * contract.
 */
typedef struct UmiApplicationReadinessReport {
    uint32_t structure_size;
    size_t feature_count;
    size_t planned_count;
    size_t foundation_count;
    size_t implemented_count;
    size_t verified_count;
    size_t p0_open_count;
    size_t p1_open_count;
    size_t framework_owned_count;
    size_t application_owned_count;
    size_t external_adapter_count;
    unsigned readiness_percent;
    const UmiExperienceFeatureDefinition *next_feature;
} UmiApplicationReadinessReport;

/**
 * Explain why a registered application can or cannot be offered by the shared launcher.
 * This is separate from feature maturity: an application may still have planned
 * features while its default workspace is valid and safe to open.
 */
typedef enum UmiApplicationLaunchReadinessState {
    UMI_APPLICATION_LAUNCH_READINESS_UNKNOWN = 0,
    UMI_APPLICATION_LAUNCH_READINESS_READY = 1,
    UMI_APPLICATION_LAUNCH_READINESS_MISSING_EXPERIENCE = 2,
    UMI_APPLICATION_LAUNCH_READINESS_INVALID_EXPERIENCE = 3,
    UMI_APPLICATION_LAUNCH_READINESS_NO_LAYOUT = 4,
    UMI_APPLICATION_LAUNCH_READINESS_NO_PANELS = 5
} UmiApplicationLaunchReadinessState;

/**
 * Carry the bounded launch gate result used by Desk and every thin product
 * that asks Framework to open an application.
 */
typedef struct UmiApplicationLaunchReadiness {
    uint32_t structure_size;
    char application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    UmiApplicationLaunchReadinessState state;
    bool launchable;
    size_t layout_count;
    size_t panel_count;
    unsigned feature_readiness_percent;
    char reason[UMI_APPLICATION_RUNTIME_MESSAGE_CAPACITY];
} UmiApplicationLaunchReadiness;

/**
 * Summarise launch readiness for the complete Framework application portfolio.
 *
 * The summary lets a launcher, dashboard or release check answer one simple
 * question without reimplementing the readiness rules: how many products can
 * open now, and why are the others blocked?
 */
typedef struct UmiApplicationLaunchReadinessSummary {
    uint32_t structure_size;
    size_t application_count;
    size_t ready_count;
    size_t blocked_count;
    size_t missing_experience_count;
    size_t invalid_experience_count;
    size_t no_layout_count;
    size_t no_panels_count;
    unsigned average_feature_readiness_percent;
    char first_blocked_application_id[UMI_APPLICATION_RUNTIME_ID_CAPACITY];
    char first_blocked_reason[UMI_APPLICATION_RUNTIME_MESSAGE_CAPACITY];
} UmiApplicationLaunchReadinessSummary;

/**
 * Provide the application readiness report operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_readiness_report(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationReadinessReport *out_report);
/**
 * Provide the application readiness has open priority operation used by this module and
 * its client applications.
 */
int umi_application_readiness_has_open_priority(
    const UmiApplicationReadinessReport *report);

/**
 * Check the canonical Framework experience before the application picker marks
 * a runtime record as launchable.
 */
UmiStatus umi_application_launch_readiness_check(
    const char *application_id,
    UmiApplicationLaunchReadiness *out_readiness);

/**
 * Return stable text for a launch-readiness state so consoles and GUI panels
 * can display the same explanation without duplicating labels.
 */
const char *umi_application_launch_readiness_state_text(
    UmiApplicationLaunchReadinessState state);

/**
 * Check every canonical experience and collect one bounded portfolio summary
 * for launchers, release dashboards and thin application consoles.
 */
UmiStatus umi_application_launch_readiness_summary(
    UmiApplicationLaunchReadinessSummary *out_summary);

#ifdef __cplusplus
}
#endif

#endif
