/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_tools/test_workspace.c
 * PURPOSE: Exercise trading composition, scene output and reviewed persistence for drawing tools.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../trading_execution/order_review_fixture.h"
#include "umicom/chart/drawing_tools.h"
#include "umicom/trading/chart_persistence.h"
#include "umicom/trading_ui/chart_scene.h"
#define CHECK REVIEW_CHECK
#define OK(x) CHECK((x)==UMI_STATUS_OK)
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];ReviewFixture f;ReviewFixtureInit(&f);
    UmiTradingWorkspace *workspace=f.workspace;UmiTradingWorkspaceSnapshot before,after;OK(umi_trading_workspace_snapshot(workspace,&before));
    const char *id=before.selected_instrument_id;UmiChartDrawingRegistry *registry=umi_chart_workspace_drawings(umi_trading_workspace_charts(workspace));
    UmiChartPoint a={60000,100},b={120000,105};
    OK(UmiTradingWorkspaceAddChartDrawing(workspace,id,"liquidity-zone",a,b));
    UmiChartDrawingSnapshot drawing,current;OK(umi_chart_drawing_registry_at(registry,0,&drawing));
    if(strcmp(name,"create")==0){
        const char *tools[]={"trend","support","resistance","range","ray"};
        for(size_t i=0;i<5;++i)OK(UmiTradingWorkspaceAddChartDrawing(workspace,id,tools[i],a,b));
        CHECK(umi_chart_drawing_registry_count(registry)==6);
        for(size_t i=0;i<6;++i){OK(umi_chart_drawing_registry_at(registry,i,&current));OK(UmiChartDrawingToolValidate(&current));}
    }else if(strcmp(name,"edit")==0){
        OK(UmiTradingWorkspaceSetChartDrawingLocked(workspace,id,drawing.id,drawing.revision,1));OK(umi_chart_drawing_registry_find(registry,drawing.id,&drawing));
        CHECK(UmiTradingWorkspaceRemoveChartDrawing(workspace,id,drawing.id,drawing.revision)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiTradingWorkspaceMoveChartDrawing(workspace,id,drawing.id,drawing.revision,a,(UmiChartPoint){180000,110})==UMI_STATUS_PERMISSION_DENIED);
        char copy[128];OK(UmiTradingWorkspaceDuplicateChartDrawing(workspace,id,drawing.id,drawing.revision,copy,sizeof(copy)));
        OK(umi_chart_drawing_registry_find(registry,copy,&current));CHECK(!current.locked&&strcmp(copy,drawing.id)!=0);
        OK(UmiTradingWorkspaceMoveChartDrawing(workspace,id,copy,current.revision,a,(UmiChartPoint){180000,110}));
        OK(umi_chart_drawing_registry_find(registry,drawing.id,&current));CHECK(current.locked&&current.time2==120000);
    }else if(strcmp(name,"guards")==0){
        char copy[2]="x";uint64_t revision=umi_chart_drawing_registry_revision(registry);
        CHECK(UmiTradingWorkspaceDuplicateChartDrawing(workspace,id,drawing.id,drawing.revision,copy,sizeof(copy))==UMI_STATUS_CAPACITY_EXCEEDED);CHECK(strcmp(copy,"x")==0);
        CHECK(UmiTradingWorkspaceMoveChartDrawing(workspace,id,drawing.id,drawing.revision-1,a,b)==UMI_STATUS_BUSY);
        UmiInstrument other=test_instrument();
        CHECK(UmiTradingWorkspaceSetChartDrawingLocked(workspace,other.instrument_id.value,drawing.id,drawing.revision,1)==UMI_STATUS_INVALID_STATE);
        CHECK(umi_chart_drawing_registry_revision(registry)==revision);
    }else if(strcmp(name,"persistence")==0){
        OK(UmiTradingWorkspaceAddChartDrawing(workspace,id,"range",a,b));OK(UmiTradingWorkspaceAddChartDrawing(workspace,id,"ray",b,a));
        OK(UmiTradingWorkspaceSetChartDrawingLocked(workspace,id,drawing.id,drawing.revision,1));
        UmiDataServer *server=NULL;UmiTradingChartPersistence *service=NULL;OK(umi_data_server_create_memory(&server));
        OK(UmiTradingChartPersistenceCreate(workspace,&service));OK(UmiTradingChartPersistenceBind(service,server,"tools"));
        UmiChartCheckpointReport report;OK(UmiTradingChartPersistenceSave(service,id,1000,&report));
        UmiTradingChartPreview preview;OK(UmiTradingChartPersistencePreview(service,id,&preview));CHECK(preview.saved.drawing_count==3);
        OK(umi_chart_drawing_registry_at(registry,1,&current));OK(UmiTradingWorkspaceMoveChartDrawing(workspace,id,current.id,current.revision,a,(UmiChartPoint){180000,115}));
        CHECK(UmiTradingChartPersistenceRestore(service,id,preview.preview_id)==UMI_STATUS_INVALID_STATE);
        OK(UmiTradingChartPersistencePreview(service,id,&preview));OK(UmiTradingChartPersistenceRestore(service,id,preview.preview_id));
        OK(umi_chart_drawing_registry_at(registry,0,&current));CHECK(current.locked&&strcmp(current.tool,"liquidity-zone")==0);
        OK(umi_chart_drawing_registry_at(registry,1,&current));CHECK(current.time2==120000&&current.value2==105);
        OK(umi_chart_drawing_registry_at(registry,2,&current));CHECK(strcmp(current.tool,"ray")==0&&current.time1>current.time2);
        UmiTradingChartPersistenceDestroy(service);umi_data_server_destroy(server);
    }else if(strcmp(name,"empty-history")==0){
        UmiTradingChartSceneInfo info;memset(&info,0x5a,sizeof(info));
        UmiChartRenderScene *scene=NULL;
        CHECK(UmiTradingChartBuildScene(workspace,&info,&scene)==UMI_STATUS_NOT_FOUND);
        CHECK(scene==NULL&&info.retained_bars==0&&info.window.count==0&&info.price.area.width==0);
        CHECK(strcmp(info.instrument_id,id)==0&&umi_chart_drawing_registry_count(registry)==1);
    }else if(strcmp(name,"scene")==0){
        UmiTradingMarketSnapshot market;OK(umi_trading_workspace_selected_market(workspace,&market));
        for(int i=0;i<4;++i){UmiBar bar={0};bar.instrument=market.instrument;bar.start_time_ms=(i+1)*60000;bar.end_time_ms=bar.start_time_ms+59999;
            bar.open=100;bar.close=101;bar.low=90;bar.high=110;bar.volume=100;OK(umi_trading_workspace_update_bar(workspace,&bar,100));}
        UmiTradingChartSceneInfo info;UmiChartRenderScene *scene=NULL;OK(UmiTradingChartBuildScene(workspace,&info,&scene));
        size_t count=umi_chart_render_scene_count(scene),zones=0;
        for(size_t i=0;i<count;++i){UmiChartRenderCommand command;OK(umi_chart_render_scene_at(scene,i,&command));
            if(command.kind==UMI_CHART_RENDER_FILL_RECTANGLE&&command.color.alpha==0.16)++zones;}
        CHECK(zones==1);umi_chart_render_scene_destroy(scene);
        OK(UmiTradingWorkspaceAddChartDrawing(workspace,id,"range",a,b));OK(UmiTradingWorkspaceAddChartDrawing(workspace,id,"ray",a,b));
        OK(UmiTradingChartBuildScene(workspace,&info,&scene));CHECK(umi_chart_render_scene_count(scene)==count+2);umi_chart_render_scene_destroy(scene);
    }else return 2;
    OK(umi_trading_workspace_snapshot(workspace,&after));CHECK(after.order_count==before.order_count&&after.environment==before.environment&&after.live_armed==before.live_armed);
    CHECK(strcmp(after.selected_instrument_id,before.selected_instrument_id)==0&&strcmp(after.selected_order_id,before.selected_order_id)==0);
    CHECK(memcmp(&after.draft_order,&before.draft_order,sizeof(before.draft_order))==0);
    umi_trading_workspace_destroy(workspace);return 0;
}
