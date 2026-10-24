/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/structured_form.h
 * PURPOSE: Render and review browser controls through the shared form contracts.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_STRUCTURED_FORM_H
#define UMICOM_WEB_STRUCTURED_FORM_H
#include "umicom/application/experience/structured_form_edit.h"
#include "umicom/web/form_body.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_WEB_FORM_HTML_MAX_BYTES 524288U
#define UMI_WEB_FORM_TOKEN_CAPACITY 128U
/* The host authenticates/authorizes the user and supplies a nonempty,
 * session-bound CSRF token. These functions do not create sessions, validate
 * origins, authorize a record, perform network IO or persist a submission.
 * This adapter uses LF text; existing CR-bearing answers are refused during
 * render rather than normalized silently. Browser CRLF submission is decoded
 * to LF; bare CR is refused.
 * POST bodies are sensitive; the host must not copy them into diagnostic logs.
 * Action must be a rooted local path such as /forms/review, without query,
 * fragment, percent escapes or a second initial slash. No script is emitted.
 * All data is escaped as text/attributes. Known SECRET fields are refused:
 * password entry requires a dedicated non-reflecting workflow. */
UmiStatus umi_web_structured_form_render(const UmiStructuredFormSchema *schema,
                                         const UmiStructuredForm *form, const char *action,
                                         const char *csrf_token, char *out, size_t capacity,
                                         size_t *out_length);
/* Prepare an edit only; the current form and all outputs remain unchanged on
 * refusal. Require complete controls for every editable field and reject
 * unknown/read-only controls. form/version/revision bind to current state.
 * intent is draft or submit; successful submit includes required-field checks.
 * Read-only answers remain server-owned. Inactive value/reason controls stay
 * browser drafts and are ignored according to the explicit state selector.
 * The caller must serialize prepare+apply with other writes and use the shared
 * edit_apply function; its revision/old-value checks still run at application. */
UmiStatus umi_web_structured_form_prepare(const UmiStructuredFormSchema *schema,
                                          const UmiStructuredForm *current, const void *body,
                                          size_t length, const char *expected_csrf_token,
                                          UmiStructuredFormEdit *out_edit, int *out_submitting);
#ifdef __cplusplus
}
#endif
#endif
