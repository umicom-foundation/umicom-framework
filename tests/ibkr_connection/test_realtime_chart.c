/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_realtime_chart.c
 * PURPOSE: Check chart freshness, projection, pan ranges and independence from connection lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "realtime_fixture.h"
int main(int argc, char **argv)
{
    if (argc != 2)
        return 2;
    const char *mode = argv[1];
    static const char *const cases[] = {"candle",    "scene",     "single",     "pan",       "pending",
                                        "age",       "cancelled", "disconnect", "precision", "missing-volume",
                                        "bad-range", "no-output", "owned",      "frozen"};
    if (!HistoricalKnownCase(mode, cases, sizeof cases / sizeof cases[0]))
        return 2;
    StreamReferences();

    Fixture *f = New();
    CHECK(f);
    CHECK(Connect(f) == 0);
    UmiIbkrRealtimeQuery q = StreamQuery();
    uint32_t request;
    CHECK(UmiIbkrRealtimeRequest(f->c, &q, 10U, &request) == UMI_STATUS_OK);
    if (strcmp(mode, "pending"))
    {
        CHECK(StreamFeed(f, request, 1791363600U) == 0);
        CHECK(StreamFeed(f, request, 1791363605U) == 0);
        CHECK(StreamPump(f, 11U) == 0);
    }
    UmiIbkrRealtimeStore *store = UmiIbkrRealtimeFind(f->c, request);
    CHECK(store);
    if (!strcmp(mode, "precision"))
        store->bars[1].open.exact = false;
    if (!strcmp(mode, "missing-volume"))
        store->bars[0].volumeAvailable = false;
    if (!strcmp(mode, "cancelled"))
        CHECK(UmiIbkrRealtimeCancel(f->c, request, 12U) == UMI_STATUS_OK);
    if (!strcmp(mode, "disconnect"))
        UmiIbkrConnectionClose(f->c);
    uint64_t now = !strcmp(mode, "age") ? 112U : 12U;
    if (!strcmp(mode, "candle") || !strcmp(mode, "missing-volume"))
    {
        UmiChartCandle candle;
        bool volume;
        CHECK(UmiIbkrRealtimeCandleCopy(f->c, request, 0U, now, 100U, &candle, &volume) == UMI_STATUS_OK);
        CHECK(candle.time_ms == INT64_C(1791363600000) && candle.open == 10.0 && candle.high == 12.0 &&
              candle.low == 9.0 && candle.close == 11.0);
        CHECK(volume == (strcmp(mode, "missing-volume") != 0) && candle.volume == (volume ? 100.0 : 0.0));
    }
    else if (!strcmp(mode, "no-output"))
    {
        CHECK(UmiIbkrRealtimeSceneCreate(f->c, request, 0U, 2U, now, 100U, NULL) ==
              UMI_STATUS_INVALID_ARGUMENT);
    }
    else
    {
        UmiChartRenderScene *original = NULL;
        CHECK(umi_chart_render_scene_create(1U, &original) == UMI_STATUS_OK);
        UmiChartRenderScene *scene = original;
        size_t first = !strcmp(mode, "pan") ? 1U : !strcmp(mode, "bad-range") ? SIZE_MAX : 0U;
        size_t count = (!strcmp(mode, "single") || !strcmp(mode, "pan")) ? 1U : 2U;
        UmiStatus expected = UMI_STATUS_OK;
        if (!strcmp(mode, "pending") || !strcmp(mode, "age") || !strcmp(mode, "cancelled") ||
            !strcmp(mode, "disconnect"))
            expected = UMI_STATUS_INVALID_STATE;
        if (!strcmp(mode, "precision"))
            expected = UMI_STATUS_NOT_IMPLEMENTED;
        if (!strcmp(mode, "bad-range"))
            expected = UMI_STATUS_INVALID_ARGUMENT;
        CHECK(UmiIbkrRealtimeSceneCreate(f->c, request, first, count, now, 100U, &scene) == expected);
        if (expected != UMI_STATUS_OK)
            CHECK(scene == original);
        else
        {
            CHECK(scene != original && umi_chart_render_scene_count(scene) > 3U);
            size_t commands = umi_chart_render_scene_count(scene);
            if (!strcmp(mode, "frozen"))
            {
                CHECK(StreamFeed(f, request, 1791363610U) == 0);
                CHECK(StreamPump(f, 13U) == 0);
                CHECK(umi_chart_render_scene_count(scene) == commands);
            }
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
