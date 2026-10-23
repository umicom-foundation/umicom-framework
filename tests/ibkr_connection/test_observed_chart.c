/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_observed_chart.c
 * PURPOSE: Inspect historical and streaming chart commands through injected broker observations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "realtime_fixture.h"
int main(int argc, char **argv) {
    if (argc != 3)
        return 2;
    bool stream = strcmp(argv[1], "streaming") == 0;
    if (!stream && strcmp(argv[1], "historical") != 0)
        return 2;
    const char *mode = argv[2];
    const char *const modes[] = {"labels", "alignment", "mixed-volume", "missing-volume",
                                "zero-volume", "volume-precision", "owned"};
    if (!HistoricalKnownCase(mode, modes, sizeof modes / sizeof modes[0]))
        return 2;
    StreamReferences();
    Fixture *f = New();
    CHECK(f != NULL && Connect(f) == 0);
    uint32_t request;
    UmiIbkrHistoricalBar *bars;
    if (stream) {
        UmiIbkrRealtimeQuery query = StreamQuery();
        CHECK(UmiIbkrRealtimeRequest(f->c, &query, 10U, &request) == UMI_STATUS_OK);
        CHECK(StreamFeed(f, request, 1791363600U) == 0);
        CHECK(StreamFeed(f, request, 1791363605U) == 0);
        CHECK(StreamPump(f, 11U) == 0);
        bars = UmiIbkrRealtimeFind(f->c, request)->bars;
    } else {
        UmiIbkrHistoricalQuery query = HistoricalQuery();
        CHECK(UmiIbkrHistoricalRequest(f->c, &query, 10U, &request) == UMI_STATUS_OK);
        CHECK(HistoricalFeed(f, request) == 0);
        CHECK(UmiIbkrConnectionPump(f->c, 11U) == UMI_STATUS_OK);
        bars = f->c->history->bars;
    }
    if (strcmp(mode, "mixed-volume") == 0 || strcmp(mode, "missing-volume") == 0)
        bars[0].volumeAvailable = false;
    if (strcmp(mode, "missing-volume") == 0)
        bars[1].volumeAvailable = false;
    if (strcmp(mode, "zero-volume") == 0) {
        bars[0].volume.value.coefficient = 0;
        bars[1].volume.value.coefficient = 0;
    }
    if (strcmp(mode, "volume-precision") == 0)
        bars[1].volume.exact = false;
    UmiChartRenderScene *original = NULL;
    CHECK(umi_chart_render_scene_create(1U, &original) == UMI_STATUS_OK);
    UmiChartRenderScene *scene = original;
    UmiStatus status = stream ? UmiIbkrRealtimeSceneCreate(f->c, request, 0U, 2U, 12U, 100U, &scene)
                             : UmiIbkrHistoricalSceneCreate(f->c, request, 0U, 2U, 12U, 100U, &scene);
    if (strcmp(mode, "volume-precision") == 0) {
        CHECK(status == UMI_STATUS_NOT_IMPLEMENTED && scene == original);
        umi_chart_render_scene_destroy(original);
        Delete(f);
        return 0;
    }
    CHECK(status == UMI_STATUS_OK && scene != original);
    if (strcmp(mode, "owned") == 0) {
        Delete(f);
        f = NULL;
    }
    UmiChartRenderRectangle bodies[2], volumes[2];
    size_t bodyCount = 0U, volumeCount = 0U, timeCount = 0U;
    bool summary = false, seconds = false;
    for (size_t i = 0U; i < umi_chart_render_scene_count(scene); ++i) {
        UmiChartRenderCommand command;
        CHECK(umi_chart_render_scene_at(scene, i, &command) == UMI_STATUS_OK);
        if (command.kind == UMI_CHART_RENDER_FILL_RECTANGLE && command.rectangle.width <= 36.0) {
            if (command.rectangle.y < 290.0) {
                CHECK(bodyCount < 2U);
                bodies[bodyCount++] = command.rectangle;
            } else if (command.rectangle.y >= 325.0) {
                CHECK(volumeCount < 2U);
                volumes[volumeCount++] = command.rectangle;
            }
        }
        if (command.kind == UMI_CHART_RENDER_TEXT) {
            if (strstr(command.text, " UTC") != NULL) {
                ++timeCount;
                if (strstr(command.text, ":05 UTC") != NULL)
                    seconds = true;
            }
            if (strstr(command.text, "Volume ") != NULL) {
                summary = true;
                if (strcmp(mode, "missing-volume") == 0)
                    CHECK(strstr(command.text, "unavailable") != NULL);
                else if (strcmp(mode, "mixed-volume") == 0)
                    CHECK(strstr(command.text, "1/2 observed") != NULL);
                else
                    CHECK(strstr(command.text, "2/2 observed") != NULL);
            }
        }
    }
    CHECK(bodyCount == 2U && timeCount == 2U && summary);
    CHECK(!stream || seconds);
    size_t wanted = strcmp(mode, "missing-volume") == 0 ? 0U :
                    strcmp(mode, "mixed-volume") == 0 ? 1U : 2U;
    CHECK(volumeCount == wanted);
    for (size_t i = 0U; i < wanted; ++i) {
        size_t body = wanted == 1U ? 1U : i;
        CHECK(volumes[i].x == bodies[body].x && volumes[i].width == bodies[body].width);
        if (strcmp(mode, "zero-volume") == 0)
            CHECK(volumes[i].height == 1.0);
    }
    umi_chart_render_scene_destroy(scene);
    umi_chart_render_scene_destroy(original);
    Delete(f);
    return 0;
}
