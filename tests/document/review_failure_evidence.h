/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/document/review_failure_evidence.h
 * Purpose: Preserve native review outcome evidence when an assertion fails.
 * Author and Organisation: Sammy Hegab, Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_TEST_REVIEW_FAILURE_EVIDENCE_H
#define UMICOM_TEST_REVIEW_FAILURE_EVIDENCE_H
#include "umicom/ui/gtk4/automation.h"
#include <gtk/gtk.h>
#include <stdio.h>

/* Only synthetic fixture state is printed. No editor source, credentials or
 * arbitrary widget text is enumerated. Keep failure assertions authoritative;
 * this evidence explains whether a worker failed or a result was retired. */
static void UmiTestReviewFailureEvidence(GtkWindow *dialog, const char *scenario)
{
    fprintf(stderr, "Review scenario: %s\n", scenario == NULL ? "unknown" : scenario);
    if (dialog == NULL) {
        fputs("Review window was not created.\n", stderr);
        return;
    }
    GtkWidget *window = GTK_WIDGET(dialog);
    GtkWidget *message = umi_gtk4_automation_find_tagged_widget(window, "document.completion.message");
    fprintf(stderr, "Review state: pending=%d visible=%d rooted=%d\n",
        g_object_get_data(G_OBJECT(window), "umicom-completion-pending") != NULL,
        gtk_widget_get_visible(window), gtk_widget_get_root(window) != NULL);
    if (GTK_IS_LABEL(message)) {
        const unsigned char *text = (const unsigned char *)gtk_label_get_text(GTK_LABEL(message));
        fputs("Review result: ", stderr);
        /* Bound and escape the observed message so diagnostics cannot change
         * terminal state or manufacture another result line. */
        for (size_t i = 0U; text[i] != 0U && i < 2048U; ++i) {
            if (text[i] < 32U || text[i] == 127U) fprintf(stderr, "\\x%02X", (unsigned)text[i]);
            else fputc(text[i], stderr);
        }
        fputc('\n', stderr);
    }
    fflush(stderr);
}
#endif
