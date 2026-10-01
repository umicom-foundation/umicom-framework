/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/payment_quote.h
 * PURPOSE: Expose a reusable read-only fee scenario calculator.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_PAYMENT_QUOTE_H
#define UMICOM_UI_GTK4_PAYMENT_QUOTE_H
#include <gtk/gtk.h>
#ifdef __cplusplus
extern "C" {
#endif
/** Create a floating calculator. It owns only a copied quote and widgets; it
 * has no banking-service, database or execution-provider pointer. A host calls
 * Detach at closure so retained controls cannot act after their owner closes. */
GtkWidget *UmiGtk4PaymentQuoteCreate(void);
void UmiGtk4PaymentQuoteDetach(GtkWidget *calculator);
#ifdef __cplusplus
}
#endif
#endif
