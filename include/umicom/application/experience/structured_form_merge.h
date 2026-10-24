/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * Review field-level concurrent changes before any persistence decision. */
#ifndef UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_MERGE_H
#define UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_MERGE_H
#include "umicom/application/experience/structured_form.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStructuredFormMerge UmiStructuredFormMerge;
typedef enum UmiFormMergeDifference {
  UMI_FORM_MERGE_UNCHANGED = 0,
  UMI_FORM_MERGE_LOCAL_ONLY = 1,
  UMI_FORM_MERGE_INCOMING_ONLY = 2,
  UMI_FORM_MERGE_SAME_CHANGE = 3,
  UMI_FORM_MERGE_CONFLICT = 4
} UmiFormMergeDifference;
typedef enum UmiFormMergeChoice {
  UMI_FORM_MERGE_AUTOMATIC = 0,
  UMI_FORM_MERGE_KEEP_BASE = 1,
  UMI_FORM_MERGE_KEEP_LOCAL = 2,
  UMI_FORM_MERGE_TAKE_INCOMING = 3,
  UMI_FORM_MERGE_CUSTOM = 4
} UmiFormMergeChoice;
typedef struct UmiFormMergeState {
  uint64_t revision, baseline_revision, local_revision, incoming_revision;
  size_t field_count, conflict_count, unresolved_count;
} UmiFormMergeState;
typedef struct UmiFormMergeField {
  char field_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  UmiFormMergeDifference difference;
  UmiFormMergeChoice choice;
  int resolved;
  UmiFormAnswer baseline, local, incoming, selected;
} UmiFormMergeField;
/* Copies all three input forms. The immutable schema remains borrowed. Calls
 * belong to one serialized owner. The caller must establish that the baseline
 * is a common ancestor for the SAME record; form IDs alone cannot prove this.
 * Every input must be valid as a draft and have this schema/version. A changed
 * read-only field requires a separate domain operation and is refused here.
 * Branch revision counters may coincide while their answers differ. Baseline
 * counters cannot exceed either branch; same-baseline revisions cannot carry
 * changed answers. No store, network, UI or callbacks are involved. */
UmiStatus umi_structured_form_merge_create(const UmiStructuredFormSchema *schema,
                                           const UmiStructuredForm *baseline,
                                           const UmiStructuredForm *local,
                                           const UmiStructuredForm *incoming,
                                           UmiStructuredFormMerge **out);
void umi_structured_form_merge_destroy(UmiStructuredFormMerge *review);
UmiStatus umi_structured_form_merge_state(const UmiStructuredFormMerge *review,
                                          UmiFormMergeState *out);
UmiStatus umi_structured_form_merge_field(const UmiStructuredFormMerge *review, size_t index,
                                          UmiFormMergeField *out);
/* Automatic chooses the single changed branch or a common identical change.
 * A true conflict remains unresolved until an explicit choice is made. Custom
 * values use the separate method, validated with the same type/unit rules.
 * Revision checking rejects selections from a stale review. No-op selections
 * do not advance the review token; all failures preserve the review. */
UmiStatus umi_structured_form_merge_select(UmiStructuredFormMerge *review,
                                           uint64_t expected_revision, const char *field_id,
                                           UmiFormMergeChoice choice);
UmiStatus umi_structured_form_merge_custom(UmiStructuredFormMerge *review,
                                           uint64_t expected_revision, const char *field_id,
                                           const UmiFormAnswer *answer);
/* Check that a later host decision still refers to these exact input snapshots.
 * This is a semantic comparison, not a concurrent store lock or authorization. */
UmiStatus umi_structured_form_merge_check_inputs(const UmiStructuredFormMerge *review,
                                                 const UmiStructuredForm *baseline,
                                                 const UmiStructuredForm *local,
                                                 const UmiStructuredForm *incoming);
/* Produce a proposed form only after all fields are resolved. If it equals the
 * incoming answers, that form is adopted clean. Otherwise its edit counter is
 * max(local,incoming)+1 and its saved counter is incoming.revision. Overflow is
 * refused. submitting=1 checks required answers; draft completion uses zero.
 * Output is unchanged on failure. The host must advance its selection context
 * and recheck the storage revision before adopting or saving this proposal. */
UmiStatus umi_structured_form_merge_finish(const UmiStructuredFormMerge *review,
                                           uint64_t expected_revision, int submitting,
                                           UmiStructuredForm *out);
#ifdef __cplusplus
}
#endif
#endif
