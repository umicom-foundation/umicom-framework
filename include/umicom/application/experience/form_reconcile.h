/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/form_reconcile.h
 *
 * PURPOSE:
 *   Refresh an isolated form draft against current live values without overwriting conflicting edits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#ifndef UMICOM_APPLICATION_EXPERIENCE_FORM_RECONCILE_H
#define UMICOM_APPLICATION_EXPERIENCE_FORM_RECONCILE_H
#include "umicom/application/experience/form_draft.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Counts compare each field's presence and typed value against the old draft
 * base. Converged means both sides changed to the same stored representation.
 * Absent and an explicitly stored default remain distinct. No values are logged
 * or copied into this report; conflict identifiers follow schema order. */
typedef struct UmiApplicationExperienceFormReconcileReport {
    uint64_t base_revision,live_revision,snapshot_revision;
    size_t local_changed_count,live_changed_count,converged_count,conflict_count;
    char conflict_fields[UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY][UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
    int refreshed;
} UmiApplicationExperienceFormReconcileReport;
/* Explicitly refresh a usable draft, never its live model. Compare captured
 * base, current live fields and local draft fields. Adopt live changes to fields
 * untouched locally; retain local-only edits and identical concurrent results.
 * A field changed differently on both sides is a conflict.
 *
 * BUSY publishes a complete copied conflict report but leaves the draft and
 * live model unchanged. Other errors preserve report too. OK publishes a report;
 * refreshed=0 means no live revision, saved marker or stored field changed.
 * A real refresh captures the new base and saved marker and advances the draft
 * snapshot revision to max(previous snapshot,live revision)+1. Refuse overflow.
 *
 * Require the same live model address and unchanged immutable schema. A changed
 * schema, backward live revision or consumed draft returns INVALID_STATE; a
 * different owner returns INVALID_ARGUMENT. Structurally valid incomplete values
 * remain editable; full field/completeness validation still occurs at commit.
 *
 * Serialize all access on the owner thread. The complete schema and live model
 * outlive the draft. Report storage must not overlap either owner. No callbacks,
 * persistence, commands or provider requests run. Commit retains its original
 * stale-base refusal: refresh is never performed automatically. */
UmiStatus umi_application_experience_form_draft_reconcile(
    UmiApplicationExperienceFormDraft *draft,
    const UmiApplicationExperienceFormModel *model,
    UmiApplicationExperienceFormReconcileReport *report);
#ifdef __cplusplus
}
#endif
#endif
