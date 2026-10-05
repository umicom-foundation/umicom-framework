/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart/test_volume_profile_scene.c
 * PURPOSE: Exercise profile composition, timeframe changes and navigation while retaining order state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../chart_timeframe/fixture.h"
#include "umicom/chart/volume_profile.h"
#include <math.h>
int main(int argc, char **argv)
{
    CHECK(argc == 2);
    const char *name = argv[1];
    ReviewFixture f;
    ReviewFixtureInit(&f);
    TimeframeBars(&f, 40U);
    UmiTradingWorkspaceSnapshot before;
    OK(umi_trading_workspace_snapshot(f.workspace, &before));
    if (strcmp(name, "timeframe") == 0)
        OK(UmiTradingWorkspaceSetChartTimeframe(f.workspace, before.selected_instrument_id, 300000U));
    else
        CHECK(strcmp(name, "scene") == 0 || strcmp(name, "adapter") == 0 ||
              strcmp(name, "invalid-time") == 0);
    if (strcmp(name, "adapter") == 0 || strcmp(name, "invalid-time") == 0)
    {
        UmiChartCandle candles[] = {
            {100, 10, 15, 5, 10, 10}, {200, 12, 18, 8, 12, 30}, {300, 14, 20, 10, 14, 60}};
        UmiChartVolumeProfile result = {0};
        if (strcmp(name, "invalid-time") == 0)
        {
            candles[2].time_ms = 200;
            CHECK(UmiChartVolumeProfileFromCandles(candles, 3U, 2U, 0.70, &result) ==
                      UMI_STATUS_INVALID_ARGUMENT &&
                  result.bin_count == 0U);
        }
        else
        {
            OK(UmiChartVolumeProfileFromCandles(candles, 3U, 2U, 0.70, &result));
            CHECK(result.control_bin == 1U && result.area_low == 12 && result.area_high == 14 &&
                  fabs(result.area_volume - 90) < 1e-9);
        }
    }
    OK(umi_trading_workspace_set_chart_study(f.workspace, UMI_TRADING_CHART_STUDY_VOLUME_PROFILE, 20U));
    UmiTradingChartSceneInfo info = {0};
    UmiChartRenderScene *scene = NULL;
    OK(UmiTradingChartBuildScene(f.workspace, &info, &scene));
    size_t controls = 0, notes = 0;
    for (size_t i = 0; i < umi_chart_render_scene_count(scene); ++i)
    {
        UmiChartRenderCommand command;
        OK(umi_chart_render_scene_at(scene, i, &command));
        if (command.kind == UMI_CHART_RENDER_TEXT && strcmp(command.text, "Profile control") == 0)
            ++controls;
        if (command.kind == UMI_CHART_RENDER_TEXT && strstr(command.text, "Volume at bar close (approx.)"))
            ++notes;
    }
    CHECK(controls == 1U && notes == 1U);
    SameTrading(f.workspace, &before);
    umi_chart_render_scene_destroy(scene);
    umi_trading_workspace_destroy(f.workspace);
    return 0;
}
