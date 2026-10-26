/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/bank_comparison.h
 *
 * PURPOSE:
 *   Review immutable imported-bank differences in a shared native window.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_BANK_COMPARISON_H
#define UMICOM_UI_GTK4_BANK_COMPARISON_H
#include <gtk/gtk.h>
#include "umicom/bank_feed/comparison.h"
#ifdef __cplusplus
extern "C" {
#endif
/** Create an unpresented window with independent copies of both imports.
 * NULL inputs, removed parent or comparison failure returns NULL. parent may
 * be NULL. GTK owns the window; the caller presents and eventually destroys it.
 * Inputs are borrowed only during construction. Subsequent source replacement
 * cannot alter the review. "Before" and "after" mean the caller's chosen sides,
 * not proven chronology. The view includes every account in both imports.
 * Rows keep exact identities, original indices and explicit unmatched/unknown
 * states. Read-only filtering never changes the underlying comparison.
 * CSV export includes both sides, even when the visible view hides same rows.
 * Export limits may refuse a large report without hiding the native review.
 * No bank contact, posting, import merging or balance calculation occurs.
 * Call on the GTK owner thread. Closing retires retained export controls. */
GtkWindow *UmiBankFeedComparisonGtk4Create(const UmiBankFeed *before,
    const UmiBankFeed *after, GtkWindow *parent);
#ifdef __cplusplus
}
#endif
#endif
