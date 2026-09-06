/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_window_identity_gtk4.c
 *
 * PURPOSE:
 *   Verify that canonical embedded ICO resources reach a GTK Win32 window's
 *   native small and large icon slots without presenting or replacing chrome.
 *
 * AUTHOR AND ORGANISATION:
 *   Sammy Hegab
 *   Umicom Foundation
 *
 * LICENCE:
 *   MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/gtk4/workstation/shell_header.h"

#include <stdio.h>

#if defined(G_OS_WIN32) && defined(GDK_WINDOWING_WIN32)
#include <windows.h>
#include <gdk/win32/gdkwin32.h>

/* Keep assertions active in release builds and dispose the unpresented window. */
#define REQUIRE(expression) do { \
    if (!(expression)) { \
        (void)fprintf(stderr, "Line %d: %s\n", __LINE__, #expression); \
        failed = 1; \
        goto cleanup; \
    } \
} while (0)

/* Inspect native pixels and release the bitmap copies returned by GetIconInfo.
 * Checking both sizes catches shared-resource caches returning a small icon
 * for the large window slot even though both WM_GETICON calls are non-NULL. */
static bool icon_has_size(HICON icon, int width, int height)
{
    ICONINFO info = {0};
    BITMAP bitmap = {0};
    bool matches = false;

    if (icon == NULL || !GetIconInfo(icon, &info)) return false;
    if (info.hbmColor != NULL && GetObjectW(info.hbmColor, sizeof(bitmap), &bitmap) != 0)
        matches = bitmap.bmWidth == width && bitmap.bmHeight == height;
    if (info.hbmColor != NULL) (void)DeleteObject(info.hbmColor);
    if (info.hbmMask != NULL) (void)DeleteObject(info.hbmMask);
    return matches;
}

/* Realization creates a native handle for inspection but does not map a window.
 * This test never opens a product, starts a process or calls window_present. */
int main(void)
{
    GtkWindow *window = NULL;
    GtkWidget *titlebar;
    GdkSurface *surface;
    HWND handle;
    HICON observed_small;
    HICON observed_large;
    int failed = 0;

    if (!gtk_init_check() || !GDK_IS_WIN32_DISPLAY(gdk_display_get_default())) {
        (void)puts("SKIP: GTK Win32 display is unavailable.");
        return 77;
    }
    REQUIRE(umi_gtk4_ws_apply_window_identity(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    REQUIRE(FindResourceW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(101),
        MAKEINTRESOURCEW(14)) != NULL); /* RT_GROUP_ICON */
    window = GTK_WINDOW(gtk_window_new());
    g_object_ref_sink(window);
    titlebar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_window_set_titlebar(window, titlebar);
    REQUIRE(!gtk_widget_get_realized(GTK_WIDGET(window)));
    REQUIRE(umi_gtk4_ws_apply_window_identity(window) == UMI_STATUS_OK);
    REQUIRE(umi_gtk4_ws_apply_window_identity(window) == UMI_STATUS_OK);
    REQUIRE(!gtk_widget_get_realized(GTK_WIDGET(window)));
    REQUIRE(gtk_window_get_titlebar(window) == titlebar);

    gtk_widget_realize(GTK_WIDGET(window));
    REQUIRE(!gtk_widget_get_mapped(GTK_WIDGET(window)));
    surface = gtk_native_get_surface(GTK_NATIVE(window));
    REQUIRE(surface != NULL && GDK_IS_WIN32_SURFACE(surface));
    handle = gdk_win32_surface_get_handle(surface);
    REQUIRE(handle != NULL);
    observed_small = (HICON)SendMessageW(handle, WM_GETICON, ICON_SMALL, 0);
    observed_large = (HICON)SendMessageW(handle, WM_GETICON, ICON_BIG, 0);
    REQUIRE(icon_has_size(observed_small,
        GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON)));
    REQUIRE(icon_has_size(observed_large,
        GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON)));

    /* Already-realized calls repair either slot without changing the titlebar. */
    (void)SendMessageW(handle, WM_SETICON, ICON_SMALL, 0);
    (void)SendMessageW(handle, WM_SETICON, ICON_BIG, 0);
    REQUIRE(umi_gtk4_ws_apply_window_identity(window) == UMI_STATUS_OK);
    REQUIRE((HICON)SendMessageW(handle, WM_GETICON, ICON_SMALL, 0) == observed_small);
    REQUIRE((HICON)SendMessageW(handle, WM_GETICON, ICON_BIG, 0) == observed_large);
    REQUIRE(gtk_window_get_titlebar(window) == titlebar);
    REQUIRE(!gtk_widget_get_mapped(GTK_WIDGET(window)));

cleanup:
    if (window != NULL) {
        gtk_window_destroy(window);
        g_object_unref(window);
    }
    /* The GTK window owns and releases the icon handles; the test borrows them. */
    return failed;
}
#else
/* A non-Win32 GTK backend cannot verify HWND icon slots. */
int main(void)
{
    (void)puts("SKIP: GTK Win32 backend is unavailable.");
    return 77;
}
#endif
