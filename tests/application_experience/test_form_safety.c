/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_experience/test_form_safety.c
 *
 * PURPOSE:
 *   Invalid shared form structures must fail before mutating a model or report.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Invalid shared form structures must fail before mutating a model or report. */
#include "umicom/application/experience/experience.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d [%s]: %s\n", __FILE__, __LINE__, name, #x); result=1; goto cleanup; } } while (0)
#define OK(x) CHECK((x) == UMI_STATUS_OK)
static int exercise(const char *name)
{
    int result = 0;
    UmiApplicationExperienceFormModel *before = calloc(1U, sizeof *before);
    UmiApplicationExperienceFormModel *after = calloc(1U, sizeof *after);
    UmiApplicationExperienceFormModel *model = calloc(1U, sizeof *model);
    UmiApplicationExperienceFormChangeset *changes = calloc(1U, sizeof *changes);
    UmiApplicationExperienceFormChangeset *unchanged = calloc(1U, sizeof *unchanged);
    UmiApplicationExperienceValidationReport *report = calloc(1U, sizeof *report);
    UmiApplicationExperienceValidationReport *old_report = calloc(1U, sizeof *old_report);
    UmiApplicationExperienceFormHistory *history = NULL, *old_history = NULL;
    CHECK(before && after && model && changes && unchanged && report && old_report);
    UmiApplicationExperienceField fields[] = {
        {.field_id="name", .label="Name", .help_text="Fictional name", .kind=UMI_APPLICATION_EXPERIENCE_FIELD_TEXT, .value_type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT},
        {.field_id="count", .label="Count", .help_text="Example count", .kind=UMI_APPLICATION_EXPERIENCE_FIELD_INTEGER, .value_type=UMI_APPLICATION_EXPERIENCE_VALUE_INTEGER}
    };
    UmiApplicationExperienceFormSchema schema = {.struct_size=sizeof schema, .api_version=UMI_APPLICATION_EXPERIENCE_API_VERSION,
        .form_id="synthetic.form", .title="Fictional form", .description="Shared form regression", .submit_command_id="synthetic.save", .fields=fields, .field_count=2U};
    UmiApplicationExperienceValue value;
    OK(umi_application_experience_form_model_init(before, &schema));
    OK(umi_application_experience_value_text(&value, "original"));
    OK(umi_application_experience_form_model_set(before, "name", &value));
    OK(umi_application_experience_value_integer(&value, 2));
    OK(umi_application_experience_form_model_set(before, "count", &value));
    *after = *before;
    OK(umi_application_experience_value_text(&value, "changed"));
    OK(umi_application_experience_form_model_set(after, "name", &value));
    OK(umi_application_experience_value_integer(&value, 3));
    OK(umi_application_experience_form_model_set(after, "count", &value));
    OK(umi_application_experience_form_changeset_create(before, after, changes));
    *model = *before;
    if (strcmp(name, "roundtrip") == 0) {
        OK(umi_application_experience_form_changeset_apply(model, changes, 0)); CHECK(memcmp(model, after, sizeof *model) == 0);
        OK(umi_application_experience_form_changeset_apply(model, changes, 1)); CHECK(memcmp(model, before, sizeof *model) == 0);
        OK(umi_application_experience_form_changeset_apply(model, changes, 0)); CHECK(memcmp(model, after, sizeof *model) == 0);
    } else if (strncmp(name, "schema-", 7U) == 0) {
        memset(report, 0xa5, sizeof *report); *old_report = *report;
        if (strcmp(name, "schema-null") == 0) schema.fields = NULL;
        else if (strcmp(name, "schema-count") == 0) schema.field_count = UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY + 1U;
        else if (strcmp(name, "schema-id") == 0) fields[0].field_id = NULL;
        else if (strcmp(name, "schema-duplicate") == 0) fields[1].field_id = fields[0].field_id;
        else CHECK(0);
        CHECK(umi_application_experience_form_schema_index(&schema, "name") == SIZE_MAX);
        CHECK(umi_application_experience_form_schema_field(&schema, "name") == NULL);
        CHECK(umi_application_experience_form_validate(model, report) != UMI_STATUS_OK);
        CHECK(memcmp(report, old_report, sizeof *report) == 0);
        *unchanged = *changes;
        CHECK(umi_application_experience_form_changeset_create(before, after, changes) != UMI_STATUS_OK);
        CHECK(memcmp(changes, unchanged, sizeof *changes) == 0);
    } else if (strcmp(name, "option-null") == 0 || strcmp(name, "option-id") == 0) {
        UmiApplicationExperienceFieldOption option = {.option_id=NULL, .label="Choice"};
        fields[0].kind=UMI_APPLICATION_EXPERIENCE_FIELD_CHOICE; fields[0].option_count=1U;
        fields[0].options=strcmp(name,"option-null")==0 ? NULL : &option;
        CHECK(umi_application_experience_field_option(&fields[0], "choice") == NULL);
    } else if (strcmp(name, "text-termination") == 0) {
        UmiApplicationExperienceValue other;
        memset(&value, 'x', sizeof value); value.type=UMI_APPLICATION_EXPERIENCE_VALUE_TEXT; other=value;
        CHECK(!umi_application_experience_value_equal(&value, &other));
        CHECK(!umi_application_experience_value_equal(NULL, &value));
    } else if (strcmp(name, "revision-limit") == 0) {
        model->revision=UINT64_MAX; *before=*model;
        OK(umi_application_experience_value_integer(&value, 4));
        CHECK(umi_application_experience_form_model_set(model,"count",&value)==UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(memcmp(model,before,sizeof *model)==0);
        OK(umi_application_experience_value_integer(&value,2));
        OK(umi_application_experience_form_model_set(model,"count",&value)); CHECK(memcmp(model,before,sizeof *model)==0);
    } else if (strcmp(name,"history-cursor")==0) {
        history=calloc(1U,sizeof *history); old_history=calloc(1U,sizeof *old_history); CHECK(history&&old_history);
        history->count=1U; history->cursor=2U; *old_history=*history;
        CHECK(umi_application_experience_form_history_record(history,changes)==UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(history,old_history,sizeof *history)==0);
    } else if (strcmp(name,"capture-invalid")==0 || strcmp(name,"capture-same-revision")==0) {
        *unchanged=*changes;
        if (strcmp(name,"capture-invalid")==0) after->present[1]=2;
        else after->revision=before->revision;
        CHECK(umi_application_experience_form_changeset_create(before,after,changes)==UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(changes,unchanged,sizeof *changes)==0);
    } else {
        UmiStatus expected=UMI_STATUS_INVALID_STATE;
        if (strcmp(name,"missing-second")==0) { strcpy(changes->changes[1].field_id,"missing"); expected=UMI_STATUS_NOT_FOUND; }
        else if (strcmp(name,"duplicate")==0) { strcpy(changes->changes[1].field_id,"name"); expected=UMI_STATUS_ALREADY_EXISTS; }
        else if (strcmp(name,"unterminated-id")==0) { memset(changes->changes[1].field_id,'x',sizeof changes->changes[1].field_id); expected=UMI_STATUS_INVALID_ARGUMENT; }
        else if (strcmp(name,"presence")==0) { changes->changes[1].after_present=2; expected=UMI_STATUS_INVALID_ARGUMENT; }
        else if (strcmp(name,"target-type")==0) OK(umi_application_experience_value_text(&changes->changes[1].after_value,"invalid integer"));
        else if (strcmp(name,"stale-value")==0) { model->values[1].integer_value=88; *before=*model; }
        else if (strcmp(name,"readonly")==0) { fields[1].read_only=1; expected=UMI_STATUS_PERMISSION_DENIED; }
        else if (strcmp(name,"revision-order")==0) changes->target_revision=changes->base_revision;
        else CHECK(0);
        CHECK(umi_application_experience_form_changeset_apply(model,changes,0)==expected);
        CHECK(memcmp(model,before,sizeof *model)==0);
    }
cleanup:
    free(before); free(after); free(model); free(changes); free(unchanged); free(report); free(old_report); free(history); free(old_history);
    return result;
}
int main(void)
{
    const char *cases[]={"roundtrip","schema-null","schema-count","schema-id","schema-duplicate","option-null","option-id",
        "text-termination","revision-limit","history-cursor","capture-invalid","capture-same-revision","missing-second","duplicate",
        "unterminated-id","presence","target-type","stale-value","readonly","revision-order"};
    for (size_t i=0U;i<sizeof cases/sizeof cases[0];++i) { int result=exercise(cases[i]); if(result!=0)return result; }
    return 0;
}
