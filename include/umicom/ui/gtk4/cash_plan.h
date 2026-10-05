/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/cash_plan.h
 * PURPOSE: Compose the shared local cash planning workflow in any native application.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_CASH_PLAN_H
#define UMICOM_UI_GTK4_CASH_PLAN_H
#include <gtk/gtk.h>
#include "umicom/base/status.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* GTK owner thread only. The floating widget owns an independent local plan.
 * No bank account is read and no payment/trade is submitted. File workers own
 * immutable copies; loading requires separate review and explicit apply.
 * The root owns signal lifetimes; retained controls become inert on teardown. */
    UmiStatus UmiGtk4CashPlanCreate(GtkWidget **out);
    /* Floating button opens an independent planner window, destroyed with its
 * parent when hosted in a window. An unparented launcher is inert. Each launch
 * starts with an empty local plan;
 * users explicitly save/open plans through absolute paths. */
    GtkWidget *UmiGtk4CashPlanLauncherCreate(void);
#ifdef __cplusplus
}
#endif
#endif
