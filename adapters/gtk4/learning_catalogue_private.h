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
    GtkWidget *root, *controls, *path, *query, *results;
    GtkWidget *heading, *status, *content;
} LearningCatalogue;
typedef enum LearningCatalogueAction {
    LEARNING_CATALOGUE_LOAD,
    LEARNING_CATALOGUE_OPEN,
    LEARNING_CATALOGUE_BACK,
    LEARNING_CATALOGUE_FORWARD,
    LEARNING_CATALOGUE_RELOAD
} LearningCatalogueAction;
typedef struct LearningCatalogueJob {
    GWeakRef root;
    LearningCatalogueAction action;
    UmiVersion host;
    char directory[UMI_PATH_CAPACITY];
    UmiLearningSearchHit selected;
    UmiLearningShelf *selection_shelf;
    UmiLearningPack *pack;
    UmiLearningReader *reader;
    UmiLearningReaderSnapshot snapshot;
    UmiStatus status;
    char *display;
    char notice[256];
} LearningCatalogueJob;
LearningCatalogue *LearningCatalogueOwner(GtkWidget *widget);
void LearningCatalogueMessage(LearningCatalogue *owner, UmiStatus status, const char *detail);
UmiStatus LearningCatalogueRefresh(LearningCatalogue *owner, const char *query);
UmiStatus LearningCatalogueDispatch(LearningCatalogue *owner, LearningCatalogueJob *job);
void LearningCatalogueJobFree(gpointer data);
#endif
