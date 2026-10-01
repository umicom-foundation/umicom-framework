/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/chart/drawing_edit.c
 * PURPOSE: Keep stale checks, locks and copy semantics independent of native chart widgets.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/chart/drawing_edit.h"
#include <string.h>
static UmiStatus EditableDrawing(UmiChartDrawingRegistry *registry,const char *pane,
    const char *id,uint64_t expected,UmiChartDrawingSnapshot *out)
{
    if(registry==NULL||pane==NULL||pane[0]=='\0'||id==NULL||id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status=umi_chart_drawing_registry_find(registry,id,out);
    if(status!=UMI_STATUS_OK)return status;
    if(strcmp(out->pane_id,pane)!=0)return UMI_STATUS_INVALID_STATE;
    if(out->revision!=expected)return UMI_STATUS_BUSY;
    return UmiChartDrawingToolValidate(out);
}
UmiStatus UmiChartDrawingSetLocked(UmiChartDrawingRegistry *registry,const char *pane,
    const char *id,uint64_t expectedRevision,int locked)
{
    if(locked!=0&&locked!=1)return UMI_STATUS_INVALID_ARGUMENT;
    UmiChartDrawingSnapshot drawing;UmiStatus status=EditableDrawing(registry,pane,id,expectedRevision,&drawing);
    if(status!=UMI_STATUS_OK||drawing.locked==locked)return status;
    drawing.locked=locked;return umi_chart_drawing_registry_upsert(registry,&drawing);
}
UmiStatus UmiChartDrawingSetGeometry(UmiChartDrawingRegistry *registry,const char *pane,
    const char *id,uint64_t expectedRevision,UmiChartPoint first,UmiChartPoint second)
{
    UmiChartDrawingSnapshot drawing;UmiStatus status=EditableDrawing(registry,pane,id,expectedRevision,&drawing);
    if(status!=UMI_STATUS_OK)return status;
    if(drawing.locked)return UMI_STATUS_PERMISSION_DENIED;
    UmiChartDrawingKind kind;(void)UmiChartDrawingKindParse(drawing.tool,&kind);
    UmiChartDrawingSnapshot geometry;
    status=UmiChartDrawingInitialize(drawing.id,drawing.pane_id,kind,first,second,&geometry);
    if(status!=UMI_STATUS_OK)return status;
    if(drawing.time1==geometry.time1&&drawing.time2==geometry.time2&&drawing.value1==geometry.value1&&drawing.value2==geometry.value2)
        return UMI_STATUS_OK;
    drawing.time1=geometry.time1;drawing.time2=geometry.time2;drawing.value1=geometry.value1;drawing.value2=geometry.value2;
    return umi_chart_drawing_registry_upsert(registry,&drawing);
}
UmiStatus UmiChartDrawingDuplicate(UmiChartDrawingRegistry *registry,const char *pane,
    const char *id,uint64_t expectedRevision,const char *newId)
{
    if(newId==NULL||newId[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    UmiChartDrawingSnapshot drawing,existing;UmiStatus status=EditableDrawing(registry,pane,id,expectedRevision,&drawing);
    if(status!=UMI_STATUS_OK)return status;
    if(strlen(newId)>=sizeof(drawing.id))return UMI_STATUS_CAPACITY_EXCEEDED;
    status=umi_chart_drawing_registry_find(registry,newId,&existing);
    if(status==UMI_STATUS_OK)return UMI_STATUS_ALREADY_EXISTS;
    if(status!=UMI_STATUS_NOT_FOUND)return status;
    strcpy(drawing.id,newId);drawing.locked=0;drawing.selected=0;drawing.revision=0;
    return umi_chart_drawing_registry_upsert(registry,&drawing);
}
