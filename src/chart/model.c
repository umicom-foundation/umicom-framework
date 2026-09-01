/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/model.c
 *
 * PURPOSE:
 *   Own the toolkit-neutral chart model and series catalogue, including the
 *   canonical stack-safe allocation path used by Framework consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * The chart model keeps its point storage inline so adding points remains
 * bounded and deterministic.  The complete model is intentionally large, so
 * umi_chart_model_create() places that storage on the heap rather than asking
 * every application to invent its own allocation workaround.
 */

#include "umicom/chart/model.h"
#include "umicom/web/types.h"

#include <stdlib.h>
#include <string.h>

UmiStatus umi_chart_model_create(
    const char *title,
    UmiChartModel **out_model)
{
    UmiChartModel *model;
    UmiStatus status;

    if (title == NULL || out_model == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Clear the output immediately so callers never receive a stale pointer
     * when allocation or initialisation fails.
     */
    *out_model = NULL;

    model = (UmiChartModel *)calloc(1U, sizeof(*model));
    if (model == NULL) {
        return UMI_STATUS_OUT_OF_MEMORY;
    }

    status = umi_chart_model_init(model, title);
    if (status != UMI_STATUS_OK) {
        free(model);
        return status;
    }

    *out_model = model;
    return UMI_STATUS_OK;
}

void umi_chart_model_destroy(UmiChartModel *model)
{
    /* free(NULL) is defined, but the explicit check documents the contract. */
    if (model == NULL) {
        return;
    }

    free(model);
}

UmiStatus umi_chart_model_init(
    UmiChartModel *model,
    const char *title)
{
    if (model == NULL || title == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(model, 0, sizeof(*model));
    umi_chart_viewport_init(&model->viewport);
    return umi_web_copy_text(model->title, sizeof(model->title), title);
}

UmiStatus umi_chart_model_add_series(
    UmiChartModel *model,
    const UmiChartSeries *series)
{
    if (model == NULL || series == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (model->series_count >= UMI_CHART_MAX_SERIES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    /*
     * The model owns a value copy.  A caller can therefore reuse or clear its
     * source series after this call without changing the chart model.
     */
    model->series[model->series_count] = *series;
    ++model->series_count;
    ++model->revision;
    return UMI_STATUS_OK;
}

UmiChartSeries *umi_chart_model_find_series(
    UmiChartModel *model,
    const char *id)
{
    size_t index;

    if (model == NULL || id == NULL) {
        return NULL;
    }

    for (index = 0U; index < model->series_count; ++index) {
        if (strcmp(model->series[index].id, id) == 0) {
            return &model->series[index];
        }
    }

    return NULL;
}
