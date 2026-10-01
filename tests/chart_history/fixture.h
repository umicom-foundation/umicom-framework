/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_history/fixture.h
 * PURPOSE: Construct real drawing registries and atomic candidate operations for history regressions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_HISTORY_TEST_FIXTURE_H
#define UMICOM_CHART_HISTORY_TEST_FIXTURE_H
#include "umicom/chart/drawing_history.h"
#include "umicom/chart/drawing_edit.h"
#include "umicom/chart/drawing_visibility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"%s:%d: %s\n",__FILE__,__LINE__,#x); exit(1); } } while (0)
#define OK(x) CHECK((x)==UMI_STATUS_OK)
typedef struct Fixture { UmiChartDrawingRegistry *registry; UmiChartDrawingHistory *history; } Fixture;
static inline UmiChartDrawingSnapshot Drawing(const char *id, const char *pane)
{
    UmiChartDrawingSnapshot row;
    OK(UmiChartDrawingInitialize(id,pane,UMI_CHART_DRAWING_RANGE,(UmiChartPoint){100,10},(UmiChartPoint){200,20},&row));
    return row;
}
static inline Fixture Open(void)
{ Fixture f={0}; OK(umi_chart_drawing_registry_create(&f.registry)); OK(UmiChartDrawingHistoryCreate(f.registry,&f.history)); return f; }
static inline void Close(Fixture *f)
{ UmiChartDrawingHistoryDestroy(f->history); umi_chart_drawing_registry_destroy(f->registry); }
static inline UmiChartDrawingHistorySnapshot Read(Fixture *f)
{ UmiChartDrawingHistorySnapshot s; OK(UmiChartDrawingHistoryRead(f->history,&s)); return s; }
static inline UmiStatus Upsert(UmiChartDrawingRegistry *candidate,void *data)
{ return umi_chart_drawing_registry_upsert(candidate,data); }
static inline UmiStatus Remove(UmiChartDrawingRegistry *candidate,void *data)
{ return umi_chart_drawing_registry_remove(candidate,data); }
static inline void Apply(Fixture *f,UmiChartDrawingSnapshot *row)
{ int changed=0; OK(UmiChartDrawingHistoryApply(f->history,row->pane_id,"Edit",Upsert,row,&changed)); CHECK(changed); }
static inline void Undo(Fixture *f,const char *pane)
{ UmiChartDrawingHistorySnapshot s=Read(f); OK(UmiChartDrawingHistoryUndo(f->history,pane,s.revision)); }
static inline void Redo(Fixture *f,const char *pane)
{ UmiChartDrawingHistorySnapshot s=Read(f); OK(UmiChartDrawingHistoryRedo(f->history,pane,s.revision)); }
static inline UmiChartDrawingSnapshot Find(Fixture *f,const char *id)
{ UmiChartDrawingSnapshot row; OK(umi_chart_drawing_registry_find(f->registry,id,&row)); return row; }
#endif
