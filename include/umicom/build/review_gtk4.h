/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/build/review_gtk4.h
 *
 * PURPOSE:
 *   Present an owned snapshot of build history without retaining its producer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_BUILD_REVIEW_GTK4_H
#define UMICOM_BUILD_REVIEW_GTK4_H
#include <gtk/gtk.h>
#include "umicom/build/review.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Main GTK thread only. Captures newest eight records synchronously, then
 * owns only the review snapshot. NULL parent is permitted. No action is run.
 * The returned window is GTK-owned; callers may take a reference for testing. */
GtkWindow *UmiGtk4BuildReviewPresent(GtkWindow *parent, const UmiBuildHistory *history);
#ifdef __cplusplus
}
#endif
#endif
