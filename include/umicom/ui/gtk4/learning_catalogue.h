/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/learning_catalogue.h
 *
 * PURPOSE:
 *   Share an offline learning catalogue and verified text reader across products.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_LEARNING_CATALOGUE_H
#define UMICOM_UI_GTK4_LEARNING_CATALOGUE_H
#include <gtk/gtk.h>
#include "umicom/teacher/learning_reader.h"
#include "umicom/teacher/learning_search.h"
#include "umicom/teacher/learning_help.h"
#include "umicom/teacher/learning_practice.h"
#include "umicom/platform/path.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiLearningCatalogueGtkState {
    size_t pack_count;
    UmiLearningSearchInfo search;
    UmiLearningReaderSnapshot reader;
    UmiStatus last_status;
    int busy;
    int retired;
    int has_copy_result;
} UmiLearningCatalogueGtkState;

/* Return a floating widget, or NULL on controller allocation failure. Call all
 * functions on the GTK owner thread. Keep the internal widget hierarchy intact.
 * host_version is the embedding product's compatibility version. No directory
 * is scanned, file read, project opened or command run during creation.
 * The initial presentation is a text reader: HTML gets a bounded plain-text
 * preview; images and binary/invalid UTF-8 content receive explicit notices.
 * Rich illustrated rendering is a separate capability, not implied here. */
GtkWidget *UmiLearningCatalogueGtk4Create(UmiVersion host_version);

/* Present a new catalogue window tied to parent and its GtkApplication.
 * Return a borrowed window, or NULL for invalid parent/allocation failure.
 * Closing either window retires the panel. No pack is loaded implicitly.
 * For application-backed windows, each accepted read holds the application
 * until GTK completion so closing the last window cannot strand cleanup.
 * Hosts without GtkApplication must drain PendingReads themselves.
 * Explicit g_application_quit ignores holds: drain accepted jobs before that
 * call. Practice-copy jobs follow the same hold and completion contract. */
GtkWindow *UmiLearningCatalogueGtk4Present(GtkWindow *parent, UmiVersion host_version);

/* Accept a copied absolute root and asynchronously load learning-pack.json.
 * One job is admitted per panel. Duplicate IDs preserve the existing pack and
 * return ALREADY_EXISTS through last_status. Loading additional packs retains
 * the current reader. A failed operation leaves earlier catalogue data intact.
 * OK means dispatched, not completed; inspect CopyState after busy becomes zero.
 * Native reads can block. There is no promise to interrupt an accepted read. */
UmiStatus UmiLearningCatalogueGtk4Load(GtkWidget *widget, const char *absolute_root);

/* Search copied metadata synchronously, then explicitly open a displayed index
 * asynchronously. Indices belong to the current result set, not persistent IDs.
 * Search matches the Framework's bounded ASCII-folded metadata profile. */
UmiStatus UmiLearningCatalogueGtk4Search(GtkWidget *widget, const char *query);
UmiStatus UmiLearningCatalogueGtk4Open(GtkWidget *widget, size_t result_index);
/* Resolve an explicit application-owned help route against loaded packs, then
 * open its guide on the existing asynchronous reader path. Map and request are
 * borrowed only until this call returns. Search results need not contain the
 * target and are left unchanged. No pack is loaded or downloaded implicitly.
 * Accepted work owns copied identity and pack metadata; OK is dispatch only. */
UmiStatus UmiLearningCatalogueGtk4OpenHelp(GtkWidget *widget, const UmiLearningHelpMap *map,
    const UmiLearningHelpRequest *request);
UmiStatus UmiLearningCatalogueGtk4Back(GtkWidget *widget);
UmiStatus UmiLearningCatalogueGtk4Forward(GtkWidget *widget);
UmiStatus UmiLearningCatalogueGtk4Reload(GtkWidget *widget);


typedef struct UmiLearningCatalogueCopyReceipt {
    UmiLearningPackInfo pack;
    UmiLearningPackResource selection;
    char destination_root[UMI_PATH_CAPACITY];
    char destination_path[UMI_PATH_CAPACITY];
    UmiLearningPracticeResult result;
} UmiLearningCatalogueCopyReceipt;

/* Resolve the currently displayed resource's example without reading/writing.
 * Refusal leaves out_example unchanged. A guide needs an explicit example link. */
UmiStatus UmiLearningCatalogueGtk4PreviewExample(GtkWidget *widget,
    UmiLearningPackResource *out_example);

/* Accept an explicit copy of that example into a NEW relative file below an
 * existing absolute destination root. Captures immutable pack/selection and
 * copied paths before dispatch. Rechecks source bytes on the worker. No parent
 * directories are created, existing files are never overwritten, and no source
 * is compiled or run. The native adapter limits the source to 1 MiB.
 *
 * OK means accepted, not completed. Closing does not undo an accepted copy:
 * it may finish after retirement and can leave a partial newly created file.
 * Keep the application context running for completion, then inspect the file.
 * The current reader/history is unchanged, including on copy failure. */
UmiStatus UmiLearningCatalogueGtk4CopyExample(GtkWidget *widget,
    const char *destination_root, const char *destination_path);

/* Copy the last completed copy result with its captured identity and paths.
 * NOT_FOUND means no copy result is available; refusal preserves output.
 * A subsequent read/search does not relabel or discard this receipt. */
UmiStatus UmiLearningCatalogueGtk4CopyLastReceipt(GtkWidget *widget,
    UmiLearningCatalogueCopyReceipt *out_receipt);

/* Metadata copies remain valid independently of widgets. Refusal preserves out.
 * busy remains set during result publication to refuse reentrant operations.
 * Retire before destroying a host that externally retains this widget. It
 * immediately disables new operations; accepted reads finish privately and
 * discard results. Keep the GTK main context running for task cleanup.
 * Repeated Retire is harmless; a retired panel cannot be reopened. */
UmiStatus UmiLearningCatalogueGtk4CopyState(GtkWidget *widget, UmiLearningCatalogueGtkState *out);
void UmiLearningCatalogueGtk4Retire(GtkWidget *widget);

/* Count accepted file jobs (reads or explicit practice copies) whose GTK completion has not finished, across catalogue
 * panels. A host can retire its panels and keep iterating the GTK context until
 * this reaches zero during orderly shutdown. Call on the GTK owner thread. */
size_t UmiLearningCatalogueGtk4PendingReads(void);

#ifdef __cplusplus
}
#endif
#endif
