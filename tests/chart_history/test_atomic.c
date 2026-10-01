/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_history/test_atomic.c
 * PURPOSE: Verify candidate rollback, pane scope, ordering, reentry and bounded memory.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
static UmiStatus FailAfterWrite(UmiChartDrawingRegistry *r,void *data)
{ OK(umi_chart_drawing_registry_upsert(r,data)); return UMI_STATUS_IO_ERROR; }
static UmiStatus Reenter(UmiChartDrawingRegistry *r,void *data)
{
    (void)r; Fixture *f=data; UmiChartDrawingHistorySnapshot s;
    CHECK(UmiChartDrawingHistoryRead(f->history,&s)==UMI_STATUS_BUSY);
    CHECK(UmiChartDrawingHistoryReset(f->history)==UMI_STATUS_BUSY);
    CHECK(UmiChartDrawingHistoryUndo(f->history,"pane",1)==UMI_STATUS_BUSY);
    return UMI_STATUS_CANCELLED;
}
static UmiStatus Reorder(UmiChartDrawingRegistry *r,void *data)
{
    (void)data; UmiChartDrawingSnapshot a; OK(umi_chart_drawing_registry_find(r,"a",&a));
    OK(umi_chart_drawing_registry_remove(r,"a")); OK(umi_chart_drawing_registry_upsert(r,&a));
    UmiChartDrawingSnapshot c=Drawing("c","pane"); return umi_chart_drawing_registry_upsert(r,&c);
}
static UmiStatus Bulk(UmiChartDrawingRegistry *r,void *data)
{
    int replace=*(int *)data;
    size_t count=umi_chart_drawing_registry_count(r);
    for (size_t i=0;i<count;++i) {
        UmiChartDrawingSnapshot row; OK(umi_chart_drawing_registry_at(r,replace?0:i,&row));
        if (replace) OK(umi_chart_drawing_registry_remove(r,row.id));
        else { row.locked=!row.locked; OK(umi_chart_drawing_registry_upsert(r,&row)); }
    }
    if (replace) for (size_t i=0;i<count;++i) {
        char id[128]; (void)snprintf(id,sizeof id,"new-%zu",i); UmiChartDrawingSnapshot row=Drawing(id,"pane");
        OK(umi_chart_drawing_registry_upsert(r,&row));
    }
    return UMI_STATUS_OK;
}
int main(int argc,char **argv)
{
    CHECK(argc==2); const char *name=argv[1]; Fixture f=Open(); UmiChartDrawingSnapshot a=Drawing("a","pane"), b=Drawing("b","pane");
    Apply(&f,&a); UmiChartDrawingHistorySnapshot before=Read(&f); int changed=42;
    if (strcmp(name,"failed-callback")==0) {
        CHECK(UmiChartDrawingHistoryApply(f.history,"pane","Fail",FailAfterWrite,&b,&changed)==UMI_STATUS_IO_ERROR);
        CHECK(!changed && Read(&f).revision==before.revision && umi_chart_drawing_registry_count(f.registry)==1);
    } else if (strcmp(name,"failed-redo")==0) {
        Apply(&f,&b); Undo(&f,"pane"); before=Read(&f);
        CHECK(UmiChartDrawingHistoryApply(f.history,"pane","Fail",FailAfterWrite,&b,&changed)==UMI_STATUS_IO_ERROR);
        CHECK(!changed && Read(&f).revision==before.revision && Read(&f).redo_count==1);
        Redo(&f,"pane"); CHECK(umi_chart_drawing_registry_count(f.registry)==2);
    } else if (strcmp(name,"wrong-pane")==0) {
        strcpy(b.pane_id,"other"); CHECK(UmiChartDrawingHistoryApply(f.history,"pane","Wrong",Upsert,&b,&changed)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!changed && !Read(&f).stale && Read(&f).registry_revision==before.registry_revision);
    } else if (strcmp(name,"reentry")==0) {
        CHECK(UmiChartDrawingHistoryApply(f.history,"pane","Reenter",Reenter,&f,&changed)==UMI_STATUS_CANCELLED); CHECK(!changed && Read(&f).revision==before.revision);
    } else if (strcmp(name,"unchanged-order")==0) {
        Apply(&f,&b); before=Read(&f);
        CHECK(UmiChartDrawingHistoryApply(f.history,"pane","Reorder",Reorder,NULL,&changed)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!changed && Read(&f).registry_revision==before.registry_revision && umi_chart_drawing_registry_count(f.registry)==2);
    } else if (strcmp(name,"invalid-arguments")==0) {
        char longLabel[96]; memset(longLabel,'x',sizeof longLabel);
        CHECK(UmiChartDrawingHistoryApply(f.history,"pane",longLabel,Upsert,&b,&changed)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!changed); CHECK(UmiChartDrawingHistoryApply(f.history,"", "Edit",Upsert,&b,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiChartDrawingHistoryRead(NULL,&before)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiChartDrawingHistoryCreate(NULL,NULL)==UMI_STATUS_INVALID_ARGUMENT);
    } else if (strcmp(name,"full-registry")==0 || strcmp(name,"oversize-step")==0 || strcmp(name,"byte-budget")==0) {
        for (size_t i=1;i<UMI_CHART_DRAWING_CAPACITY;++i) {
            char id[128]; (void)snprintf(id,sizeof id,"row-%zu",i); UmiChartDrawingSnapshot row=Drawing(id,"pane");
            OK(umi_chart_drawing_registry_upsert(f.registry,&row));
        }
        OK(UmiChartDrawingHistoryReset(f.history)); before=Read(&f);
        if (strcmp(name,"full-registry")==0) {
            CHECK(UmiChartDrawingHistoryApply(f.history,"pane","Full",Upsert,&b,&changed)==UMI_STATUS_CAPACITY_EXCEEDED); CHECK(!changed);
        } else if (strcmp(name,"oversize-step")==0) {
            int replace=1; CHECK(UmiChartDrawingHistoryApply(f.history,"pane","Replace",Bulk,&replace,&changed)==UMI_STATUS_CAPACITY_EXCEEDED);
            CHECK(!changed && Read(&f).registry_revision==before.registry_revision && Find(&f,"a").value1==10);
        } else {
            int replace=0; OK(UmiChartDrawingHistoryApply(f.history,"pane","Lock all",Bulk,&replace,&changed)); CHECK(changed);
            OK(UmiChartDrawingHistoryApply(f.history,"pane","Unlock all",Bulk,&replace,&changed)); CHECK(changed);
            CHECK(Read(&f).undo_count==1 && Read(&f).retained_bytes<=UMI_CHART_DRAWING_HISTORY_BYTES);
            Undo(&f,"pane"); CHECK(Find(&f,"a").locked); Redo(&f,"pane"); CHECK(!Find(&f,"a").locked);
        }
    } else return 2;
    Close(&f); return 0;
}
