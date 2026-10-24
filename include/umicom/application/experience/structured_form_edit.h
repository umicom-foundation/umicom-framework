/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/experience/structured_form_edit.h
 * PURPOSE: Capture and apply atomic multi-field edits with reviewed old answers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_EDIT_H
#define UMICOM_APPLICATION_EXPERIENCE_STRUCTURED_FORM_EDIT_H
#include "umicom/application/experience/structured_form.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiStructuredFormChange {
  char field_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  UmiFormAnswer before, after;
} UmiStructuredFormChange;
typedef struct UmiStructuredFormEdit {
  char form_id[UMI_APPLICATION_EXPERIENCE_ID_CAPACITY];
  uint64_t schema_version, base_revision;
  size_t count;
  UmiStructuredFormChange changes[UMI_APPLICATION_EXPERIENCE_FIELD_CAPACITY];
} UmiStructuredFormEdit;
/* Capture semantic differences without retaining borrowed pointers. Use heap
 * storage for edits in histories. Outputs are unchanged on any refusal. */
UmiStatus umi_structured_form_edit_capture(const UmiStructuredFormSchema *schema,
                                           const UmiStructuredForm *before,
                                           const UmiStructuredForm *after,
                                           UmiStructuredFormEdit *out);
/* Validate every identity, permission and old/new answer before mutation.
 * One nonempty batch advances the revision once, regardless of field count.
 * Empty batches are no-ops. Saved revision is retained, so changes stay dirty. */
UmiStatus umi_structured_form_edit_apply(const UmiStructuredFormSchema *schema,
                                         UmiStructuredForm *form,
                                         const UmiStructuredFormEdit *edit);
/* Prepare a reviewed undo immediately after a successful application. Applying
 * the inverse advances revision again; it never restores an old revision token. */
UmiStatus umi_structured_form_edit_inverse(const UmiStructuredFormSchema *schema,
                                           const UmiStructuredForm *form,
                                           const UmiStructuredFormEdit *applied,
                                           UmiStructuredFormEdit *out);
#ifdef __cplusplus
}
#endif
#endif
