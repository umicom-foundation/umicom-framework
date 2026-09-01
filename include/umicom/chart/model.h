/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/model.h
 *
 * PURPOSE:
 *   Own the toolkit-neutral chart model and series catalogue, including a
 *   stack-safe allocation API for normal application and test usage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * UmiChartModel intentionally contains a large, fixed-capacity series store so
 * chart data remains deterministic and does not allocate while points are being
 * added.  Because that complete store is larger than a normal Windows thread
 * stack, applications should normally create a model with
 * umi_chart_model_create() and release it with umi_chart_model_destroy().
 *
 * umi_chart_model_init() remains available when the caller already owns safe
 * storage, for example static storage, global storage, or separately allocated
 * memory.  This preserves the existing public C contract while preventing
 * future applications from needing their own chart-model allocation wrapper.
 */

#ifndef UMICOM_CHART_MODEL_H
#define UMICOM_CHART_MODEL_H

#include "umicom/chart/series.h"
#include "umicom/chart/viewport.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiChartModel {
    char title[UMI_CHART_NAME_CAPACITY];
    UmiChartSeries series[UMI_CHART_MAX_SERIES];
    size_t series_count;
    UmiChartViewport viewport;
    uint64_t revision;
} UmiChartModel;

/*
 * Allocate a complete chart model on the heap and initialise it with title.
 * This is the recommended entry point for application-local chart models.
 */
UmiStatus umi_chart_model_create(
    const char *title,
    UmiChartModel **out_model
);

/*
 * Release a model returned by umi_chart_model_create(). NULL is accepted.
 */
void umi_chart_model_destroy(UmiChartModel *model);

/*
 * Initialise caller-owned storage. The caller is responsible for ensuring the
 * storage is not placed on a thread stack that is too small for UmiChartModel.
 */
UmiStatus umi_chart_model_init(
    UmiChartModel *model,
    const char *title
);

UmiStatus umi_chart_model_add_series(
    UmiChartModel *model,
    const UmiChartSeries *series
);

UmiChartSeries *umi_chart_model_find_series(
    UmiChartModel *model,
    const char *id
);

#ifdef __cplusplus
}
#endif

#endif
