/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/form_draft.h
 *
 * PURPOSE:
 *   Stage isolated typed edits and commit one validated form update through existing Framework form services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_APPLICATION_EXPERIENCE_FORM_DRAFT_H
#define UMICOM_APPLICATION_EXPERIENCE_FORM_DRAFT_H
#include "umicom/application/experience/form_validation.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiApplicationExperienceFormDraft UmiApplicationExperienceFormDraft;
/* One owner-thread draft belongs to one live model address. The model and its
 * complete schema (including strings/options) must outlive the draft. The
 * schema is borrowed and must remain immutable; values are privately copied.
 * Serialize all model/draft access. No global state, persistence, callbacks,
 * commands or network effects are performed. *out must initially be NULL. */
UmiStatus umi_application_experience_form_draft_create(
    const UmiApplicationExperienceFormModel *model, UmiApplicationExperienceFormDraft **out);
void umi_application_experience_form_draft_destroy(UmiApplicationExperienceFormDraft *draft);
/* Field setters reuse the model's type/range/read-only constraints. Unset removes
 * a stored value; validation then uses the schema default, as for other forms.
 * An incomplete draft can be inspected but cannot commit until validation passes. */
UmiStatus umi_application_experience_form_draft_set(UmiApplicationExperienceFormDraft *draft,
    const char *field_id, const UmiApplicationExperienceValue *value);
UmiStatus umi_application_experience_form_draft_unset(UmiApplicationExperienceFormDraft *draft,
    const char *field_id);
/* Copy a render snapshot into separate caller-owned storage (never the live
 * model). The schema pointer remains borrowed. Mutating this copy cannot edit
 * the draft. Revision counts draft edits, not committed live updates. */
UmiStatus umi_application_experience_form_draft_snapshot(const UmiApplicationExperienceFormDraft *draft,
    UmiApplicationExperienceFormModel *out);
UmiStatus umi_application_experience_form_draft_validate(const UmiApplicationExperienceFormDraft *draft,
    UmiApplicationExperienceValidationReport *report);
/* Require the same owner, schema identity, base revision and all captured live
 * values before applying a complete changeset. A changed form advances the live
 * revision once; no-change submission leaves it unchanged. The live saved marker
 * is preserved. Any refusal leaves both live values and draft usable unchanged.
 * Success consumes the draft: a repeated commit or later edit is INVALID_STATE.
 * It does not execute submit_command_id or authorize a provider request. Durable
 * task ownership and external-effect deduplication require their separate service.
 * Snapshot/validate remain available after success; destroy when finished. */
UmiStatus umi_application_experience_form_draft_commit(UmiApplicationExperienceFormDraft *draft,
    UmiApplicationExperienceFormModel *model);
#ifdef __cplusplus
}
#endif
#endif
