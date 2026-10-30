/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/video_sequence_private.h
 *
 * PURPOSE:
 *   Separate worker ownership, persistent sequence state and native workspace controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_VIDEO_SEQUENCE_GTK_PRIVATE_H
#define UMICOM_VIDEO_SEQUENCE_GTK_PRIVATE_H
#include "umicom/creative_workspace/video_render.h"
#include "umicom/ui/gtk4/path_picker.h"
#include "umicom/ui/gtk4/video_sequence.h"
typedef struct UmiVideoSequencePanel {
  UmiVideoSequenceEditor *editor;
  UmiVideoSequence saved; /* Last successfully saved/opened recipe, or initial empty state. */
  GtkWidget *root, *form, *status, *summary, *selector, *title, *width, *height;
  GtkWidget *id, *path, *begin, *end, *split, *rightId, *position;
  GtkWidget *projectPath, *approval, *program, *output, *cancelButton, *video;
  GtkWidget *soundtrackVideo, *soundtrackAudio, *soundtrackOutput;
  GtkWidget *captionVideo, *captionSource, *captionOutput;
  GtkWidget *relinkPath, *relinkAll;
  /* Stable clip controls retain focus while the model supplies their identity. */
  GtkWidget *clipButtons[UMI_VIDEO_SEQUENCE_MAX_CLIPS], *durationLabel, *emptyTimeline;
  /* Cards and the timeline share the editor; navigation never owns a second recipe. */
  GtkWidget *libraryButtons[UMI_VIDEO_SEQUENCE_MAX_CLIPS], *libraryLabels[UMI_VIDEO_SEQUENCE_MAX_CLIPS];
  GtkWidget *sourceBook, *inspectorBook, *search, *zoom, *emptyLibrary;
  bool browsing;
  /* Geometry belongs to the workspace, never the media recipe or edit history. */
  GtkWidget *layoutPanes[3], *layoutRegions[3], *layoutChecks[3];
  int rememberedPositions[3];
  bool rememberedVisible[3], rememberedLayout, arranging;
  GtkMediaStream *stream;
  UmiCancellationToken *cancel; /* Borrowed from the accepted job until completion. */
  uint32_t previewBegin, previewEnd;
  bool closed, busy, dirty, refreshing, changing, waitingForSeek, prepared;
} UmiVideoSequencePanel;
UmiVideoSequencePanel *UmiVideoSequenceGtkState(GtkWidget *root);
void UmiVideoSequenceGtkRefresh(UmiVideoSequencePanel *panel);
void UmiVideoSequenceGtkMessage(UmiVideoSequencePanel *panel, const char *message);
void UmiVideoSequenceGtkJob(GtkButton *button, gpointer root);
void UmiVideoSequenceGtkPreview(GtkButton *button, gpointer root);
void UmiVideoSequenceGtkStopPreview(UmiVideoSequencePanel *panel);
void UmiVideoSequenceGtkRelink(GtkButton *button, gpointer root);
#endif
