/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/structured_form_upgrade.h
 *
 * PURPOSE:
 *   Review a compatible schema upgrade before appending a new stored revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Review a compatible schema upgrade before appending a new stored revision. */
#ifndef UMICOM_DATA_STRUCTURED_FORM_UPGRADE_H
#define UMICOM_DATA_STRUCTURED_FORM_UPGRADE_H
#include "umicom/application/experience/structured_form_edit.h"
#include "umicom/data/record_draft_snapshot.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStructuredFormUpgrade UmiStructuredFormUpgrade;
typedef struct UmiStructuredFormUpgradeView {
  uint64_t revision, source_schema_version, target_schema_version;
  size_t retained_fields, added_fields, removed_unanswered_fields;
  bool committed;
  UmiRecordDraftState record;
  UmiStructuredForm proposed;
} UmiStructuredFormUpgradeView;
/* Copied worker input binds the original historical document and the proposed
 * bytes. Keep unchanged until the worker finishes. No schema pointers travel
 * to the worker. Allocate these large bounded values on the heap. */
typedef struct UmiStructuredFormUpgradeRequest {
  UmiRecordDraftSnapshot snapshot;
  UmiRecordSaveRequest save;
} UmiStructuredFormUpgradeRequest;
/* Verify the source document and use the existing conservative form migrator.
 * Same form ID and a strictly newer target version are required. Removed known
 * answers, changed units/types/formats and invalid retained answers are refused.
 * This copies source state; source inputs need only remain stable until return.
 * The immutable target schema is borrowed until destruction. No I/O occurs.
 * The returned proposal is dirty and requires explicit review and attribution. */
UmiStatus umi_structured_form_upgrade_create(const UmiStructuredFormSchema *source_schema,
                                             const UmiRecordDocument *document,
                                             const UmiStructuredFormSchema *target_schema,
                                             UmiStructuredFormUpgrade **out);
void umi_structured_form_upgrade_destroy(UmiStructuredFormUpgrade *upgrade);
UmiStatus umi_structured_form_upgrade_snapshot(const UmiStructuredFormUpgrade *upgrade,
                                               UmiStructuredFormUpgradeView *out);
/* Edit the proposed target form with a checked changeset. All failures preserve
 * proposal and draft bytes. Pending saves are BUSY; committed reviews are closed.
 * Empty edits keep the review revision unchanged. Calls are owner-serialized. */
UmiStatus umi_structured_form_upgrade_apply(UmiStructuredFormUpgrade *upgrade,
                                            uint64_t expected_revision,
                                            const UmiStructuredFormEdit *edit);
/* Capture a reviewed save request. submitting=1 requires all mandatory answers;
 * zero permits an incomplete draft. Preparation has no storage side effects. */
UmiStatus umi_structured_form_upgrade_prepare_save(UmiStructuredFormUpgrade *upgrade,
                                                   uint64_t expected_revision,
                                                   const UmiRecordDraftAttribution *attribution,
                                                   int submitting,
                                                   UmiStructuredFormUpgradeRequest *out);
/* Worker: check exact historical source metadata/content in the chosen store,
 * then use normal revision-checked append. A legitimate concurrent append is
 * refused by the final head check. This is not a signed store identity or an
 * authorization boundary against a writer who can rewrite raw history. */
UmiStatus umi_structured_form_upgrade_request_commit(UmiDataServer *server,
                                                     const UmiStructuredFormUpgradeRequest *request,
                                                     UmiRecordRevision *out);
/* Matching completion closes a successful review. Original revisions remain
 * readable under their original schemas. Wrong replies retain pending state.
 * A failed save retains the proposal; verify ambiguous I/O outcomes before retry.
 * Destruction cannot cancel a worker. Drain/disconnect workers before destroying. */
UmiStatus umi_structured_form_upgrade_finish_save(UmiStructuredFormUpgrade *upgrade,
                                                  uint64_t request_id, UmiStatus result,
                                                  const UmiRecordRevision *saved);
#ifdef __cplusplus
}
#endif
#endif
