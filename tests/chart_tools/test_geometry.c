/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_tools/test_geometry.c
 * PURPOSE: Verify independent geometric expectations for clipped lines, rays, boxes and scene publication.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/drawing_tools.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x);exit(1);}}while(0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
#define NEAR(a,b) CHECK(fabs((a)-(b))<1e-8)
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *name=argv[1];
    UmiChartPlotViewport v={{10,20,100,100},100,200,0,100};
    UmiChartDrawingSnapshot drawing;UmiChartDrawingGeometry g;
    OK(UmiChartDrawingInitialize("drawing","pane",UMI_CHART_DRAWING_RANGE,(UmiChartPoint){50,-50},(UmiChartPoint){150,50},&drawing));
    if(strcmp(name,"catalogue")==0){
        const char *names[]={"trend","support","resistance","range","liquidity-zone","ray"};
        for(size_t i=0;i<6;++i){UmiChartDrawingKind kind;OK(UmiChartDrawingKindParse(names[i],&kind));CHECK((int)kind==(int)i+1);CHECK(strcmp(UmiChartDrawingKindName(kind),names[i])==0);}
        UmiChartDrawingKind sentinel=UMI_CHART_DRAWING_RAY;CHECK(UmiChartDrawingKindParse("future",&sentinel)==UMI_STATUS_UNAVAILABLE&&sentinel==UMI_CHART_DRAWING_RAY);
        CHECK(UmiChartDrawingKindName((UmiChartDrawingKind)0)==NULL);
        OK(UmiChartDrawingInitialize("level","pane",UMI_CHART_DRAWING_SUPPORT,(UmiChartPoint){1,12},(UmiChartPoint){1,30},&drawing));
        CHECK(drawing.value1==12&&drawing.value2==12);
    }else if(strcmp(name,"invalid")==0){
        UmiChartDrawingSnapshot before=drawing;
        CHECK(UmiChartDrawingInitialize("x","pane",UMI_CHART_DRAWING_RANGE,(UmiChartPoint){1,2},(UmiChartPoint){1,3},&drawing)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&before,&drawing,sizeof(drawing))==0);
        CHECK(UmiChartDrawingInitialize("x","pane",UMI_CHART_DRAWING_RANGE,(UmiChartPoint){1,2},(UmiChartPoint){2,2},&drawing)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiChartDrawingInitialize("x","pane",UMI_CHART_DRAWING_RAY,(UmiChartPoint){-1,2},(UmiChartPoint){2,3},&drawing)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiChartDrawingInitialize("x","pane",UMI_CHART_DRAWING_SUPPORT,(UmiChartPoint){1,NAN},(UmiChartPoint){2,3},&drawing)==UMI_STATUS_INVALID_ARGUMENT);
        memset(&g,0x5a,sizeof(g));UmiChartDrawingGeometry untouched=g;v.area.width=INFINITY;
        CHECK(UmiChartDrawingProject(&drawing,&v,&g)==UMI_STATUS_INVALID_ARGUMENT&&memcmp(&g,&untouched,sizeof(g))==0);
    }else if(strcmp(name,"range")==0){
        OK(UmiChartDrawingProject(&drawing,&v,&g));CHECK(g.visible);NEAR(g.rectangle.x,10);NEAR(g.rectangle.y,70);NEAR(g.rectangle.width,50);NEAR(g.rectangle.height,50);
    }else if(strcmp(name,"reversed-zone")==0){
        OK(UmiChartDrawingInitialize("z","pane",UMI_CHART_DRAWING_LIQUIDITY_ZONE,(UmiChartPoint){175,75},(UmiChartPoint){125,25},&drawing));
        OK(UmiChartDrawingProject(&drawing,&v,&g));CHECK(g.visible);NEAR(g.rectangle.x,35);NEAR(g.rectangle.y,45);NEAR(g.rectangle.width,50);NEAR(g.rectangle.height,50);
    }else if(strcmp(name,"ray-right")==0||strcmp(name,"ray-left")==0||strcmp(name,"ray-away")==0){
        int left=strcmp(name,"ray-left")==0,away=strcmp(name,"ray-away")==0;
        OK(UmiChartDrawingInitialize("ray","pane",UMI_CHART_DRAWING_RAY,(UmiChartPoint){away?250:left?175:125,50},(UmiChartPoint){away?275:150,50},&drawing));
        OK(UmiChartDrawingProject(&drawing,&v,&g));CHECK(g.visible==!away);
        if(!away){NEAR(g.first.x,left?85:35);NEAR(g.last.x,left?10:110);NEAR(g.first.y,70);NEAR(g.last.y,70);}
    }else if(strcmp(name,"slope-clipping")==0){
        OK(UmiChartDrawingInitialize("line","pane",UMI_CHART_DRAWING_TREND,(UmiChartPoint){50,-100},(UmiChartPoint){250,100},&drawing));
        OK(UmiChartDrawingProject(&drawing,&v,&g));CHECK(g.visible);NEAR(g.first.x,60);NEAR(g.first.y,120);NEAR(g.last.x,110);NEAR(g.last.y,70);
    }else if(strcmp(name,"offscreen")==0){
        drawing.time1=201;drawing.time2=250;OK(UmiChartDrawingProject(&drawing,&v,&g));CHECK(!g.visible);
        drawing.time1=100;drawing.time2=200;drawing.value1=101;drawing.value2=120;OK(UmiChartDrawingProject(&drawing,&v,&g));CHECK(!g.visible);
    }else if(strcmp(name,"single-time")==0){
        v.start_ms=125;v.end_ms=125;OK(UmiChartDrawingProject(&drawing,&v,&g));CHECK(g.visible);NEAR(g.rectangle.x,10);NEAR(g.rectangle.width,100);
        OK(UmiChartDrawingInitialize("level","pane",UMI_CHART_DRAWING_SUPPORT,(UmiChartPoint){1,25},(UmiChartPoint){2,25},&drawing));
        OK(UmiChartDrawingProject(&drawing,&v,&g));NEAR(g.first.x,10);NEAR(g.last.x,110);
    }else if(strcmp(name,"extreme-time")==0){
        v.start_ms=INT64_MAX-100;v.end_ms=INT64_MAX;
        OK(UmiChartDrawingInitialize("far","pane",UMI_CHART_DRAWING_RANGE,(UmiChartPoint){INT64_MAX-75,25},(UmiChartPoint){INT64_MAX-25,75},&drawing));
        OK(UmiChartDrawingProject(&drawing,&v,&g));CHECK(g.visible);NEAR(g.rectangle.x,35);NEAR(g.rectangle.width,50);
    }else if(strcmp(name,"render-capacity")==0||strcmp(name,"render-theme")==0){
        UmiChartRenderScene *scene=NULL;UmiChartPlotStyle style;umi_chart_plot_style_dark(&style);
        OK(UmiChartDrawingInitialize("zone","pane",UMI_CHART_DRAWING_LIQUIDITY_ZONE,(UmiChartPoint){125,25},(UmiChartPoint){175,75},&drawing));
        int small=strcmp(name,"render-capacity")==0;OK(umi_chart_render_scene_create(small?1U:4U,&scene));
        if(small){CHECK(UmiChartDrawingRender(scene,&drawing,&v,&style)==UMI_STATUS_CAPACITY_EXCEEDED);CHECK(umi_chart_render_scene_count(scene)==0);}
        else{
            OK(UmiChartDrawingRender(scene,&drawing,&v,&style));CHECK(umi_chart_render_scene_count(scene)==2);
            UmiChartRenderCommand fill,border;OK(umi_chart_render_scene_at(scene,0,&fill));OK(umi_chart_render_scene_at(scene,1,&border));
            CHECK(fill.kind==UMI_CHART_RENDER_FILL_RECTANGLE&&fill.color.alpha>0&&fill.color.alpha<1&&border.kind==UMI_CHART_RENDER_STROKE_RECTANGLE);
            style.positive_color.red=NAN;CHECK(UmiChartDrawingRender(scene,&drawing,&v,&style)==UMI_STATUS_INVALID_ARGUMENT);CHECK(umi_chart_render_scene_count(scene)==2);
        }
        umi_chart_render_scene_destroy(scene);
    }else return 2;
    return 0;
}
