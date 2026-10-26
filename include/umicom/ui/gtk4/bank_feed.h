/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/bank_feed.h
 *
 * PURPOSE:
 *   Open a local bank transaction review using reusable Framework services.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_BANK_FEED_H
#define UMICOM_UI_GTK4_BANK_FEED_H
#include <gtk/gtk.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
/** Create a floating launcher for the local bank transaction inspector.
 * Statement CSV is also supported through the schema in UmiBankFeedCsvCreate;
 * the window does not guess a bank's export layout.
 * Custom CSV columns use UmiBankFeedCsvMapping. Settings remain in memory for
 * this window; their value copy belongs to the import worker that captured them.
 * A click opens a window attached to the live parent. Users explicitly select
 * a saved Open Banking JSON response; loading never contacts a bank or posts
 * ledger entries. The window shows per-account/currency totals and exports
 * the imported observations. Closing retires retained controls and late loads. */
/* Reusable observation archives use UmiBankFeedArchiveEncode/Decode. Select the
 * archive format explicitly to reopen retained rows and original input digest.
 * The archive exporter includes all accounts in this import, uses exclusive
 * creation and retains no original path. A restored digest is a claim, not
 * provider authentication. CSV reports remain a separate export choice.
 * The native exporter additionally limits output to 2,097,152 characters. */
/* Retain current import as before captures an independent in-memory snapshot.
 * It survives changing the current file, format, mapping and Clear report.
 * Compare retained import opens an immutable review against the current accepted
 * import, including all accounts. Forget retained import releases that side;
 * closing releases both. No snapshot is saved automatically and no rows merge. */
GtkWidget *UmiBankFeedGtk4LauncherCreate(void);
/** Create an unpresented bank transaction window owned by GTK. parent may be
 * NULL; a removed parent returns NULL. The caller presents it and eventually destroys it. Import
 * work runs off the GTK thread; each result belongs to the path generation that started it.
 * Changing the selected format also retires exports and pending imports.
 * Editing any column mapping or fallback value applies the same retirement.
 * Changing path or clearing the view invalidates earlier work. */
GtkWindow *UmiBankFeedGtk4Create(GtkWindow *parent);
/** Return whether an import worker is still pending, including after native
 * window removal when the caller retains the window. Call on the GTK thread.
 * A destroyed result is discarded; this is not a network connection status. */
bool UmiBankFeedGtk4Busy(GtkWindow *window);
#ifdef __cplusplus
}
#endif
#endif
