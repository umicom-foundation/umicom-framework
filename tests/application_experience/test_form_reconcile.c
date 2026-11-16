/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_form_reconcile.c
 *
 * PURPOSE:
 *   Verify explicit draft refresh, complete conflict reporting, atomic refusal and revision lifetime contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/application/experience/form_reconcile.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"%d: %s\n",__LINE__,#x);return 1;}}while(0)
/* Provide required non-null field help text so fixtures reach reconciliation rather than failing schema admission. The former implementation is retained for owner review. */
#if 0
static const UmiApplicationExperienceField fields[]={
    {.field_id="a",.label="Primary",.kind=UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,.value_type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,.required=1},
    {.field_id="b",.label="Secondary",.kind=UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,.value_type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,
     .default_value={.type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,.text_value="B"}},
    {.field_id="c",.label="Optional",.kind=UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,.value_type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT},
    {.field_id="locked",.label="Profile",.kind=UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,.value_type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,.read_only=1,
     .default_value={.type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,.text_value="fixed"}}
};

#endif
static const UmiApplicationExperienceField fields[]={
    {.field_id="a",.label="Primary",.help_text="Review stored form values.",.kind=UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,.value_type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,.required=1},
    {.field_id="b",.label="Secondary",.help_text="Review stored form values.",.kind=UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,.value_type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,
     .default_value={.type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,.text_value="B"}},
    {.field_id="c",.label="Optional",.help_text="Review stored form values.",.kind=UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,.value_type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT},
    {.field_id="locked",.label="Profile",.help_text="Review stored form values.",.kind=UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,.value_type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,.read_only=1,
     .default_value={.type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,.text_value="fixed"}}
};
static const UmiApplicationExperienceFormSchema schema={sizeof schema,UMI_APPLICATION_EXPERIENCE_API_VERSION,
    "umicom.form.reconcile","Draft refresh","Review local and live changes.","umicom.form.review",fields,4U};
static UmiStatus Live(UmiApplicationExperienceFormModel *model,const char *id,const char *text)
{
    UmiApplicationExperienceValue value;UmiStatus status=umi_application_experience_value_text(&value,text);
    return status==UMI_STATUS_OK?umi_application_experience_form_model_set(model,id,&value):status;
}
static UmiStatus Edit(UmiApplicationExperienceFormDraft *draft,const char *id,const char *text)
{
    UmiApplicationExperienceValue value;UmiStatus status=umi_application_experience_value_text(&value,text);
    return status==UMI_STATUS_OK?umi_application_experience_form_draft_set(draft,id,&value):status;
}
static int Same(const UmiApplicationExperienceFormModel *a,const UmiApplicationExperienceFormModel *b)
{
    if(a->schema!=b->schema || a->revision!=b->revision || a->saved_revision!=b->saved_revision)return 0;
    for(size_t i=0U;i<a->schema->field_count;++i)
        if(a->present[i]!=b->present[i] || (a->present[i]&&!umi_application_experience_value_equal(&a->values[i],&b->values[i])))return 0;
    return 1;
}
static void Remove(UmiApplicationExperienceFormModel *model,size_t index)
{
    model->present[index]=0;umi_application_experience_value_clear(&model->values[index]);++model->revision;
}
static int Maximum(void)
{
    UmiApplicationExperienceField many[UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY]={0};
    char names[UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY][16];
    for(size_t i=0U;i<UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY;++i){
        int n=snprintf(names[i],sizeof names[i],"field-%zu",i);CHECK(n>0 && (size_t)n<sizeof names[i]);
        many[i].help_text="Review this conflicting field.";
        many[i].field_id=names[i];many[i].label="Field";many[i].kind=UMI_APPLICATION_EXPERIENCE_FIELD_TEXT;
        many[i].value_type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT;
    }
    UmiApplicationExperienceFormSchema full=schema;full.fields=many;full.field_count=UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY;
    UmiApplicationExperienceFormModel model,before,after;UmiApplicationExperienceFormDraft *draft=NULL;
    UmiApplicationExperienceFormReconcileReport report;
    CHECK(umi_application_experience_form_model_init(&model,&full)==UMI_STATUS_OK);
    for(size_t i=0U;i<full.field_count;++i)CHECK(Live(&model,names[i],"base")==UMI_STATUS_OK);
    CHECK(umi_application_experience_form_draft_create(&model,&draft)==UMI_STATUS_OK);
    for(size_t i=0U;i<full.field_count;++i){CHECK(Edit(draft,names[i],"local")==UMI_STATUS_OK);CHECK(Live(&model,names[i],"remote")==UMI_STATUS_OK);}
    CHECK(umi_application_experience_form_draft_snapshot(draft,&before)==UMI_STATUS_OK);
    CHECK(umi_application_experience_form_draft_reconcile(draft,&model,&report)==UMI_STATUS_BUSY);
    CHECK(report.conflict_count==full.field_count && report.local_changed_count==full.field_count && report.live_changed_count==full.field_count);
    for(size_t i=0U;i<full.field_count;++i)CHECK(!strcmp(report.conflict_fields[i],names[i]));
    CHECK(umi_application_experience_form_draft_snapshot(draft,&after)==UMI_STATUS_OK && Same(&before,&after));
    umi_application_experience_form_draft_destroy(draft);return 0;
}
int main(int argc,char **argv)
{
    CHECK(argc==2);const char *mode=argv[1];if(!strcmp(mode,"maximum-conflicts"))return Maximum();
    UmiApplicationExperienceFormModel model,prior,snapshot,after,other;
    UmiApplicationExperienceFormSchema alternate=schema;
    UmiApplicationExperienceFormDraft *draft=NULL;
    UmiApplicationExperienceFormReconcileReport report;
    unsigned char report_before[sizeof report];
    CHECK(umi_application_experience_form_model_init(&model,&schema)==UMI_STATUS_OK);
    CHECK(Live(&model,"a","A")==UMI_STATUS_OK);umi_application_experience_form_model_mark_saved(&model);
    if(!strcmp(mode,"saved-marker"))model.saved_revision=1U;
    if(!strcmp(mode,"revision-limit"))model.revision=UINT64_MAX-1U;
    if(!strcmp(mode,"limit-nochange"))model.revision=UINT64_MAX;
    CHECK(umi_application_experience_form_draft_create(&model,&draft)==UMI_STATUS_OK);
    UmiStatus expected=UMI_STATUS_OK;
    if(!strcmp(mode,"local-only")||!strcmp(mode,"disjoint")||!strcmp(mode,"commit-after")||
       !strcmp(mode,"conflict")||!strcmp(mode,"conflict-order")||!strcmp(mode,"conflict-preserve")||
       !strcmp(mode,"resolve-live")||!strcmp(mode,"readonly-refresh")||!strcmp(mode,"revision-limit")||
       !strcmp(mode,"edit-after"))CHECK(Edit(draft,"a","local")==UMI_STATUS_OK);
/* The successful local-unset scenario needs a disjoint live edit. Editing the same field differently is covered by conflict cases, not a successful refresh. The former implementation is retained for owner review. */
#if 0
    if(!strcmp(mode,"remote-only")||!strcmp(mode,"disjoint")||!strcmp(mode,"commit-after")||
       !strcmp(mode,"local-unset")||!strcmp(mode,"required-incomplete")||!strcmp(mode,"snapshot-revision")||
       !strcmp(mode,"revision-limit")||!strcmp(mode,"repeat")||!strcmp(mode,"edit-after"))CHECK(Live(&model,"b","remote")==UMI_STATUS_OK);
#endif
    if(!strcmp(mode,"remote-only")||!strcmp(mode,"disjoint")||!strcmp(mode,"commit-after")||
       !strcmp(mode,"required-incomplete")||!strcmp(mode,"snapshot-revision")||
       !strcmp(mode,"revision-limit")||!strcmp(mode,"repeat")||!strcmp(mode,"edit-after"))CHECK(Live(&model,"b","remote")==UMI_STATUS_OK);
    if(!strcmp(mode,"local-unset"))CHECK(Live(&model,"a","remote")==UMI_STATUS_OK);
    if(!strcmp(mode,"convergent")){CHECK(Edit(draft,"a","same")==UMI_STATUS_OK);CHECK(Live(&model,"a","same")==UMI_STATUS_OK);}
    if(!strcmp(mode,"conflict")||!strcmp(mode,"conflict-order")||!strcmp(mode,"conflict-preserve")||!strcmp(mode,"resolve-live")){
        CHECK(Live(&model,"a","remote")==UMI_STATUS_OK);expected=UMI_STATUS_BUSY;
    }
    if(!strcmp(mode,"conflict-order")){CHECK(Edit(draft,"b","local-b")==UMI_STATUS_OK);CHECK(Live(&model,"b","remote-b")==UMI_STATUS_OK);}
    if(!strcmp(mode,"conflict-preserve"))CHECK(Live(&model,"c","remote-c")==UMI_STATUS_OK);
    if(!strcmp(mode,"unversioned-live"))CHECK(umi_application_experience_value_text(&model.values[1],"unversioned")==UMI_STATUS_OK);
    if(!strcmp(mode,"remote-unset")||!strcmp(mode,"converged-unset"))Remove(&model,1U);
    if(!strcmp(mode,"local-unset")||!strcmp(mode,"converged-unset")||!strcmp(mode,"presence-default"))CHECK(umi_application_experience_form_draft_unset(draft,"b")==UMI_STATUS_OK);
    if(!strcmp(mode,"required-incomplete"))CHECK(umi_application_experience_form_draft_unset(draft,"a")==UMI_STATUS_OK);
    if(!strcmp(mode,"readonly-refresh")){CHECK(umi_application_experience_value_text(&model.values[3],"new-profile")==UMI_STATUS_OK);++model.revision;}
    if(!strcmp(mode,"saved-marker"))umi_application_experience_form_model_mark_saved(&model);
    if(!strcmp(mode,"revision-only"))++model.revision;
    if(!strcmp(mode,"snapshot-revision")){CHECK(Edit(draft,"a","one")==UMI_STATUS_OK);CHECK(Edit(draft,"a","two")==UMI_STATUS_OK);CHECK(Edit(draft,"a","three")==UMI_STATUS_OK);}
    if(!strcmp(mode,"revision-limit"))expected=UMI_STATUS_CAPACITY_EXCEEDED;
    if(!strcmp(mode,"consumed")){CHECK(umi_application_experience_form_draft_commit(draft,&model)==UMI_STATUS_OK);expected=UMI_STATUS_INVALID_STATE;}
    if(!strcmp(mode,"schema")){model.schema=&alternate;expected=UMI_STATUS_INVALID_STATE;}
    if(!strcmp(mode,"null-schema")){model.schema=NULL;expected=UMI_STATUS_INVALID_STATE;}
    if(!strcmp(mode,"backwards")){model.revision=model.saved_revision=1U;expected=UMI_STATUS_INVALID_STATE;}
    if(!strcmp(mode,"bad-presence")){model.present[0]=2;expected=UMI_STATUS_INVALID_STATE;}
    if(!strcmp(mode,"bad-value")){memset(model.values[0].text_value,'x',sizeof model.values[0].text_value);expected=UMI_STATUS_INVALID_STATE;}
    const UmiApplicationExperienceFormModel *current=&model;
    if(!strcmp(mode,"wrong-owner")){other=model;current=&other;expected=UMI_STATUS_INVALID_ARGUMENT;}
    CHECK(umi_application_experience_form_draft_snapshot(draft,&snapshot)==UMI_STATUS_OK);
    memcpy(&prior,&model,sizeof prior);memset(&report,0xa5,sizeof report);memcpy(report_before,&report,sizeof report);
    if(!strcmp(mode,"nulls")){
        CHECK(umi_application_experience_form_draft_reconcile(NULL,&model,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_application_experience_form_draft_reconcile(draft,NULL,&report)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_application_experience_form_draft_reconcile(draft,&model,NULL)==UMI_STATUS_INVALID_ARGUMENT);
        CHECK(!memcmp(report_before,&report,sizeof report));umi_application_experience_form_draft_destroy(draft);return 0;
    }
    CHECK(umi_application_experience_form_draft_reconcile(draft,current,&report)==expected);
    CHECK(!memcmp(&prior,&model,sizeof model));
    CHECK(umi_application_experience_form_draft_snapshot(draft,&after)==UMI_STATUS_OK);
    if(expected!=UMI_STATUS_OK){
        CHECK(Same(&snapshot,&after));
        if(expected==UMI_STATUS_BUSY){
            size_t count=!strcmp(mode,"conflict-order")?2U:1U;
            CHECK(report.conflict_count==count && !report.refreshed && !strcmp(report.conflict_fields[0],"a"));
            if(count==2U)CHECK(!strcmp(report.conflict_fields[1],"b"));
            if(!strcmp(mode,"resolve-live")){
                CHECK(Edit(draft,"a","remote")==UMI_STATUS_OK);
                CHECK(umi_application_experience_form_draft_reconcile(draft,&model,&report)==UMI_STATUS_OK && report.converged_count==1U);
                uint64_t revision=model.revision;CHECK(umi_application_experience_form_draft_commit(draft,&model)==UMI_STATUS_OK && model.revision==revision);
            }
        }else CHECK(!memcmp(report_before,&report,sizeof report));
    }else if(!strcmp(mode,"unchanged")||!strcmp(mode,"local-only")||!strcmp(mode,"limit-nochange")||!strcmp(mode,"presence-default")){
        CHECK(!report.refreshed && Same(&snapshot,&after));
        if(!strcmp(mode,"presence-default"))CHECK(after.present[1]==0 && model.present[1]==1);
    }else{
        CHECK(report.refreshed && report.conflict_count==0U && after.saved_revision==model.saved_revision);
        uint64_t greatest=snapshot.revision>model.revision?snapshot.revision:model.revision;
        CHECK(after.revision==greatest+1U && report.snapshot_revision==after.revision);
        if(!strcmp(mode,"remote-only")||!strcmp(mode,"disjoint")||!strcmp(mode,"commit-after")||!strcmp(mode,"edit-after"))CHECK(!strcmp(after.values[1].text_value,"remote"));
        if(!strcmp(mode,"disjoint")||!strcmp(mode,"commit-after")||!strcmp(mode,"edit-after"))CHECK(!strcmp(after.values[0].text_value,"local"));
        if(!strcmp(mode,"convergent")||!strcmp(mode,"converged-unset"))CHECK(report.converged_count==1U);
        if(!strcmp(mode,"remote-unset")||!strcmp(mode,"local-unset")||!strcmp(mode,"converged-unset"))CHECK(after.present[1]==0);
        if(!strcmp(mode,"unversioned-live"))CHECK(!strcmp(after.values[1].text_value,"unversioned") && report.live_changed_count==1U);
        if(!strcmp(mode,"snapshot-revision"))CHECK(!strcmp(after.values[0].text_value,"three"));
        if(!strcmp(mode,"readonly-refresh"))CHECK(!strcmp(after.values[3].text_value,"new-profile") && Edit(draft,"locked","refuse")==UMI_STATUS_PERMISSION_DENIED);
        if(!strcmp(mode,"required-incomplete"))CHECK(after.present[0]==0 && umi_application_experience_form_draft_commit(draft,&model)==UMI_STATUS_INVALID_STATE);
        if(!strcmp(mode,"repeat")){
            snapshot=after;CHECK(umi_application_experience_form_draft_reconcile(draft,&model,&report)==UMI_STATUS_OK && !report.refreshed);
            CHECK(umi_application_experience_form_draft_snapshot(draft,&after)==UMI_STATUS_OK && Same(&snapshot,&after));
        }
        if(!strcmp(mode,"edit-after"))CHECK(Edit(draft,"c","later")==UMI_STATUS_OK);
        if(!strcmp(mode,"commit-after")||!strcmp(mode,"edit-after")||!strcmp(mode,"readonly-refresh")){
            uint64_t revision=model.revision;CHECK(umi_application_experience_form_draft_commit(draft,&model)==UMI_STATUS_OK && model.revision==revision+1U);
            CHECK(!strcmp(model.values[0].text_value,"local"));if(!strcmp(mode,"edit-after"))CHECK(!strcmp(model.values[2].text_value,"later"));
        }
    }
    umi_application_experience_form_draft_destroy(draft);return 0;
}
