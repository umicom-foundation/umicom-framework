/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/testing/ctest_output_gtk4.h
 * PURPOSE: Present live test invocation evidence without owning test execution.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TESTING_CTEST_OUTPUT_GTK4_H
#define UMICOM_TESTING_CTEST_OUTPUT_GTK4_H
#include <gtk/gtk.h>
#include "umicom/testing/ctest_output.h"
#ifdef __cplusplus
extern "C"
{
#endif
    /* GTK-owner-thread API; the floating widget owns display copies only. Controls
 * never launch, cancel or rerun tests. It uses the shared output view so pausing,
 * copying and character repair behave the same as build output. */
    GtkWidget *UmiCtestOutputGtk4Create(void);
    UmiStatus UmiCtestOutputGtk4Update(GtkWidget *panel, const UmiCtestOutputSnapshot *snapshot);
#ifdef __cplusplus
}
#endif
#endif
