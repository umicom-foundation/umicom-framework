/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/creative_workspace_private.h
 * PURPOSE: Keep GTK widgets and callback ownership out of the creative domain model.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_CREATIVE_WORKSPACE_GTK_PRIVATE_H
#define UMICOM_CREATIVE_WORKSPACE_GTK_PRIVATE_H
#include "umicom/ui/gtk4/creative_workspace.h"
#include "umicom/creative_workspace/scene.h"
#include "umicom/creative_workspace/export.h"
#include <stdint.h>
struct UmiCreativeGtkPanel {
    UmiDataServer * server;
    bool ownsServer, refreshing, frameValid, playActive;
    UmiCreativeWorkspace * workspace;
    UmiCreativeProject * project;
    UmiCreativeFrame frame;
    UmiCreativePlayState play;
    char profile[32];
    GtkWidget * root, * canvas, * status, * storage, * timeline, * report;
    GtkWidget * projectId, * title, * unit, * width, * height, * duration, * tempo;
    GtkWidget * elementId, * label, * kind, * x, * y, * objectWidth, * objectHeight, * colour, * visible;
    GtkWidget * keyTime, * offsetX, * offsetY, * interpolation;
    GtkWidget * noteId, * startTick, * durationTicks, * pitch, * velocity;
    GtkWidget * exportKind, * exportPath;
    GPtrArray * connections;
};

void UmiCreativeGtkTrack(UmiCreativeGtkPanel * panel, GObject * object);
void UmiCreativeGtkAction(GtkButton * button, gpointer data);
void UmiCreativeGtkStatus(UmiCreativeGtkPanel * panel, UmiStatus status, const char * operation);
void UmiCreativeGtkRefresh(UmiCreativeGtkPanel * panel);
void UmiCreativeGtkFrameRefresh(UmiCreativeGtkPanel * panel);
void UmiCreativeGtkDraw(GtkDrawingArea * area, cairo_t * cr, int width, int height, gpointer data);
void UmiCreativeGtkPressed(GtkGestureClick * gesture, int presses, double x, double y, gpointer data);
void UmiCreativeGtkTimelineChanged(GtkRange * range, gpointer data);
void UmiCreativeGtkLoadObject(UmiCreativeGtkPanel * panel);
#endif
