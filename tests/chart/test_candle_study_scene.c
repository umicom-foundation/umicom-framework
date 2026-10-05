/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart/test_candle_study_scene.c
 * PURPOSE: Check real chart composition uses candle studies without changing order state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../chart_timeframe/fixture.h"
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
    UmiTradingChartStudy study;
    if (strcmp(name, "weighted") == 0)
        study = UMI_TRADING_CHART_STUDY_VOLUME_WEIGHTED;
    else if (strcmp(name, "bollinger") == 0)
        study = UMI_TRADING_CHART_STUDY_BOLLINGER;
    else
    {
        CHECK(strcmp(name, "donchian") == 0 || strcmp(name, "short") == 0 || strcmp(name, "timeframe") == 0);
        study = UMI_TRADING_CHART_STUDY_DONCHIAN;
    }
    if (strcmp(name, "timeframe") == 0)
        OK(UmiTradingWorkspaceSetChartTimeframe(f.workspace, before.selected_instrument_id, 300000U));
    OK(umi_trading_workspace_set_chart_study(f.workspace, study, strcmp(name, "short") == 0 ? 100U : 3U));
    UmiTradingChartSceneInfo info = {0};
    UmiChartRenderScene *scene = NULL;
    OK(UmiTradingChartBuildScene(f.workspace, &info, &scene));
    size_t centres = 0U, bands = 0U;
    for (size_t i = 0U; i < umi_chart_render_scene_count(scene); ++i)
    {
        UmiChartRenderCommand command;
        OK(umi_chart_render_scene_at(scene, i, &command));
        if (command.kind == UMI_CHART_RENDER_LINE && fabs(command.stroke_width - 1.7) < 1e-9)
            ++centres;
        if (command.kind == UMI_CHART_RENDER_LINE && fabs(command.stroke_width - 1.2) < 1e-9)
            ++bands;
    }
    if (strcmp(name, "short") == 0)
        CHECK(centres == 0U && bands == 0U);
    else
    {
        CHECK(centres > 0U);
        if (study != UMI_TRADING_CHART_STUDY_VOLUME_WEIGHTED)
            CHECK(bands > 0U);
    }
    CHECK(info.retained_bars == (strcmp(name, "timeframe") == 0 ? 8U : 40U));
    SameTrading(f.workspace, &before);
    umi_chart_render_scene_destroy(scene);
    umi_trading_workspace_destroy(f.workspace);
    return 0;
}
