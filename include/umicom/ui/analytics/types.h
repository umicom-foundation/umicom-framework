/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/types.h
 *
 * PURPOSE:
 *   Define stable toolkit-neutral analytics limits, enums and utility operations.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral analytics capability extends canonical Umicom::ui.
 *   Existing Design System chart/gauge/heatmap specs and workstation surfaces
 *   remain authoritative; GTK4, Qt6, Native Web and thin applications render
 *   the same Framework-owned analytics semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_TYPES_H
#define UMICOM_UI_ANALYTICS_TYPES_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_ANALYTICS_ID_CAPACITY 128U
#define UMI_ANALYTICS_TEXT_CAPACITY 256U
#define UMI_ANALYTICS_VALUE_CAPACITY 512U
#define UMI_ANALYTICS_MAX_POINTS 512U
#define UMI_ANALYTICS_MAX_SERIES 32U
#define UMI_ANALYTICS_MAX_ITEMS 128U
#define UMI_ANALYTICS_MAX_BINS 64U
#define UMI_ANALYTICS_MAX_MATRIX 64U
#define UMI_ANALYTICS_MAX_CELLS (UMI_ANALYTICS_MAX_MATRIX * UMI_ANALYTICS_MAX_MATRIX)

/**
 * List the named analytics axis scale values accepted by this public contract.
 */
typedef enum UmiAnalyticsAxisScale {
    UMI_ANALYTICS_SCALE_LINEAR = 1,
    UMI_ANALYTICS_SCALE_LOG10 = 2,
    UMI_ANALYTICS_SCALE_TIME = 3,
    UMI_ANALYTICS_SCALE_CATEGORY = 4
} UmiAnalyticsAxisScale;

/**
 * List the named analytics severity values accepted by this public contract.
 */
typedef enum UmiAnalyticsSeverity {
    UMI_ANALYTICS_SEVERITY_INFO = 1,
    UMI_ANALYTICS_SEVERITY_SUCCESS = 2,
    UMI_ANALYTICS_SEVERITY_WARNING = 3,
    UMI_ANALYTICS_SEVERITY_ERROR = 4
} UmiAnalyticsSeverity;

/**
 * List the named analytics aggregate kind values accepted by this public contract.
 */
typedef enum UmiAnalyticsAggregateKind {
    UMI_ANALYTICS_AGGREGATE_COUNT = 1,
    UMI_ANALYTICS_AGGREGATE_SUM = 2,
    UMI_ANALYTICS_AGGREGATE_MIN = 3,
    UMI_ANALYTICS_AGGREGATE_MAX = 4,
    UMI_ANALYTICS_AGGREGATE_AVERAGE = 5
} UmiAnalyticsAggregateKind;

/**
 * List the named analytics compare values accepted by this public contract.
 */
typedef enum UmiAnalyticsCompare {
    UMI_ANALYTICS_COMPARE_GT = 1,
    UMI_ANALYTICS_COMPARE_GTE = 2,
    UMI_ANALYTICS_COMPARE_LT = 3,
    UMI_ANALYTICS_COMPARE_LTE = 4,
    UMI_ANALYTICS_COMPARE_EQ = 5,
    UMI_ANALYTICS_COMPARE_NE = 6
} UmiAnalyticsCompare;

/**
 * List the named analytics orientation values accepted by this public contract.
 */
typedef enum UmiAnalyticsOrientation {
    UMI_ANALYTICS_HORIZONTAL = 1,
    UMI_ANALYTICS_VERTICAL = 2
} UmiAnalyticsOrientation;

/**
 * List the named analytics stage kind values accepted by this public contract.
 */
typedef enum UmiAnalyticsStageKind {
    UMI_ANALYTICS_STAGE_FILTER = 1,
    UMI_ANALYTICS_STAGE_GROUP = 2,
    UMI_ANALYTICS_STAGE_AGGREGATE = 3,
    UMI_ANALYTICS_STAGE_SORT = 4,
    UMI_ANALYTICS_STAGE_LIMIT = 5
} UmiAnalyticsStageKind;

/* Copy text into a bounded ABI buffer and reject silent truncation. */
UmiStatus umi_analytics_copy_text(char *destination, size_t capacity, const char *source);
/* Return one when a floating-point value is finite. */
int umi_analytics_number_valid(double value);
/* Evaluate a numeric comparison using Framework analytics semantics. */
int umi_analytics_compare(double left, UmiAnalyticsCompare op, double right);
/* Produce a deterministic FNV-1a fingerprint for analytics cache keys. */
uint64_t umi_analytics_hash_text(const char *text);

#ifdef __cplusplus
}
#endif

#endif
