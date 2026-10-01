/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_tools/test_edit.c
 * PURPOSE: Verify protected edits preserve locks, source records, styles and failed-operation state.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/drawing_edit.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);exit(1);}}while(0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];UmiChartDrawingRegistry *registry=NULL;
    OK(umi_chart_drawing_registry_create(&registry));UmiChartDrawingSnapshot source,after;
    OK(UmiChartDrawingInitialize("source","pane",UMI_CHART_DRAWING_LIQUIDITY_ZONE,(UmiChartPoint){1,20},(UmiChartPoint){2,30},&source));
    strcpy(source.style,"user style; caf\xc3\xa9");source.selected=1;
    OK(umi_chart_drawing_registry_upsert(registry,&source));OK(umi_chart_drawing_registry_find(registry,"source",&source));
    uint64_t revision=umi_chart_drawing_registry_revision(registry);
    if(strcmp(name,"lock")==0){
        OK(UmiChartDrawingSetLocked(registry,"pane","source",source.revision,0));CHECK(umi_chart_drawing_registry_revision(registry)==revision);
        OK(UmiChartDrawingSetLocked(registry,"pane","source",source.revision,1));OK(umi_chart_drawing_registry_find(registry,"source",&after));
        CHECK(after.locked);CHECK(UmiChartDrawingSetGeometry(registry,"pane","source",after.revision,(UmiChartPoint){3,40},(UmiChartPoint){4,50})==UMI_STATUS_PERMISSION_DENIED);
        OK(UmiChartDrawingSetLocked(registry,"pane","source",after.revision,0));OK(umi_chart_drawing_registry_find(registry,"source",&after));CHECK(!after.locked);
    }else if(strcmp(name,"move")==0){
        OK(UmiChartDrawingSetGeometry(registry,"pane","source",source.revision,(UmiChartPoint){3,40},(UmiChartPoint){4,50}));
        OK(umi_chart_drawing_registry_find(registry,"source",&after));CHECK(after.time1==3&&after.time2==4&&after.value1==40&&after.value2==50);
        CHECK(strcmp(after.style,source.style)==0&&after.selected&&strcmp(after.id,"source")==0);
        revision=umi_chart_drawing_registry_revision(registry);OK(UmiChartDrawingSetGeometry(registry,"pane","source",after.revision,(UmiChartPoint){3,40},(UmiChartPoint){4,50}));
        CHECK(umi_chart_drawing_registry_revision(registry)==revision);
    }else if(strcmp(name,"duplicate")==0){
        OK(UmiChartDrawingSetLocked(registry,"pane","source",source.revision,1));OK(umi_chart_drawing_registry_find(registry,"source",&source));
        OK(UmiChartDrawingDuplicate(registry,"pane","source",source.revision,"copy"));OK(umi_chart_drawing_registry_find(registry,"copy",&after));
        CHECK(!after.locked&&!after.selected&&strcmp(after.style,source.style)==0&&after.time1==source.time1&&after.value2==source.value2);
        OK(umi_chart_drawing_registry_find(registry,"source",&after));CHECK(after.locked&&after.selected&&after.revision==source.revision);
        CHECK(UmiChartDrawingDuplicate(registry,"pane","source",source.revision,"copy")==UMI_STATUS_ALREADY_EXISTS);
    }else if(strcmp(name,"stale")==0){
        OK(umi_chart_drawing_registry_remove(registry,"source"));OK(umi_chart_drawing_registry_upsert(registry,&source));revision=umi_chart_drawing_registry_revision(registry);
        CHECK(UmiChartDrawingSetLocked(registry,"pane","source",source.revision,1)==UMI_STATUS_BUSY);
        CHECK(UmiChartDrawingSetGeometry(registry,"pane","source",source.revision,(UmiChartPoint){3,40},(UmiChartPoint){4,50})==UMI_STATUS_BUSY);
        CHECK(UmiChartDrawingDuplicate(registry,"pane","source",source.revision,"copy")==UMI_STATUS_BUSY);
        CHECK(umi_chart_drawing_registry_revision(registry)==revision&&umi_chart_drawing_registry_count(registry)==1);
    }else if(strcmp(name,"wrong-pane")==0){
        CHECK(UmiChartDrawingSetLocked(registry,"other","source",source.revision,1)==UMI_STATUS_INVALID_STATE);
        CHECK(UmiChartDrawingDuplicate(registry,"other","source",source.revision,"copy")==UMI_STATUS_INVALID_STATE);
        CHECK(umi_chart_drawing_registry_revision(registry)==revision);
    }else if(strcmp(name,"invalid")==0){
        CHECK(UmiChartDrawingSetLocked(registry,"pane","source",source.revision,2)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiChartDrawingSetGeometry(registry,"pane","source",source.revision,(UmiChartPoint){1,20},(UmiChartPoint){1,30})==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiChartDrawingDuplicate(registry,"pane","source",source.revision,"")==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_chart_drawing_registry_revision(registry)==revision);OK(umi_chart_drawing_registry_find(registry,"source",&after));CHECK(after.time2==2&&after.value2==30);
    }else if(strcmp(name,"capacity")==0){
        for(size_t i=1;i<UMI_CHART_DRAWING_CAPACITY;++i){UmiChartDrawingSnapshot item=source;(void)snprintf(item.id,sizeof(item.id),"other.%zu",i);OK(umi_chart_drawing_registry_upsert(registry,&item));}
        revision=umi_chart_drawing_registry_revision(registry);
        CHECK(UmiChartDrawingDuplicate(registry,"pane","source",source.revision,"copy")==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_chart_drawing_registry_revision(registry)==revision&&umi_chart_drawing_registry_count(registry)==UMI_CHART_DRAWING_CAPACITY);
    }else return 2;
    umi_chart_drawing_registry_destroy(registry);return 0;
}
