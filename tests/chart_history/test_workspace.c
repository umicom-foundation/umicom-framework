/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_history/test_workspace.c
 * PURPOSE: Qualify canonical drawing commands, persistence barriers and trading-state isolation.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../trading_execution/order_review_fixture.h"
#include "umicom/trading/chart_history.h"
#include "umicom/trading/chart_persistence.h"
#define CHECK REVIEW_CHECK
#define OK(x) CHECK((x)==UMI_STATUS_OK)
int main(int argc,char **argv)
{
    CHECK(argc==2); const char *name=argv[1]; ReviewFixture f; ReviewFixtureInit(&f);
    UmiTradingWorkspaceSnapshot before,after; OK(umi_trading_workspace_snapshot(f.workspace,&before));
    const char *pane=before.selected_instrument_id;
    UmiChartDrawingHistorySnapshot history; OK(UmiTradingWorkspaceDrawingHistory(f.workspace,&history)); CHECK(!history.undo_count);
    OK(UmiTradingWorkspaceAddChartDrawing(f.workspace,pane,"range",(UmiChartPoint){100,10},(UmiChartPoint){200,20}));
    UmiChartDrawingRegistry *registry=umi_chart_workspace_drawings(umi_trading_workspace_charts(f.workspace));
    UmiChartDrawingSnapshot drawing,current; OK(umi_chart_drawing_registry_at(registry,0,&drawing));
    if (strcmp(name,"failed-first")==0) {
        UmiTradingWorkspace *empty=NULL; OK(umi_trading_workspace_create(NULL,&empty));
        UmiInstrument instrument=test_instrument(); OK(umi_trading_workspace_add_instrument(empty,&instrument));
        OK(umi_trading_workspace_select_instrument(empty,instrument.instrument_id.value));
        UmiTradingWorkspaceSnapshot initial; OK(umi_trading_workspace_snapshot(empty,&initial));
        OK(UmiTradingWorkspaceDrawingHistory(empty,&history)); uint64_t revision=history.revision;
        CHECK(UmiTradingWorkspaceRemoveChartDrawing(empty,initial.selected_instrument_id,"missing",0)!=UMI_STATUS_OK);
        OK(UmiTradingWorkspaceDrawingHistory(empty,&history)); CHECK(history.revision==revision && !history.undo_count);
        umi_trading_workspace_destroy(empty);
    } else if (strcmp(name,"no-op")==0) {
        OK(UmiTradingWorkspaceDrawingHistory(f.workspace,&history)); uint64_t revision=history.revision;
        UmiTradingWorkspaceSnapshot same; OK(umi_trading_workspace_snapshot(f.workspace,&same));
        OK(UmiTradingWorkspaceSetChartDrawingLocked(f.workspace,pane,drawing.id,drawing.revision,0));
        OK(UmiTradingWorkspaceDrawingHistory(f.workspace,&history)); CHECK(history.revision==revision && history.undo_count==1);
        OK(umi_trading_workspace_snapshot(f.workspace,&after)); CHECK(after.revision==same.revision);
    } else if (strcmp(name,"move")==0) OK(UmiTradingWorkspaceMoveChartDrawing(f.workspace,pane,drawing.id,drawing.revision,(UmiChartPoint){300,30},(UmiChartPoint){400,40}));
    else if (strcmp(name,"lock")==0) OK(UmiTradingWorkspaceSetChartDrawingLocked(f.workspace,pane,drawing.id,drawing.revision,1));
    else if (strcmp(name,"hidden")==0) OK(UmiTradingWorkspaceSetChartDrawingHidden(f.workspace,pane,drawing.id,drawing.revision,1));
    else if (strcmp(name,"appearance")==0) { UmiChartDrawingAppearance style={18,52,86,25,30}; OK(UmiTradingWorkspaceSetChartDrawingAppearance(f.workspace,pane,drawing.id,drawing.revision,&style)); }
    else if (strcmp(name,"duplicate")==0) { char id[128]; OK(UmiTradingWorkspaceDuplicateChartDrawing(f.workspace,pane,drawing.id,drawing.revision,id,sizeof id)); CHECK(strcmp(id,drawing.id)!=0); }
    else if (strcmp(name,"remove")==0) OK(UmiTradingWorkspaceRemoveChartDrawing(f.workspace,pane,drawing.id,drawing.revision));
    else if (strcmp(name,"pane-hidden")==0) {
        OK(UmiTradingWorkspaceAddChartDrawing(f.workspace,pane,"support",(UmiChartPoint){100,10},(UmiChartPoint){200,20}));
        size_t changed=0; OK(UmiTradingWorkspaceSetChartDrawingsHidden(f.workspace,pane,umi_chart_drawing_registry_revision(registry),1,&changed)); CHECK(changed==2);
    } else if (strcmp(name,"failed-edit")==0) {
        char id[1]={'x'}; OK(UmiTradingWorkspaceDrawingHistory(f.workspace,&history)); uint64_t revision=history.revision;
        CHECK(UmiTradingWorkspaceDuplicateChartDrawing(f.workspace,pane,drawing.id,drawing.revision,id,sizeof id)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(id[0]=='x'); size_t changed=99;
        CHECK(UmiTradingWorkspaceSetChartDrawingsHidden(f.workspace,pane,0,1,&changed)==UMI_STATUS_BUSY && changed==99);
        CHECK(UmiTradingWorkspaceMoveChartDrawing(f.workspace,pane,drawing.id,drawing.revision-1,(UmiChartPoint){300,30},(UmiChartPoint){400,40})==UMI_STATUS_BUSY);
        OK(UmiTradingWorkspaceDrawingHistory(f.workspace,&history)); CHECK(history.revision==revision && history.undo_count==1);
    } else if (strcmp(name,"selected-instrument")==0) {
        UmiInstrument other=test_instrument(); OK(UmiTradingWorkspaceDrawingHistory(f.workspace,&history));
        CHECK(UmiTradingWorkspaceUndoDrawing(f.workspace,other.instrument_id.value,history.revision)==UMI_STATUS_INVALID_STATE);
        OK(umi_trading_workspace_select_instrument(f.workspace,other.instrument_id.value));
        CHECK(UmiTradingWorkspaceUndoDrawing(f.workspace,pane,history.revision)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiTradingWorkspaceUndoDrawing(f.workspace,other.instrument_id.value,history.revision)==UMI_STATUS_INVALID_STATE);
        OK(umi_trading_workspace_select_instrument(f.workspace,pane));
        OK(umi_trading_workspace_snapshot(f.workspace,&before));
    } else if (strcmp(name,"restore-barrier")==0 || strcmp(name,"save-redo")==0 || strcmp(name,"stale-preview")==0) {
        UmiDataServer *server=NULL; UmiTradingChartPersistence *service=NULL;
        OK(umi_data_server_create_memory(&server)); OK(UmiTradingChartPersistenceCreate(f.workspace,&service)); OK(UmiTradingChartPersistenceBind(service,server,"history"));
        UmiChartCheckpointReport report; UmiTradingChartPreview preview;
        OK(UmiTradingChartPersistenceSave(service,pane,1000,&report)); OK(UmiTradingChartPersistencePreview(service,pane,&preview));
        OK(UmiTradingWorkspaceDrawingHistory(f.workspace,&history)); OK(UmiTradingWorkspaceUndoDrawing(f.workspace,pane,history.revision));
        OK(UmiTradingWorkspaceDrawingHistory(f.workspace,&history)); CHECK(history.redo_count==1);
        if (strcmp(name,"save-redo")==0) {
            OK(UmiTradingChartPersistenceSave(service,pane,2000,&report)); OK(UmiTradingWorkspaceDrawingHistory(f.workspace,&history)); CHECK(history.redo_count==1);
            OK(UmiTradingWorkspaceRedoDrawing(f.workspace,pane,history.revision));
        } else {
            CHECK(UmiTradingChartPersistenceRestore(service,pane,preview.preview_id)==UMI_STATUS_INVALID_STATE);
            OK(UmiTradingWorkspaceDrawingHistory(f.workspace,&history)); CHECK(history.redo_count==1);
            if (strcmp(name,"restore-barrier")==0) {
                OK(UmiTradingChartPersistencePreview(service,pane,&preview)); OK(UmiTradingChartPersistenceRestore(service,pane,preview.preview_id));
                OK(UmiTradingWorkspaceDrawingHistory(f.workspace,&history)); CHECK(!history.undo_count && !history.redo_count && !history.stale);
                CHECK(umi_chart_drawing_registry_count(registry)==1);
            }
        }
        UmiTradingChartPersistenceDestroy(service); umi_data_server_destroy(server);
    } else return 2;
    if (strcmp(name,"move")==0 || strcmp(name,"lock")==0 || strcmp(name,"hidden")==0 || strcmp(name,"appearance")==0 ||
        strcmp(name,"duplicate")==0 || strcmp(name,"remove")==0 || strcmp(name,"pane-hidden")==0) {
        size_t count=umi_chart_drawing_registry_count(registry);
        OK(UmiTradingWorkspaceDrawingHistory(f.workspace,&history)); OK(UmiTradingWorkspaceUndoDrawing(f.workspace,pane,history.revision));
        OK(umi_chart_drawing_registry_find(registry,drawing.id,&current));
        CHECK(current.time1==100 && current.value1==10 && !current.locked && !current.visibility_flags && current.style[0]=='\0');
        OK(UmiTradingWorkspaceDrawingHistory(f.workspace,&history)); OK(UmiTradingWorkspaceRedoDrawing(f.workspace,pane,history.revision));
        CHECK(umi_chart_drawing_registry_count(registry)==count);
        if (strcmp(name,"remove")!=0) {
            OK(umi_chart_drawing_registry_find(registry,drawing.id,&current));
            if (strcmp(name,"move")==0) CHECK(current.time1==300 && current.value1==30);
            if (strcmp(name,"lock")==0) CHECK(current.locked);
            if (strcmp(name,"hidden")==0 || strcmp(name,"pane-hidden")==0) CHECK(current.visibility_flags==1);
            if (strcmp(name,"appearance")==0) CHECK(strcmp(current.style,"umi-drawing:1:123456:25:30")==0);
        }
    }
    OK(umi_trading_workspace_snapshot(f.workspace,&after));
    CHECK(after.order_count==before.order_count && after.live_armed==before.live_armed && after.environment==before.environment);
    CHECK(strcmp(after.selected_order_id,before.selected_order_id)==0 && memcmp(&after.draft_order,&before.draft_order,sizeof before.draft_order)==0);
    umi_trading_workspace_destroy(f.workspace); return 0;
}
