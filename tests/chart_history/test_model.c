/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_history/test_model.c
 * PURPOSE: Exercise chronological reversible state, no-op branches, fresh revisions and instrument boundaries.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
int main(int argc,char **argv)
{
    CHECK(argc==2); const char *name=argv[1]; Fixture f=Open();
    UmiChartDrawingSnapshot a=Drawing("a","pane"), b=Drawing("b","pane"), value;
    UmiChartDrawingHistorySnapshot s=Read(&f); CHECK(s.undo_count==0 && !s.stale && s.redo_count==0);
    if (strcmp(name,"empty")==0) {
        CHECK(UmiChartDrawingHistoryUndo(f.history,"pane",s.revision)==UMI_STATUS_NOT_FOUND);
        CHECK(UmiChartDrawingHistoryRedo(f.history,"pane",s.revision)==UMI_STATUS_NOT_FOUND);
        OK(UmiChartDrawingHistoryReset(f.history)); CHECK(Read(&f).revision==s.revision);
    } else if (strcmp(name,"create")==0) {
        Apply(&f,&a); uint64_t version=Find(&f,"a").revision; Undo(&f,"pane");
        CHECK(umi_chart_drawing_registry_count(f.registry)==0); Redo(&f,"pane"); value=Find(&f,"a");
        CHECK(value.revision>version && value.time1==100 && value.value2==20);
    } else if (strcmp(name,"remove-order")==0) {
        Apply(&f,&a); Apply(&f,&b); UmiChartDrawingSnapshot c=Drawing("c","other"); Apply(&f,&c);
        uint64_t unchanged=Find(&f,"c").revision;
        OK(UmiChartDrawingHistoryApply(f.history,"pane","Remove",Remove,"a",NULL));
        Undo(&f,"pane"); OK(umi_chart_drawing_registry_at(f.registry,0,&value)); CHECK(strcmp(value.id,"a")==0);
        OK(umi_chart_drawing_registry_at(f.registry,1,&value)); CHECK(strcmp(value.id,"b")==0);
        CHECK(Find(&f,"c").revision==unchanged); Redo(&f,"pane"); CHECK(umi_chart_drawing_registry_count(f.registry)==2);
    } else if (strcmp(name,"all-fields")==0) {
        Apply(&f,&a); a.time1=90; a.value2=75; a.selected=1; a.locked=1; a.visibility_flags=1;
        strcpy(a.style,"legacy-style"); a.style[255]='x'; /* bounded terminator earlier remains valid */
        Apply(&f,&a); Undo(&f,"pane"); value=Find(&f,"a"); CHECK(!value.locked && !value.selected && !value.visibility_flags && value.style[0]=='\0');
        Redo(&f,"pane"); value=Find(&f,"a"); CHECK(value.time1==90 && value.value2==75 && value.locked && value.selected && value.visibility_flags==1 && value.style[255]=='x');
    } else if (strcmp(name,"chain")==0) {
        Apply(&f,&a); a.value2=25; Apply(&f,&a); a.value2=30; Apply(&f,&a);
        Undo(&f,"pane"); CHECK(Find(&f,"a").value2==25); Undo(&f,"pane"); CHECK(Find(&f,"a").value2==20);
        Redo(&f,"pane"); CHECK(Find(&f,"a").value2==25); Redo(&f,"pane"); CHECK(Find(&f,"a").value2==30);
    } else if (strcmp(name,"no-op-redo")==0 || strcmp(name,"branch")==0) {
        Apply(&f,&a); Apply(&f,&b); Undo(&f,"pane"); s=Read(&f);
        if (strcmp(name,"no-op-redo")==0) {
            int changed=1; OK(UmiChartDrawingHistoryApply(f.history,"pane","Same",Upsert,&a,&changed));
            CHECK(!changed && Read(&f).revision==s.revision && Read(&f).registry_revision==s.registry_revision && Read(&f).redo_count==1);
            Redo(&f,"pane");
        } else { a.value1=5; Apply(&f,&a); CHECK(Read(&f).redo_count==0 && Read(&f).undo_count==2); }
    } else if (strcmp(name,"stale-revision")==0) {
        Apply(&f,&a); s=Read(&f); Apply(&f,&b);
        CHECK(UmiChartDrawingHistoryUndo(f.history,"pane",s.revision)==UMI_STATUS_BUSY); CHECK(umi_chart_drawing_registry_count(f.registry)==2);
    } else if (strcmp(name,"other-pane")==0) {
        Apply(&f,&a); strcpy(b.pane_id,"other"); Apply(&f,&b); s=Read(&f);
        CHECK(strcmp(s.undo_pane,"other")==0);
        CHECK(UmiChartDrawingHistoryUndo(f.history,"pane",s.revision)==UMI_STATUS_INVALID_STATE);
        Undo(&f,"other"); Undo(&f,"pane"); Redo(&f,"pane"); Redo(&f,"other");
    } else if (strcmp(name,"external")==0) {
        Apply(&f,&a); s=Read(&f); OK(umi_chart_drawing_registry_upsert(f.registry,&b)); CHECK(Read(&f).stale);
        CHECK(UmiChartDrawingHistoryUndo(f.history,"pane",s.revision)==UMI_STATUS_BUSY);
        int changed=1; OK(UmiChartDrawingHistoryApply(f.history,"pane","Same",Upsert,&a,&changed)); CHECK(!changed && Read(&f).stale);
        a.value1=1; Apply(&f,&a); CHECK(!Read(&f).stale && Read(&f).undo_count==1); Undo(&f,"pane"); CHECK(Find(&f,"b").value1==10);
    } else if (strcmp(name,"reset")==0) {
        Apply(&f,&a); s=Read(&f); OK(UmiChartDrawingHistoryReset(f.history)); CHECK(Read(&f).undo_count==0 && Read(&f).retained_bytes==0);
        CHECK(Find(&f,"a").revision==s.registry_revision); CHECK(Read(&f).revision>s.revision);
    } else if (strcmp(name,"step-budget")==0) {
        for (size_t i=0;i<70;++i) { a.value2=20+(double)i; Apply(&f,&a); }
        CHECK(Read(&f).undo_count==64); for (size_t i=0;i<64;++i) Undo(&f,"pane");
        CHECK(Find(&f,"a").value2==25 && Read(&f).redo_count==64);
    } else if (strcmp(name,"independent")==0) {
        Fixture other=Open(); Apply(&f,&a); CHECK(Read(&other).undo_count==0); Close(&other); Undo(&f,"pane");
    } else return 2;
    Close(&f); return 0;
}
