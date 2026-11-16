/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/form_draft/reconcile.c
 *
 * PURPOSE:
 *   Demonstrate explicit draft refresh and conflict refusal using only installed Framework SDK contracts.
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
#include <stdlib.h>
#include <string.h>
int main(void)
{
    static const UmiApplicationExperienceField fields[]={
        {.field_id="query",.label="Search",.help_text="Review a local search draft.",.kind=UMI_APPLICATION_EXPERIENCE_FIELD_SEARCH,
         .value_type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,.required=1},
        {.field_id="catalogue",.label="Observation",.help_text="Name a synthetic captured catalogue.",.kind=UMI_APPLICATION_EXPERIENCE_FIELD_TEXT,
         .value_type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,.default_value={.type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,.text_value="first"}}
    };
    static const UmiApplicationExperienceFormSchema schema={sizeof schema,UMI_APPLICATION_EXPERIENCE_API_VERSION,
        "umicom.form.review-refresh","Search review","Keep edits while observing live changes.","umicom.search.review",fields,2U};
    UmiApplicationExperienceFormModel *model=calloc(1U,sizeof *model);
    UmiApplicationExperienceFormDraft *draft=NULL;
    UmiApplicationExperienceFormReconcileReport report;
    UmiApplicationExperienceValue value;
    UmiStatus status=model?umi_application_experience_form_model_init(model,&schema):UMI_STATUS_OUT_OF_MEMORY;
    if(status==UMI_STATUS_OK)status=umi_application_experience_value_text(&value,"initial");
    if(status==UMI_STATUS_OK)status=umi_application_experience_form_model_set(model,"query",&value);
    if(status==UMI_STATUS_OK)status=umi_application_experience_form_draft_create(model,&draft);
    if(status==UMI_STATUS_OK)status=umi_application_experience_value_text(&value,"reviewed query");
    if(status==UMI_STATUS_OK)status=umi_application_experience_form_draft_set(draft,"query",&value);
    if(status==UMI_STATUS_OK)status=umi_application_experience_value_text(&value,"second");
    if(status==UMI_STATUS_OK)status=umi_application_experience_form_model_set(model,"catalogue",&value);
    /* Ordinary commit never silently refreshes a stale draft. */
    if(status==UMI_STATUS_OK && umi_application_experience_form_draft_commit(draft,model)!=UMI_STATUS_BUSY)
        status=UMI_STATUS_INTERNAL_ERROR;
    if(status==UMI_STATUS_OK)status=umi_application_experience_form_draft_reconcile(draft,model,&report);
    if(status==UMI_STATUS_OK && (!report.refreshed || report.conflict_count!=0U ||
        strcmp(model->values[0].text_value,"initial")))status=UMI_STATUS_INTERNAL_ERROR;
    if(status==UMI_STATUS_OK)status=umi_application_experience_form_draft_commit(draft,model);
    if(status==UMI_STATUS_OK && (strcmp(model->values[0].text_value,"reviewed query") ||
        strcmp(model->values[1].text_value,"second")))status=UMI_STATUS_INTERNAL_ERROR;
    umi_application_experience_form_draft_destroy(draft);draft=NULL;
    if(status==UMI_STATUS_OK)status=umi_application_experience_form_draft_create(model,&draft);
    if(status==UMI_STATUS_OK)status=umi_application_experience_value_text(&value,"my edit");
    if(status==UMI_STATUS_OK)status=umi_application_experience_form_draft_set(draft,"query",&value);
    if(status==UMI_STATUS_OK)status=umi_application_experience_value_text(&value,"another edit");
    if(status==UMI_STATUS_OK)status=umi_application_experience_form_model_set(model,"query",&value);
    if(status==UMI_STATUS_OK){
        UmiStatus refreshed=umi_application_experience_form_draft_reconcile(draft,model,&report);
        if(refreshed!=UMI_STATUS_BUSY || report.conflict_count!=1U || strcmp(report.conflict_fields[0],"query") ||
            strcmp(model->values[0].text_value,"another edit"))status=UMI_STATUS_INTERNAL_ERROR;
    }
    if(status==UMI_STATUS_OK)puts("Disjoint draft refreshed; conflicting query retained for review. No provider request.");
    else fprintf(stderr,"Umicom draft refresh example: %s\n",umi_status_text(status));
    umi_application_experience_form_draft_destroy(draft);free(model);
    return status==UMI_STATUS_OK?0:1;
}
