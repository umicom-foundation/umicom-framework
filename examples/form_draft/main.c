/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/form_draft/main.c
 *
 * PURPOSE:
 *   Demonstrate isolated native form editing and one local commit using the installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/application/experience/form_draft.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(void)
{
    static const UmiApplicationExperienceField fields[] = {
        {.field_id="query",.label="Model search",.help_text="Search copied model observations.",
         .kind=UMI_APPLICATION_EXPERIENCE_FIELD_SEARCH,.value_type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT,.required=1}
    };
    static const UmiApplicationExperienceFormSchema schema = {sizeof schema,UMI_APPLICATION_EXPERIENCE_API_VERSION,
        "umicom.form.model-search","Model search","Submit an isolated local filter.","umicom.models.filter",fields,1};
    UmiApplicationExperienceFormModel *model=calloc(1U,sizeof *model);
    UmiApplicationExperienceFormDraft *draft=NULL;
    UmiApplicationExperienceValue value;
    UmiStatus status=model?umi_application_experience_form_model_init(model,&schema):UMI_STATUS_OUT_OF_MEMORY;
    if(status==UMI_STATUS_OK)status=umi_application_experience_form_draft_create(model,&draft);
    if(status==UMI_STATUS_OK)status=umi_application_experience_value_text(&value,"local model");
    if(status==UMI_STATUS_OK)status=umi_application_experience_form_draft_set(draft,"query",&value);
    /* Editing a draft must not trigger filtering, model loading or live edits. */
    if(status==UMI_STATUS_OK && (model->present[0]!=0 || model->revision!=1U))status=UMI_STATUS_INTERNAL_ERROR;
    if(status==UMI_STATUS_OK)status=umi_application_experience_form_draft_commit(draft,model);
    if(status==UMI_STATUS_OK && (model->revision!=2U || strcmp(model->values[0].text_value,"local model")!=0 ||
       umi_application_experience_form_draft_commit(draft,model)!=UMI_STATUS_INVALID_STATE))status=UMI_STATUS_INTERNAL_ERROR;
    if(status==UMI_STATUS_OK)puts("One local form commit; no provider request or model activation.");
    else fprintf(stderr,"Form draft example: %s\n",umi_status_text(status));
    umi_application_experience_form_draft_destroy(draft);free(model);
    return status==UMI_STATUS_OK?0:1;
}
