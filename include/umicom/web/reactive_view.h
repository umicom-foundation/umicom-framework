/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/reactive_view.h
 *
 * PURPOSE:
 *   Serve selected, revisioned reactive values to an admitted browser view.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WEB_REACTIVE_VIEW_H
#define UMICOM_WEB_REACTIVE_VIEW_H

#include "umicom/ui/reactive/observable_store.h"
#include "umicom/web/service.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWebReactiveView {
    const char *path;
    const UmiUiReactiveObservableStore *store;
    const char *const *keys;
    size_t keyCount;
} UmiWebReactiveView;

/* Handle GET for an exact configured path. Omit the query or send since=0 for
 * the first view, then send since=<observed revision>. The query cannot change
 * the store or key selection. A future revision receives HTTP 409.
 *
 * The embedding host authorizes the request, selects the correct store and
 * keeps the same keys for the view lifetime. It serializes store reads and
 * writes and keeps this borrowed context alive through dispatch. Configure a
 * request gate or call this inside the application's admitted body handler.
 *
 * Success returns a detached JSON body with text representations of revision
 * and integer values, so JavaScript can read them without precision loss.
 * Respond with the external body as-is. Never inline it in a script element.
 * Pass an empty body slot. An operational failure preserves both outputs. */
UmiStatus UmiWebReactiveViewHandle(const UmiWebRequest *request, UmiWebResponse *response,
    UmiWebResponseBody **outBody, void *context);

#ifdef __cplusplus
}
#endif
#endif
