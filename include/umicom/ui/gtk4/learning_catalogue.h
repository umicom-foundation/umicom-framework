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
} UmiLearningCatalogueGtkState;

/* Return a floating widget, or NULL on controller allocation failure. Call all
 * functions on the GTK owner thread. Keep the internal widget hierarchy intact.
 * host_version is the embedding product's compatibility version. No directory
 * is scanned, file read, project opened or command run during creation.
 * The initial presentation is a text reader: HTML gets a bounded plain-text
 * preview; images and binary/invalid UTF-8 content receive explicit notices.
 * Rich illustrated rendering is a separate capability, not implied here. */
GtkWidget *UmiLearningCatalogueGtk4Create(UmiVersion host_version);

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
UmiStatus UmiLearningCatalogueGtk4Back(GtkWidget *widget);
UmiStatus UmiLearningCatalogueGtk4Forward(GtkWidget *widget);
UmiStatus UmiLearningCatalogueGtk4Reload(GtkWidget *widget);

/* Metadata copies remain valid independently of widgets. Refusal preserves out.
 * busy remains set during result publication to refuse reentrant operations.
 * Retire before destroying a host that externally retains this widget. It
 * immediately disables new operations; accepted reads finish privately and
 * discard results. Keep the GTK main context running for task cleanup.
 * Repeated Retire is harmless; a retired panel cannot be reopened. */
UmiStatus UmiLearningCatalogueGtk4CopyState(GtkWidget *widget, UmiLearningCatalogueGtkState *out);
void UmiLearningCatalogueGtk4Retire(GtkWidget *widget);

#ifdef __cplusplus
}
#endif
#endif
