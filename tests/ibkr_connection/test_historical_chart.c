/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_historical_chart.c
 * PURPOSE: Check chart projection, missing-volume handling and independently owned render scenes.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "historical_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    static const char *const cases[] = {"candle",         "scene",
                                        "single",         "pan",
                                        "bad-range",      "no-output",
                                        "pending",        "age",
                                        "disconnect",     "precision",
                                        "missing-volume", "empty",
                                        "owned",          "failure-preserves"};
    if (!HistoricalKnownCase(mode, cases, sizeof cases / sizeof cases[0]))
        return 2;
    HistoricalReferences();
    Fixture *f = New();
    CHECK(f);
    CHECK(Connect(f) == 0);
    UmiIbkrHistoricalQuery q = HistoricalQuery();
    uint32_t request;
    CHECK(UmiIbkrHistoricalRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
    if (strcmp(mode, "pending"))
    {
        if (!strcmp(mode, "empty"))
        {
            char id[32];
            (void)snprintf(id, sizeof id, "%u", (unsigned)request);
            FEED(f, "17", id, "", "", "0");
        }
        else
            CHECK(HistoricalFeed(f, request) == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
    }
    if (!strcmp(mode, "disconnect"))
        UmiIbkrConnectionClose(f->c);
    if (!strcmp(mode, "precision"))
        f->c->history->bars[1].open.exact = false;
    if (!strcmp(mode, "missing-volume"))
        f->c->history->bars[0].volumeAvailable = false;
    uint64_t now = !strcmp(mode, "age") ? 112U : 12U;
    if (!strcmp(mode, "candle") || !strcmp(mode, "missing-volume"))
    {
        UmiChartCandle candle;
        bool volume = false;
        CHECK(UmiIbkrHistoricalCandleCopy(f->c, request, 0U, now, 100U, &candle, &volume) == UMI_STATUS_OK);
        CHECK(candle.time_ms == INT64_C(1791363600000) && candle.open == 10.0 && candle.high == 12.0 &&
              candle.low == 9.0 && candle.close == 11.0);
        CHECK(volume == (strcmp(mode, "missing-volume") != 0));
        CHECK(candle.volume == (volume ? 100.0 : 0.0));
    }
    else if (!strcmp(mode, "no-output"))
    {
        CHECK(UmiIbkrHistoricalSceneCreate(f->c, request, 0U, 2U, now, 100U, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
    else
    {
        UmiChartRenderScene *original = NULL;
        CHECK(umi_chart_render_scene_create(1U, &original) == UMI_STATUS_OK);
        UmiChartRenderScene *scene = original;
        size_t first = !strcmp(mode, "pan") ? 1U : 0U;
        size_t count = (!strcmp(mode, "single") || !strcmp(mode, "pan")) ? 1U : 2U;
        if (!strcmp(mode, "bad-range") || !strcmp(mode, "failure-preserves"))
            first = SIZE_MAX;
        UmiStatus expected = UMI_STATUS_OK;
        if (!strcmp(mode, "pending") || !strcmp(mode, "age") || !strcmp(mode, "disconnect"))
            expected = UMI_STATUS_INVALID_STATE;
        if (!strcmp(mode, "precision"))
            expected = UMI_STATUS_NOT_IMPLEMENTED;
        if (!strcmp(mode, "bad-range") || !strcmp(mode, "failure-preserves") || !strcmp(mode, "empty"))
            expected = UMI_STATUS_INVALID_ARGUMENT;
        CHECK(UmiIbkrHistoricalSceneCreate(f->c, request, first, count, now, 100U, &scene) == expected);
        if (expected != UMI_STATUS_OK)
            CHECK(scene == original && umi_chart_render_scene_count(original) == 0U);
        else
        {
            CHECK(scene != original && umi_chart_render_scene_count(scene) > 3U);
            if (!strcmp(mode, "owned"))
            {
                Delete(f);
                f = NULL;
            }
            UmiChartRenderCommand command;
            CHECK(umi_chart_render_scene_at(scene, 0U, &command) == UMI_STATUS_OK);
            CHECK(command.kind == UMI_CHART_RENDER_FILL_RECTANGLE);
            umi_chart_render_scene_destroy(scene);
        }
        umi_chart_render_scene_destroy(original);
    }
    Delete(f);
    return 0;
}
