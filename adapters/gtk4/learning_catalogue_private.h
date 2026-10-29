/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/learning_catalogue_private.h
 *
 * PURPOSE:
 *   Separate GTK owner state from copied asynchronous learning requests.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_LEARNING_CATALOGUE_PRIVATE_H
#define UMICOM_LEARNING_CATALOGUE_PRIVATE_H
#include "umicom/ui/gtk4/learning_catalogue.h"
#include "umicom/platform/path.h"
#define UMI_LEARNING_CATALOGUE_DATA "umicom-learning-catalogue-owner"
#define UMI_LEARNING_CATALOGUE_BYTES (1024U * 1024U)
typedef struct LearningCatalogue {
    UmiVersion host;
    UmiLearningShelf *shelf;
    UmiLearningShelf *replacement_shelf;
    UmiLearningCatalogueReplacement replacement;
    UmiLearningPackChanges *replacement_changes;
    uint64_t review_serial;
    UmiLearningSearch *search;
    UmiLearningReader *reader;
    UmiBrowserReadingDocument *reading_document;
    GtkWidget *reading_links, *reading_anchor, *reading_zoom;
    unsigned int reading_zoom_percent;
    UmiLearningCatalogueGtkState state;
    UmiLearningCatalogueCopyReceipt copy;
    UmiLearningProgressLocalReceipt progress_receipt;
    UmiLearningLibraryChoices *library_choices;
    UmiLearningCatalogueLibraryReceipt library_receipt;
    char progress_recovery_path[UMI_PATH_CAPACITY];
    char progress_recovery_learner[UMI_TEACHER_ID_CAPACITY];
    GtkWidget *progress_path, *progress_learner, *progress_checkpoint;
    GtkWidget *progress_evidence, *progress_status;
    GtkWidget *library_path, *library_rows, *library_status;
    GtkWidget *root, *controls, *path, *query, *results;
    GtkWidget *link, *replacement_status, *replacement_details;
    GtkWidget *heading, *status, *content, *destination, *copy_status;
} LearningCatalogue;
typedef enum LearningCatalogueAction {
    LEARNING_CATALOGUE_LOAD,
    LEARNING_CATALOGUE_OPEN,
    LEARNING_CATALOGUE_BACK,
    LEARNING_CATALOGUE_FORWARD,
    LEARNING_CATALOGUE_RELOAD,
    LEARNING_CATALOGUE_COPY,
    LEARNING_CATALOGUE_FOLLOW,
    LEARNING_CATALOGUE_PREPARE_REPLACEMENT,
    LEARNING_CATALOGUE_PROGRESS,
    LEARNING_CATALOGUE_LIBRARIES_LOAD,
    LEARNING_CATALOGUE_LIBRARIES_SAVE,
    LEARNING_CATALOGUE_LIBRARY_REOPEN
} LearningCatalogueAction;
typedef struct LearningCatalogueJob {
    GWeakRef root;
    /* Held/released only on the GTK thread; never used by the worker. */
    GApplication *application;
    LearningCatalogueAction action;
    int continues_reader;
    UmiVersion host;
    char directory[UMI_PATH_CAPACITY];
    char link[UMI_TEACHER_PATH_CAPACITY + UMI_LEARNING_FRAGMENT_CAPACITY];
    UmiLearningSearchHit selected;
    UmiLearningShelf *selection_shelf;
    UmiLearningPack *pack;
    UmiLearningReader *reader;
    UmiBrowserReadingDocument *reading_document;
    UmiLearningReader *progress_reader;
    UmiLearningProgressLocalReceipt progress_receipt;
    UmiLearningLibraryChoices *library_choices;
    UmiLearningCatalogueLibraryReceipt library_receipt;
    UmiLearningReaderSnapshot snapshot;
    UmiLearningCatalogueCopyReceipt copy;
    UmiLearningCatalogueReplacement replacement;
    UmiLearningPackChanges *replacement_changes;
    UmiStatus status;
    char *display;
    char notice[256];
} LearningCatalogueJob;
LearningCatalogue *LearningCatalogueOwner(GtkWidget *widget);
void LearningCatalogueMessage(LearningCatalogue *owner, UmiStatus status, const char *detail);
UmiStatus LearningCatalogueRefresh(LearningCatalogue *owner, const char *query);
UmiStatus LearningCatalogueDispatch(LearningCatalogue *owner, LearningCatalogueJob *job);
void LearningCatalogueJobFree(gpointer data);
UmiStatus LearningCatalogueOpenSelection(LearningCatalogue *owner,
    const UmiLearningPackInfo *pack, const UmiLearningPackResource *resource);
/* Prepare copied metadata off-thread, then transfer it only to a live panel. */
void LearningCataloguePrepareReplacement(LearningCatalogueJob *job);
void LearningCatalogueCompleteReplacement(LearningCatalogue *owner, LearningCatalogueJob *job);
UmiStatus LearningCataloguePublishShelf(LearningCatalogue *owner,
    UmiLearningShelf **candidate, const char *query);
void LearningCatalogueProgressControls(LearningCatalogue *owner);
void LearningCatalogueProgressWorker(LearningCatalogueJob *job);
void LearningCatalogueProgressComplete(LearningCatalogue *owner, LearningCatalogueJob *job);
void LearningCatalogueLibraryControls(LearningCatalogue *owner);
void LearningCatalogueLibraryWorker(LearningCatalogueJob *job);
void LearningCatalogueLibraryComplete(LearningCatalogue *owner, LearningCatalogueJob *job);
void LearningCatalogueReadingControls(LearningCatalogue *owner);
void LearningCatalogueReadingPublish(LearningCatalogue *owner, LearningCatalogueJob *job);
#endif
