/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/test_bar_scene.c
 * PURPOSE: Validate time gaps, missing volume, bounds and all-or-nothing scene output.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "bar_chart_private.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(value) do { if (!(value)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #value); return 1; } } while (0)
int main(int argc, char **argv) {
    if (argc != 2)
        return 2;
    UmiChartCandle candles[UMI_IBKR_HISTORICAL_BAR_LIMIT];
    bool available[UMI_IBKR_HISTORICAL_BAR_LIMIT];
    for (size_t i = 0U; i < UMI_IBKR_HISTORICAL_BAR_LIMIT; ++i) {
        candles[i] = (UmiChartCandle){(int64_t)i * 5000, 10.0, 12.0, 9.0, 11.0, 100.0};
        available[i] = true;
    }
    size_t count = 3U;
    const char *caption = "Reference observations";
    UmiStatus expected = UMI_STATUS_OK;
    const char *mode = argv[1];
    if (strcmp(mode, "gap") == 0)
        candles[2].time_ms = 60000;
    else if (strcmp(mode, "negative-time") == 0) {
        count = 1U;
        candles[0].time_ms = -1;
    } else if (strcmp(mode, "capacity") == 0)
        count = UMI_IBKR_HISTORICAL_BAR_LIMIT;
    else if (strcmp(mode, "duplicate-time") == 0) {
        candles[2].time_ms = candles[1].time_ms;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "unordered") == 0) {
        candles[1].time_ms = -5;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "invalid-candle") == 0) {
        candles[2].volume = NAN;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "caption-overflow") == 0) {
        caption = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
                  "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
        expected = UMI_STATUS_CAPACITY_EXCEEDED;
    } else if (strcmp(mode, "bounds") == 0) {
        count = UMI_IBKR_HISTORICAL_BAR_LIMIT + 1U;
        expected = UMI_STATUS_INVALID_ARGUMENT;
    } else if (strcmp(mode, "single") == 0)
        count = 1U;
    else
        return 2;
    UmiChartRenderScene *original = NULL;
    CHECK(umi_chart_render_scene_create(1U, &original) == UMI_STATUS_OK);
    UmiChartRenderScene *scene = original;
    CHECK(UmiIbkrMarketBarSceneObserved(candles, available, count, caption, &scene) == expected);
    if (expected != UMI_STATUS_OK) {
        CHECK(scene == original && umi_chart_render_scene_count(original) == 0U);
    } else {
        CHECK(scene != original);
        double width = 0.0, height = 0.0;
        CHECK(umi_chart_render_scene_coordinate_size(scene, &width, &height) == UMI_STATUS_OK);
        CHECK(width == 960.0 && height == 440.0);
        size_t bodies = 0U, volumes = 0U, times = 0U;
        double bodyX[3] = {0}, volumeX[3] = {0};
        for (size_t i = 0U; i < umi_chart_render_scene_count(scene); ++i) {
            UmiChartRenderCommand c;
            CHECK(umi_chart_render_scene_at(scene, i, &c) == UMI_STATUS_OK);
            if (c.kind == UMI_CHART_RENDER_FILL_RECTANGLE && c.rectangle.width <= 36.0) {
                if (c.rectangle.y < 290.0) {
                    if (bodies < 3U) bodyX[bodies] = c.rectangle.x;
                    ++bodies;
                } else if (c.rectangle.y >= 325.0) {
                    if (volumes < 3U) volumeX[volumes] = c.rectangle.x;
                    ++volumes;
                }
            }
            if (c.kind == UMI_CHART_RENDER_TEXT && strstr(c.text, " UTC") != NULL) {
                ++times;
                if (strcmp(mode, "negative-time") == 0)
                    CHECK(strcmp(c.text, "1969-12-31 23:59:59 UTC") == 0);
            }
        }
        CHECK(bodies == count && volumes == count && times == (count == 1U ? 1U : 2U));
        if (strcmp(mode, "gap") == 0) {
            CHECK(bodyX[1] - bodyX[0] < (bodyX[2] - bodyX[1]) / 5.0);
            CHECK(bodyX[0] == volumeX[0] && bodyX[1] == volumeX[1] && bodyX[2] == volumeX[2]);
        }
        umi_chart_render_scene_destroy(scene);
    }
    umi_chart_render_scene_destroy(original);
    return 0;
}
