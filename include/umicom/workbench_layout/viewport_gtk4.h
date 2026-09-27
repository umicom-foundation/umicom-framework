/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#ifndef UMICOM_WORKBENCH_LAYOUT_VIEWPORT_GTK4_H
#define UMICOM_WORKBENCH_LAYOUT_VIEWPORT_GTK4_H
#include <gtk/gtk.h>
#include "umicom/workbench_layout/viewport.h"
G_BEGIN_DECLS
/* Main GTK thread only. The factory is called synchronously for each component
 * leaf, including inactive/hidden leaves, and transfers one newly created,
 * unparented widget (floating or one owned strong reference). Context is borrowed
 * only during construction. The node pointer is borrowed until the factory
 * returns; copy any identifiers needed by later callbacks. A failure releases
 * all already-created widgets.
 * The source document is copied. No database or application command is invoked.
 */
typedef GtkWidget *(*UmiWorkbenchViewportFactory)(const UmiWorkbenchLayoutNode *node,void *context);
UmiStatus UmiWorkbenchViewportWidgetCreate(const UmiWorkbenchLayoutDocument *document,
    UmiWorkbenchViewportFactory factory,void *context,GtkWidget **outWidget,
    UmiWorkbenchViewportDiagnostic *diagnostic);
/* Returned widget is floating. Parent it or sink and release it. Use a surrounding
 * GtkScrolledWindow so layouts remain reachable when their minimum exceeds the
 * window size. Leaf factories are wrapped in independent scrolled containers.
 * Changes below affect the owned PREVIEW only. They never alter the caller's
 * canonical document, persist a layout, recreate a leaf, or clear its text.
 * Successful changes emit "layout-changed" with no additional arguments.
 */
UmiStatus UmiWorkbenchViewportWidgetSetSplit(GtkWidget *widget,const char *nodeId,double ratio);
UmiStatus UmiWorkbenchViewportWidgetSetActive(GtkWidget *widget,const char *nodeId,size_t position);
UmiStatus UmiWorkbenchViewportWidgetSetVisible(GtkWidget *widget,const char *nodeId,bool visible);
UmiStatus UmiWorkbenchViewportWidgetPlan(GtkWidget *widget,UmiWorkbenchViewportPlan *outPlan);
/* Built-in gallery: memory-only Notes and fictional account browsing controls.
 * The application owns the returned top-level window. No disk writes or business
 * operations. It is an example consumer, not a replacement Notes or Bank app. */
GtkWindow *UmiWorkbenchViewportGallery(GtkApplication *application);
G_END_DECLS
#endif
