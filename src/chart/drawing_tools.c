/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/drawing_tools.c
 * PURPOSE: Validate tool semantics and clip lines, rays and boxes before rendering.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/drawing_tools.h"
#include "umicom/chart/drawing_appearance.h"
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

const char *UmiChartDrawingKindName(UmiChartDrawingKind kind)
{
    switch(kind){
    case UMI_CHART_DRAWING_TREND:return "trend";
    case UMI_CHART_DRAWING_SUPPORT:return "support";
    case UMI_CHART_DRAWING_RESISTANCE:return "resistance";
    case UMI_CHART_DRAWING_RANGE:return "range";
    case UMI_CHART_DRAWING_LIQUIDITY_ZONE:return "liquidity-zone";
    case UMI_CHART_DRAWING_RAY:return "ray";
    default:return NULL;
    }
}
UmiStatus UmiChartDrawingKindParse(const char *name,UmiChartDrawingKind *outKind)
{
    if(name==NULL||outKind==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    for(int i=UMI_CHART_DRAWING_TREND;i<=UMI_CHART_DRAWING_RAY;++i){
        if(strcmp(name,UmiChartDrawingKindName((UmiChartDrawingKind)i))==0){*outKind=(UmiChartDrawingKind)i;return UMI_STATUS_OK;}
    }
    return UMI_STATUS_UNAVAILABLE;
}
UmiStatus UmiChartDrawingToolValidate(const UmiChartDrawingSnapshot *drawing)
{
    UmiStatus status=UmiChartDrawingValidateGeometry(drawing);
    if(status!=UMI_STATUS_OK)return status;
    UmiChartDrawingKind kind;
    status=UmiChartDrawingKindParse(drawing->tool,&kind);
    if(status!=UMI_STATUS_OK)return status;
    if(drawing->time1<0||drawing->time2<0)return UMI_STATUS_INVALID_ARGUMENT;
    if(kind==UMI_CHART_DRAWING_SUPPORT||kind==UMI_CHART_DRAWING_RESISTANCE)
        return drawing->value1==drawing->value2?UMI_STATUS_OK:UMI_STATUS_INVALID_ARGUMENT;
    if(drawing->time1==drawing->time2)return UMI_STATUS_INVALID_ARGUMENT;
    if((kind==UMI_CHART_DRAWING_RANGE||kind==UMI_CHART_DRAWING_LIQUIDITY_ZONE)&&drawing->value1==drawing->value2)
        return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
UmiStatus UmiChartDrawingInitialize(const char *id,const char *pane,UmiChartDrawingKind kind,
    UmiChartPoint first,UmiChartPoint second,UmiChartDrawingSnapshot *outDrawing)
{
    const char *tool=UmiChartDrawingKindName(kind);
    if(id==NULL||pane==NULL||tool==NULL||outDrawing==NULL||!isfinite(first.value)||!isfinite(second.value))
        return UMI_STATUS_INVALID_ARGUMENT;
    UmiChartDrawingSnapshot result={0};
    if(strlen(id)>=sizeof(result.id)||strlen(pane)>=sizeof(result.pane_id))return UMI_STATUS_CAPACITY_EXCEEDED;
    strcpy(result.id,id);strcpy(result.pane_id,pane);strcpy(result.tool,tool);
    result.time1=first.time_ms;result.time2=second.time_ms;result.value1=first.value;
    result.value2=kind==UMI_CHART_DRAWING_SUPPORT||kind==UMI_CHART_DRAWING_RESISTANCE?first.value:second.value;
    UmiStatus status=UmiChartDrawingToolValidate(&result);
    if(status==UMI_STATUS_OK)*outDrawing=result;
    return status;
}

/* Compute integer time differences before floating-point conversion, retaining
 * small windows near INT64_MAX even when long double is only double precision.
 * Unsigned subtraction also handles a viewport crossing the Unix epoch without
 * signed overflow. Sloped segments are clipped before their pixel projection. */
static long double DrawingTimeDelta(int64_t time,int64_t origin)
{
    return time>=origin?(long double)((uint64_t)time-(uint64_t)origin):
        -(long double)((uint64_t)origin-(uint64_t)time);
}
static int DrawingViewportValid(const UmiChartPlotViewport *v)
{
    return v!=NULL&&v->end_ms>=v->start_ms&&isfinite(v->minimum_value)&&isfinite(v->maximum_value)&&
        v->maximum_value>v->minimum_value&&isfinite((long double)v->maximum_value-v->minimum_value)&&
        isfinite(v->area.x)&&isfinite(v->area.y)&&isfinite(v->area.width)&&isfinite(v->area.height)&&
        v->area.width>0&&v->area.height>0&&isfinite(v->area.x+v->area.width)&&isfinite(v->area.y+v->area.height);
}
static UmiChartRenderPoint DrawingPoint(const UmiChartPlotViewport *v,long double timeOffset,long double value)
{
    long double x=v->end_ms==v->start_ms?0.5L:timeOffset/DrawingTimeDelta(v->end_ms,v->start_ms);
    long double y=((long double)v->maximum_value-value)/((long double)v->maximum_value-v->minimum_value);
    x=fmaxl(0,fminl(1,x));y=fmaxl(0,fminl(1,y));
    return (UmiChartRenderPoint){v->area.x+(double)x*v->area.width,v->area.y+(double)y*v->area.height};
}
UmiStatus UmiChartDrawingProject(const UmiChartDrawingSnapshot *drawing,
    const UmiChartPlotViewport *v,UmiChartDrawingGeometry *outGeometry)
{
    if(outGeometry==NULL||!DrawingViewportValid(v))return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=UmiChartDrawingToolValidate(drawing);
    if(status!=UMI_STATUS_OK)return status;
    UmiChartDrawingGeometry g={0};(void)UmiChartDrawingKindParse(drawing->tool,&g.kind);
    /* All renderers and hit-test consumers share the same hidden geometry.
     * The canonical drawing remains available for object lists and persistence. */
    if ((drawing->visibility_flags & UMI_CHART_DRAWING_VISIBILITY_HIDDEN) != 0U) {
        *outGeometry = g;
        return UMI_STATUS_OK;
    }
    if(g.kind==UMI_CHART_DRAWING_SUPPORT||g.kind==UMI_CHART_DRAWING_RESISTANCE){
        if(drawing->value1>=v->minimum_value&&drawing->value1<=v->maximum_value){
            g.visible=1;g.first=DrawingPoint(v,0,drawing->value1);g.last=DrawingPoint(v,DrawingTimeDelta(v->end_ms,v->start_ms),drawing->value1);
            g.first.x=v->area.x;g.last.x=v->area.x+v->area.width;
        }
    }else if(g.kind==UMI_CHART_DRAWING_RANGE||g.kind==UMI_CHART_DRAWING_LIQUIDITY_ZONE){
        int64_t left=drawing->time1<drawing->time2?drawing->time1:drawing->time2;
        int64_t right=drawing->time1>drawing->time2?drawing->time1:drawing->time2;
        if(left<v->start_ms)left=v->start_ms;
        if(right>v->end_ms)right=v->end_ms;
        double low=fmax(fmin(drawing->value1,drawing->value2),v->minimum_value);
        double high=fmin(fmax(drawing->value1,drawing->value2),v->maximum_value);
        if(left<=right&&low<high){
            g.first=DrawingPoint(v,DrawingTimeDelta(left,v->start_ms),high);g.last=DrawingPoint(v,DrawingTimeDelta(right,v->start_ms),low);
            if(v->start_ms==v->end_ms){g.first.x=v->area.x;g.last.x=v->area.x+v->area.width;}
            g.rectangle=(UmiChartRenderRectangle){g.first.x,g.first.y,g.last.x-g.first.x,g.last.y-g.first.y};
            g.visible=g.rectangle.width>0&&g.rectangle.height>0;
        }
    }else{
        long double x=DrawingTimeDelta(drawing->time1,v->start_ms),y=drawing->value1;
        long double dx=DrawingTimeDelta(drawing->time2,drawing->time1),dy=(long double)drawing->value2-drawing->value1;
        if(!isfinite(dx)||!isfinite(dy))return UMI_STATUS_INVALID_ARGUMENT;
        long double p[4]={-dx,dx,-dy,dy};
        long double q[4]={x,DrawingTimeDelta(v->end_ms,v->start_ms)-x,y-v->minimum_value,v->maximum_value-y};
        long double first=0,last=g.kind==UMI_CHART_DRAWING_RAY?LDBL_MAX:1;
        int visible=1;
        for(size_t i=0;i<4;++i){
            if(!isfinite(q[i]))return UMI_STATUS_INVALID_ARGUMENT;
            if(p[i]==0){if(q[i]<0){visible=0;break;}continue;}
            long double ratio=q[i]/p[i];
            if(p[i]<0){if(ratio>last){visible=0;break;}if(ratio>first)first=ratio;}
            else{if(ratio<first){visible=0;break;}if(ratio<last)last=ratio;}
        }
        if(visible){
            long double ax=x+first*dx,ay=y+first*dy,bx=x+last*dx,by=y+last*dy;
            if(!isfinite(ax)||!isfinite(ay)||!isfinite(bx)||!isfinite(by))return UMI_STATUS_INVALID_ARGUMENT;
            g.first=DrawingPoint(v,ax,ay);g.last=DrawingPoint(v,bx,by);g.visible=1;
        }
    }
    *outGeometry=g;return UMI_STATUS_OK;
}
/* Rendering now resolves appearance through Framework so every graphics adapter receives the same per-object colour, width and fill. Existing default colours and widths remain exact, and capacity is checked before publishing any command. The previous implementation remains for engineering review. */
#if 0
UmiStatus UmiChartDrawingRender(UmiChartRenderScene *scene,const UmiChartDrawingSnapshot *drawing,
    const UmiChartPlotViewport *viewport,const UmiChartPlotStyle *style)
{
    if(scene==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=umi_chart_plot_validate(viewport,style);
    if(status!=UMI_STATUS_OK)return status;
    UmiChartDrawingGeometry g;status=UmiChartDrawingProject(drawing,viewport,&g);
    if(status!=UMI_STATUS_OK||!g.visible)return status;
    int level=g.kind==UMI_CHART_DRAWING_SUPPORT||g.kind==UMI_CHART_DRAWING_RESISTANCE;
    size_t needed=level||g.kind==UMI_CHART_DRAWING_LIQUIDITY_ZONE?2U:1U;
    size_t count=umi_chart_render_scene_count(scene),capacity=umi_chart_render_scene_capacity(scene);
    if(count>capacity||needed>capacity-count)return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiChartColor color=g.kind==UMI_CHART_DRAWING_SUPPORT?style->positive_color:
        g.kind==UMI_CHART_DRAWING_RESISTANCE?style->negative_color:(UmiChartColor){0.98,0.72,0.20,1};
    if(g.kind==UMI_CHART_DRAWING_LIQUIDITY_ZONE){
        UmiChartColor fill={0.22,0.65,0.9,0.16};color=(UmiChartColor){0.22,0.65,0.9,1};
        status=umi_chart_render_scene_add_filled_rectangle(scene,g.rectangle,fill);
    }
    if(status==UMI_STATUS_OK){
        if(g.kind==UMI_CHART_DRAWING_RANGE||g.kind==UMI_CHART_DRAWING_LIQUIDITY_ZONE)
            status=umi_chart_render_scene_add_stroked_rectangle(scene,g.rectangle,color,1.4);
        else status=umi_chart_render_scene_add_line(scene,g.first,g.last,color,level?1.4:2.0);
    }
    if(status==UMI_STATUS_OK&&level){
        char label[96];(void)snprintf(label,sizeof(label),"%s %.8g",drawing->tool,drawing->value1);
        status=umi_chart_render_scene_add_text(scene,(UmiChartRenderPoint){g.first.x+8,g.first.y-4},label,color);
    }
    return status;
}
#endif
UmiStatus UmiChartDrawingRender(UmiChartRenderScene *scene,const UmiChartDrawingSnapshot *drawing,
    const UmiChartPlotViewport *viewport,const UmiChartPlotStyle *style)
{
    if (scene == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_chart_plot_validate(viewport, style);
    if (status != UMI_STATUS_OK) return status;
    UmiChartDrawingGeometry g;
    status = UmiChartDrawingProject(drawing, viewport, &g);
    if (status != UMI_STATUS_OK || !g.visible) return status;
    UmiChartDrawingResolvedAppearance appearance;
    status = UmiChartDrawingAppearanceResolve(drawing, style, &appearance);
    if (status != UMI_STATUS_OK) return status;
    int level = g.kind == UMI_CHART_DRAWING_SUPPORT || g.kind == UMI_CHART_DRAWING_RESISTANCE;
    int box = g.kind == UMI_CHART_DRAWING_RANGE || g.kind == UMI_CHART_DRAWING_LIQUIDITY_ZONE;
    int filled = box && appearance.fill.alpha > 0;
    size_t needed = 1U + (size_t)level + (size_t)filled;
    size_t count = umi_chart_render_scene_count(scene), capacity = umi_chart_render_scene_capacity(scene);
    if (count > capacity || needed > capacity - count) return UMI_STATUS_CAPACITY_EXCEEDED;
    if (filled) status = umi_chart_render_scene_add_filled_rectangle(scene, g.rectangle, appearance.fill);
    if (status == UMI_STATUS_OK) {
        if (box) status = umi_chart_render_scene_add_stroked_rectangle(scene, g.rectangle, appearance.outline, appearance.width);
        else status = umi_chart_render_scene_add_line(scene, g.first, g.last, appearance.outline, appearance.width);
    }
    if (status == UMI_STATUS_OK && level) {
        char label[96];
        (void)snprintf(label, sizeof label, "%s %.8g", drawing->tool, drawing->value1);
        status = umi_chart_render_scene_add_text(scene, (UmiChartRenderPoint){g.first.x + 8, g.first.y - 4}, label, appearance.outline);
    }
    return status;
}
