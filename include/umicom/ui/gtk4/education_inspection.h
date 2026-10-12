/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/education_inspection.h
 * PURPOSE: Offer read-only teaching trace inspection in native learning surfaces.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_GTK4_EDUCATION_INSPECTION_H
#define UMICOM_UI_GTK4_EDUCATION_INSPECTION_H
#include <gtk/gtk.h>
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Create a floating widget with an explicit file path, teaching-format choice
 * and read-only report. Inspection uses a bounded native worker; it never runs
 * the file or controls Kernel resources. The GTK thread owns all UI methods.
 */
GtkWidget *UmiEducationInspectionGtkCreate(void);
/**
 * Retire a panel before its host closes, including externally retained widgets.
 * Already accepted reads may finish; their results will be discarded. Calling
 * this more than once is harmless. Normal widget destruction also disconnects
 * callbacks and makes pending completions inert.
 */
void UmiEducationInspectionGtkRetire(GtkWidget *widget);
/** Return whether an accepted read still awaits completion, even after Retire. */
bool UmiEducationInspectionGtkBusy(GtkWidget *widget);
#ifdef __cplusplus
}
#endif
#endif
