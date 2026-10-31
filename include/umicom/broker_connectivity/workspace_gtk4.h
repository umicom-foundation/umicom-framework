/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/workspace_gtk4.h
 *
 * PURPOSE:
 *   Own one broker inspector and display its account observations in a reusable workspace.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BROKER_WORKSPACE_GTK4_H
#define UMICOM_BROKER_WORKSPACE_GTK4_H
#include "umicom/broker_connectivity/access_plan.h"
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiBrokerWorkspaceGtk UmiBrokerWorkspaceGtk;
/* GTK-main-thread only. Paper or Live plans are copied; Guest is rejected.
 * Creation neither presents a window nor opens a socket. The controller owns
 * its root; callers may parent the borrowed Widget but must Destroy once.
 * Parent removal retires controls and connections even if widgets are retained. */
UmiStatus UmiBrokerWorkspaceGtkCreate(GtkWindow *parent,
    const UmiBrokerAccessPlan *plan, UmiBrokerWorkspaceGtk **outWorkspace);
GtkWidget *UmiBrokerWorkspaceGtkWidget(UmiBrokerWorkspaceGtk *workspace);
/* Present the one inspector for this workspace, reusing it until closed.
 * Authentication and 2FA occur in local TWS/IB Gateway; Connect stays explicit.
 * A reopened inspector starts disconnected with fresh Live acknowledgement.
 * Reentrant calls during creation or presentation return BUSY. If the host
 * retires the workspace or parent during a callback, opening is cancelled. */
UmiStatus UmiBrokerWorkspaceGtkOpenConnection(UmiBrokerWorkspaceGtk *workspace);
/* Pull the inspector's latest copied observation immediately. Also called by
 * a bounded UI timer; this never requests accounts or pumps broker transport.
 * Account and position tables retain their native rows across unrelated frames,
 * status and clock updates. Each report replaces only its changed table; a new
 * account or report generation replaces both. Retained stale rows remain marked
 * by the workspace status and never imply current broker authority. */
UmiStatus UmiBrokerWorkspaceGtkRefresh(UmiBrokerWorkspaceGtk *workspace);

/* Page identity is independent of tab order and pane placement. */
typedef enum UmiBrokerWorkspacePage {
    UMI_BROKER_PAGE_ACCOUNT,
    UMI_BROKER_PAGE_MARKET,
    UMI_BROKER_PAGE_INSTRUMENT,
    UMI_BROKER_PAGE_OPTIONS,
    UMI_BROKER_PAGE_OPEN_ORDERS,
    UMI_BROKER_PAGE_COMPLETED_ORDERS,
    UMI_BROKER_PAGE_TRADES,
    UMI_BROKER_PAGE_CHART,
    UMI_BROKER_PAGE_DEPTH,
    UMI_BROKER_PAGE_TIME_SALES,
    UMI_BROKER_PAGE_PNL,
    UMI_BROKER_PAGE_SCANNER,
    UMI_BROKER_PAGE_COUNT
} UmiBrokerWorkspacePage;
typedef struct UmiBrokerWorkspaceLayout {
    unsigned pane[UMI_BROKER_PAGE_COUNT];
    /* A page-ID permutation; relative order within each pane is retained.
     * Copy returns a canonical sequence: first pane followed by second pane. */
    UmiBrokerWorkspacePage order[UMI_BROKER_PAGE_COUNT];
    /* PAGE_COUNT means an empty pane; otherwise this page must belong to it. */
    UmiBrokerWorkspacePage selected[2];
    GtkOrientation orientation;
    int divider;
} UmiBrokerWorkspaceLayout;
const char *UmiBrokerWorkspacePageName(UmiBrokerWorkspacePage page);
/* Native layout operations never connect, subscribe or request broker data.
 * They retain existing controls and observations. All calls use the GTK thread.
 * Mutation during another layout command returns BUSY; retirement cancels it.
 * Copy preserves its output on failure. Apply validates the entire layout first.
 * A host may persist the plain layout; this controller performs no file I/O. */
UmiStatus UmiBrokerWorkspaceGtkSelectPage(UmiBrokerWorkspaceGtk *workspace,
    UmiBrokerWorkspacePage page);
UmiStatus UmiBrokerWorkspaceGtkMovePage(UmiBrokerWorkspaceGtk *workspace,
    UmiBrokerWorkspacePage page, unsigned pane);
UmiStatus UmiBrokerWorkspaceGtkSetSplit(UmiBrokerWorkspaceGtk *workspace,
    GtkOrientation orientation);
UmiStatus UmiBrokerWorkspaceGtkCopyLayout(UmiBrokerWorkspaceGtk *workspace,
    UmiBrokerWorkspaceLayout *outLayout);
UmiStatus UmiBrokerWorkspaceGtkApplyLayout(UmiBrokerWorkspaceGtk *workspace,
    const UmiBrokerWorkspaceLayout *layout);

void UmiBrokerWorkspaceGtkDestroy(UmiBrokerWorkspaceGtk *workspace);
#ifdef __cplusplus
}
#endif
#endif
