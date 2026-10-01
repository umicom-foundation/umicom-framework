/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_timeframe/test_scene.c
 * PURPOSE: Check that candles, studies, volume and UTC labels use one displayed interval.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/


#include "fixture.h"
#include <math.h>
int main(int argc, char **argv)
{
    CHECK(argc==2); const char *name=argv[1]; ReviewFixture f; ReviewFixtureInit(&f);
    if(strcmp(name,"empty")!=0)TimeframeBars(&f,40U);
    UmiTradingWorkspaceSnapshot before; OK(umi_trading_workspace_snapshot(f.workspace,&before));
    const char *id=before.selected_instrument_id;
    OK(UmiTradingWorkspaceSetChartTimeframe(f.workspace,id,strcmp(name,"daily")==0?86400000U:300000U));
    if(strcmp(name,"study")==0)OK(umi_trading_workspace_set_chart_study(f.workspace,UMI_TRADING_CHART_STUDY_SIMPLE_AVERAGE,2U));
    else CHECK(strcmp(name,"aggregate")==0 || strcmp(name,"daily")==0 || strcmp(name,"empty")==0);
    UmiTradingChartSceneInfo info={0}; UmiChartRenderScene *scene=NULL;
    UmiStatus status=UmiTradingChartBuildScene(f.workspace,&info,&scene);
    if(strcmp(name,"empty")==0){
        CHECK(status==UMI_STATUS_NOT_FOUND && scene==NULL && info.source_bars==0U && info.interval_ms==300000U);
        CHECK(strcmp(info.instrument_id,id)==0);
    }else{
        OK(status); CHECK(scene!=NULL && info.source_bars==40U);
        CHECK(info.retained_bars==(strcmp(name,"daily")==0?1U:8U) && info.window.count==info.retained_bars);
        size_t averages=0U,dates=0U,volumes=0U;
        for(size_t i=0;i<umi_chart_render_scene_count(scene);++i){
            UmiChartRenderCommand command;OK(umi_chart_render_scene_at(scene,i,&command));
            if(command.kind==UMI_CHART_RENDER_LINE && fabs(command.stroke_width-1.7)<0.000001)++averages;
            if(command.kind==UMI_CHART_RENDER_TEXT && strstr(command.text,"1970-01-01")!=NULL)++dates;
            if(command.kind==UMI_CHART_RENDER_TEXT && strcmp(command.text,strcmp(name,"daily")==0?"Volume (max 1180)":"Volume (max 235)")==0)++volumes;
        }
        CHECK(dates==5U && volumes==1U);
        CHECK(averages==(strcmp(name,"study")==0?6U:0U));
    }
    SameTrading(f.workspace,&before);umi_chart_render_scene_destroy(scene);umi_trading_workspace_destroy(f.workspace);return 0;
}
