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
    UmiLearningSearch *search;
    UmiLearningReader *reader;
    UmiLearningCatalogueGtkState state;
    UmiLearningCatalogueCopyReceipt copy;
    GtkWidget *root, *controls, *path, *query, *results;
    GtkWidget *link;
    GtkWidget *heading, *status, *content, *destination, *copy_status;
} LearningCatalogue;
typedef enum LearningCatalogueAction {
    LEARNING_CATALOGUE_LOAD,
    LEARNING_CATALOGUE_OPEN,
    LEARNING_CATALOGUE_BACK,
    LEARNING_CATALOGUE_FORWARD,
    LEARNING_CATALOGUE_RELOAD,
    LEARNING_CATALOGUE_COPY,
    LEARNING_CATALOGUE_FOLLOW
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
    UmiLearningReaderSnapshot snapshot;
    UmiLearningCatalogueCopyReceipt copy;
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
#endif
