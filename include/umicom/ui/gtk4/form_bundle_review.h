/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
 * A native preview copies all historical answers before asking for confirmation. */
#ifndef UMICOM_UI_GTK4_FORM_BUNDLE_REVIEW_H
#define UMICOM_UI_GTK4_FORM_BUNDLE_REVIEW_H
#include "umicom/data/structured_form_bundle.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/* Construct an owned read-only display of every revision. Input review,
 * catalogue and schemas need remain valid only until return; no pointers to
 * them survive in this widget. All metadata and fields must be displayable UTF-8;
 * secret fields are refused. The floating root belongs to the caller/parent.
 * No file access, database writes, authorization or worker scheduling occurs.
 * Output remains unchanged on error. All methods run on the GTK owner thread. */
UmiStatus UmiFormBundleReviewGtk4New(const UmiStructuredFormBundleReview *review, GtkWidget **out);
/* Read copied identity for display without implying approval. */
UmiStatus UmiFormBundleReviewGtk4Summary(GtkWidget *widget, UmiStructuredFormBundleSummary *out);
/* Return the same captured identity only after the visible checkbox is selected.
 * The host must authorize the destination separately, then use this digest with
 * its retained immutable review on a worker. Never fetch a new file implicitly.
 * Unchecked or permanently retired widgets return INVALID_STATE. */
UmiStatus UmiFormBundleReviewGtk4Confirmed(GtkWidget *widget, UmiStructuredFormBundleSummary *out);
/* Unroot/dispose/Retire permanently revoke confirmation. Earlier copied summaries
 * are plain data; the host must revoke any pending action when its view closes. */
void UmiFormBundleReviewGtk4Retire(GtkWidget *widget);
#ifdef __cplusplus
}
#endif
#endif
