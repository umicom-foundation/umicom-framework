/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_chart_model.c
 *
 * PURPOSE:
 *   Verify the toolkit-neutral chart model through the stack-safe public
 *   allocation API, series ownership and immutable snapshot contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * UmiChartModel contains the complete bounded chart store and is intentionally
 * large.  This regression test uses the same create/destroy API that normal
 * applications should use, so Windows test executables do not place the full
 * model on their comparatively small default thread stack.
 */

#include "umicom/umicom.h"

#include <assert.h>

int main(void)
{
    UmiChartModel *model = NULL;
    UmiChartSeries source_series;
    UmiChartSeries *stored_series;
    UmiChartSnapshot snapshot;

    assert(umi_chart_model_create("NQ", &model) == UMI_STATUS_OK);
    assert(model != NULL);

    assert(umi_chart_series_init(&source_series,
                                 "close",
                                 UMI_CHART_LINE) == UMI_STATUS_OK);
    assert(umi_chart_series_add(&source_series,
                                (UmiChartPoint){1, 100.0}) == UMI_STATUS_OK);

    assert(umi_chart_model_add_series(model, &source_series) == UMI_STATUS_OK);
    assert(model->series_count == 1U);
    assert(model->revision == 1U);

    stored_series = umi_chart_model_find_series(model, "close");
    assert(stored_series != NULL);
    assert(stored_series->point_count == 1U);
    assert(stored_series->points[0].value == 100.0);

    /*
     * Clear the caller-owned source to prove the model retained its own value
     * copy rather than depending on the source series lifetime.
     */
    umi_chart_series_clear(&source_series);
    assert(stored_series->point_count == 1U);

    assert(umi_chart_snapshot(model, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.series_count == 1U);
    assert(snapshot.point_count == 1U);
    assert(snapshot.revision == 1U);

    umi_chart_model_destroy(model);
    return 0;
}
