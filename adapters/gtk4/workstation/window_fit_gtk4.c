/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: adapters/gtk4/workstation/window_fit_gtk4.c
 *
 * PURPOSE:
 *   Implement the shared monitor-aware initial sizing rule used by native
 *   Umicom workstations. Keeping this calculation in Framework prevents Bank,
 *   Trader, TMS, Music, Desk and Studio from drifting apart visually.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/gtk4/workstation/window_fit.h"

#include <stddef.h>

/* Reserve a modest allowance for desktop panels and window decorations.
 * GDK monitor geometry is not the platform-specific usable work area. */
#define UMI_GTK4_WINDOW_FIT_MARGIN 80

/* Keep one integer inside an inclusive range. The helper is deliberately
 * local so callers cannot accidentally apply a different sizing policy. */
static int clamp_dimension(int value, int minimum, int maximum)
{
    if (maximum < minimum) return maximum;
    if (value < minimum) return minimum;
    if (value > maximum) return maximum;
    return value;
}

/* Apply one bounded dimension while allowing very small monitors to win over
 * an otherwise impossible product minimum. */
static int fit_dimension(int preferred, int minimum, int available)
{
    int safe_minimum = minimum;

    if (available <= 0) available = 1;
    if (safe_minimum > available) safe_minimum = available;
    return clamp_dimension(preferred, safe_minimum, available);
}

/*
 * Choose and apply a monitor-aware default size without forcing maximisation.
 * The operating system remains responsible for the final placement and the
 * user retains normal resize behaviour after this initial hint is applied.
 */
UmiStatus umi_gtk4_ws_window_fit(
    GtkWindow *window,
    int preferred_width,
    int preferred_height,
    int minimum_width,
    int minimum_height)
{
    GdkDisplay *display;
    GListModel *monitors;
    GdkSeat *seat;
    GdkDevice *pointer;
    GdkSurface *surface;
    GdkMonitor *monitor = NULL;
    gboolean monitor_owned = FALSE;
    GdkRectangle geometry;
    int width;
    int height;

    if (window == NULL || !GTK_IS_WINDOW(window) ||
        preferred_width <= 0 || preferred_height <= 0 ||
        minimum_width <= 0 || minimum_height <= 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    width = preferred_width;
    height = preferred_height;
    display = gtk_widget_get_display(GTK_WIDGET(window));
    surface = gtk_native_get_surface(GTK_NATIVE(window));
    if (display != NULL && surface != NULL) {
        monitor = gdk_display_get_monitor_at_surface(display, surface);
    }
    /* Before the window is realised, use a surface under the pointer when
     * GDK knows it. Other applications' surfaces may not be available. */
    if (monitor == NULL && display != NULL) {
        seat = gdk_display_get_default_seat(display);
        pointer = seat != NULL ? gdk_seat_get_pointer(seat) : NULL;
        surface = pointer != NULL ? gdk_device_get_surface_at_position(
            pointer, NULL, NULL) : NULL;
        if (surface != NULL) {
            monitor = gdk_display_get_monitor_at_surface(display, surface);
        }
    }
    if (monitor == NULL && display != NULL) {
        monitors = gdk_display_get_monitors(display);
        if (monitors != NULL && g_list_model_get_n_items(monitors) > 0U) {
            monitor = GDK_MONITOR(g_list_model_get_item(monitors, 0U));
            monitor_owned = monitor != NULL;
        }
    }
    if (monitor != NULL) {
        gdk_monitor_get_geometry(monitor, &geometry);
        width = fit_dimension(
            preferred_width,
            minimum_width,
            geometry.width - UMI_GTK4_WINDOW_FIT_MARGIN);
        height = fit_dimension(
            preferred_height,
            minimum_height,
            geometry.height - UMI_GTK4_WINDOW_FIT_MARGIN);
        /* Surface monitor lookups are borrowed. Only the list lookup returns
         * a reference owned by this function. */
        if (monitor_owned) g_object_unref(monitor);
    }

    gtk_window_set_default_size(window, width, height);
    gtk_window_set_resizable(window, TRUE);
    return UMI_STATUS_OK;
}
