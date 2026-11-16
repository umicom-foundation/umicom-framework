/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience/form_draft.c
 *
 * PURPOSE:
 *   Own isolated form values while reusing shared validation and atomic changeset publication.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/application/experience/form_draft.h"
#include "umicom/application/experience/form_reconcile.h"
#include "umicom/base/text.h"
#include "umicom/application/experience/form_changeset.h"
#include "umicom/security/secrets.h"
#include <stdlib.h>
#include <string.h>

struct UmiApplicationExperienceFormDraft {
    const UmiApplicationExperienceFormModel *owner;
    UmiApplicationExperienceFormModel base, edited;
    int consumed;
};
/* Structural admission permits incomplete values so a user can repair a form.
 * Completeness and field constraints are checked by the existing form validator. */
static UmiStatus DraftModelCheck(const UmiApplicationExperienceFormModel *model)
{
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = umi_application_experience_form_schema_validate(model->schema);
    if (status != UMI_STATUS_OK) return status;
    if (model->revision == 0U || model->saved_revision > model->revision) return UMI_STATUS_INVALID_STATE;
    for (size_t i = 0U; i < model->schema->field_count; ++i) {
        if ((model->present[i] != 0 && model->present[i] != 1) ||
            (model->present[i] && umi_application_experience_value_validate(&model->values[i]) != UMI_STATUS_OK))
            return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
UmiStatus umi_application_experience_form_draft_create(
    const UmiApplicationExperienceFormModel *model, UmiApplicationExperienceFormDraft **out)
{
    if (out == NULL || *out != NULL) return UMI_STATUS_INVALID_ARGUMENT;
    UmiStatus status = DraftModelCheck(model);
    if (status != UMI_STATUS_OK) return status;
    UmiApplicationExperienceFormDraft *draft = calloc(1U, sizeof *draft);
    if (draft == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    draft->owner = model; draft->base = *model; draft->edited = *model;
    *out = draft;
    return UMI_STATUS_OK;
}
void umi_application_experience_form_draft_destroy(UmiApplicationExperienceFormDraft *draft)
{
    if (draft != NULL) { umi_secret_clear(draft, sizeof *draft); free(draft); }
}
UmiStatus umi_application_experience_form_draft_set(UmiApplicationExperienceFormDraft *draft,
    const char *field_id, const UmiApplicationExperienceValue *value)
{
    if (draft == NULL || field_id == NULL || value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (draft->consumed) return UMI_STATUS_INVALID_STATE;
    return umi_application_experience_form_model_set(&draft->edited, field_id, value);
}
UmiStatus umi_application_experience_form_draft_unset(UmiApplicationExperienceFormDraft *draft,
    const char *field_id)
{
    if (draft == NULL || field_id == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (draft->consumed) return UMI_STATUS_INVALID_STATE;
    size_t index = umi_application_experience_form_schema_index(draft->edited.schema, field_id);
    if (index == SIZE_MAX) return UMI_STATUS_NOT_FOUND;
    if (draft->edited.schema->fields[index].read_only) return UMI_STATUS_PERMISSION_DENIED;
    if (!draft->edited.present[index]) return UMI_STATUS_OK;
    if (draft->edited.revision == UINT64_MAX) return UMI_STATUS_CAPACITY_EXCEEDED;
    umi_secret_clear(&draft->edited.values[index], sizeof draft->edited.values[index]);
    draft->edited.present[index] = 0;
    ++draft->edited.revision;
    return UMI_STATUS_OK;
}
UmiStatus umi_application_experience_form_draft_snapshot(const UmiApplicationExperienceFormDraft *draft,
    UmiApplicationExperienceFormModel *out)
{
    if (draft == NULL || out == NULL || out == draft->owner) return UMI_STATUS_INVALID_ARGUMENT;
    *out = draft->edited;
    return UMI_STATUS_OK;
}
UmiStatus umi_application_experience_form_draft_validate(const UmiApplicationExperienceFormDraft *draft,
    UmiApplicationExperienceValidationReport *report)
{
    if (draft == NULL || report == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_application_experience_form_validate(&draft->edited, report);
}
UmiStatus umi_application_experience_form_draft_commit(UmiApplicationExperienceFormDraft *draft,
    UmiApplicationExperienceFormModel *model)
{
    if (draft == NULL || model == NULL || model != draft->owner) return UMI_STATUS_INVALID_ARGUMENT;
    if (draft->consumed) return UMI_STATUS_INVALID_STATE;
    UmiStatus status = DraftModelCheck(model);
    if (status != UMI_STATUS_OK) return status;
    if (model->schema != draft->base.schema || model->revision != draft->base.revision)
        return UMI_STATUS_BUSY;
    /* Compare every live field, not only edited ones: another writer must not
     * change unedited context under a review, even if it forgot to bump revision. */
    for (size_t i = 0U; i < model->schema->field_count; ++i) {
        if (model->present[i] != draft->base.present[i] ||
            (model->present[i] && !umi_application_experience_value_equal(&model->values[i], &draft->base.values[i])))
            return UMI_STATUS_BUSY;
    }
    UmiApplicationExperienceValidationReport *report = malloc(sizeof *report);
    UmiApplicationExperienceFormChangeset *changes = malloc(sizeof *changes);
    if (report == NULL || changes == NULL) { free(report); free(changes); return UMI_STATUS_OUT_OF_MEMORY; }
    status = umi_application_experience_form_validate(&draft->edited, report);
    if (status == UMI_STATUS_OK && !umi_application_experience_validation_report_passed(report))
        status = UMI_STATUS_INVALID_STATE;
    if (status == UMI_STATUS_OK)
        status = umi_application_experience_form_changeset_create(&draft->base, &draft->edited, changes);
    if (status == UMI_STATUS_OK) {
        if (changes->count != 0U && model->revision == UINT64_MAX) status = UMI_STATUS_CAPACITY_EXCEEDED;
        else {
            /* Multiple widget edits publish as one live update. Existing
             * changeset validation owns read-only and captured-value checks. */
            changes->target_revision = model->revision + (changes->count != 0U ? 1U : 0U);
            status = umi_application_experience_form_changeset_apply(model, changes, 0);
        }
    }
    if (status == UMI_STATUS_OK) draft->consumed = 1;
    umi_secret_clear(report, sizeof *report); free(report);
    umi_secret_clear(changes, sizeof *changes); free(changes);
    return status;
}

/* The draft already owns base and edited values. Three-way refresh belongs here
 * so UI clients cannot accidentally bypass lifetime, schema or revision checks.
 * Presence is compared independently of effective schema defaults. */
static int DraftFieldSame(const UmiApplicationExperienceFormModel *left,
    const UmiApplicationExperienceFormModel *right,size_t index)
{
    return left->present[index]==right->present[index] &&
        (!left->present[index] || umi_application_experience_value_equal(&left->values[index],&right->values[index]));
}
UmiStatus umi_application_experience_form_draft_reconcile(
    UmiApplicationExperienceFormDraft *draft,
    const UmiApplicationExperienceFormModel *model,
    UmiApplicationExperienceFormReconcileReport *report)
{
    if(draft==NULL || model==NULL || report==NULL || model!=draft->owner)
        return UMI_STATUS_INVALID_ARGUMENT;
    if(draft->consumed)return UMI_STATUS_INVALID_STATE;
/* Check the captured schema identity before validating its replacement, so any schema transition has one deterministic INVALID_STATE result. The former implementation is retained for owner review. */
#if 0
    UmiStatus status=DraftModelCheck(model);
    if(status!=UMI_STATUS_OK)return status;
    if(model->schema!=draft->base.schema || model->revision<draft->base.revision)
        return UMI_STATUS_INVALID_STATE;
#endif
    if(model->schema!=draft->base.schema || model->revision<draft->base.revision)
        return UMI_STATUS_INVALID_STATE;
    UmiStatus status=DraftModelCheck(model);
    if(status!=UMI_STATUS_OK)return status;
    UmiApplicationExperienceFormReconcileReport observed={0};
    observed.base_revision=draft->base.revision;
    observed.live_revision=model->revision;
    observed.snapshot_revision=draft->edited.revision;
    int changed=model->revision!=draft->base.revision || model->saved_revision!=draft->base.saved_revision;
    for(size_t i=0U;i<model->schema->field_count;++i){
        int local=!DraftFieldSame(&draft->base,&draft->edited,i);
        int live=!DraftFieldSame(&draft->base,model,i);
        if(local)++observed.local_changed_count;
        if(live){++observed.live_changed_count;changed=1;}
        if(local && live){
            if(DraftFieldSame(&draft->edited,model,i))++observed.converged_count;
            else{
                status=umi_text_copy(observed.conflict_fields[observed.conflict_count],
                    sizeof observed.conflict_fields[0],model->schema->fields[i].field_id);
                if(status!=UMI_STATUS_OK)return status;
                ++observed.conflict_count;
            }
        }
    }
    /* A conflict publishes names only, never a partial merged draft. Callers
     * can explain the conflict while preserving the user's unsaved work. */
    if(observed.conflict_count!=0U){*report=observed;return UMI_STATUS_BUSY;}
    if(!changed){*report=observed;return UMI_STATUS_OK;}
    uint64_t revision=draft->edited.revision>model->revision?draft->edited.revision:model->revision;
    if(revision==UINT64_MAX)return UMI_STATUS_CAPACITY_EXCEEDED;
    UmiApplicationExperienceFormModel *merged=malloc(sizeof *merged);
    if(merged==NULL)return UMI_STATUS_OUT_OF_MEMORY;
    *merged=*model;
    for(size_t i=0U;i<model->schema->field_count;++i){
        if(!DraftFieldSame(&draft->base,&draft->edited,i)){
            merged->present[i]=draft->edited.present[i];
            merged->values[i]=draft->edited.values[i];
        }
    }
    merged->revision=revision+1U;
    /* All decisions and allocations precede publication. The live owner and
     * its revision remain untouched; a later explicit commit performs validation. */
    umi_secret_clear(&draft->base,sizeof draft->base);draft->base=*model;
    umi_secret_clear(&draft->edited,sizeof draft->edited);draft->edited=*merged;
    observed.refreshed=1;observed.snapshot_revision=merged->revision;
    umi_secret_clear(merged,sizeof *merged);free(merged);
    *report=observed;
    return UMI_STATUS_OK;
}
