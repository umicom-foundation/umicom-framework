/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/reactive_edit.h
 *
 * PURPOSE:
 *   Connect one authorized browser input to an exact native reactive revision.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_REACTIVE_EDIT_H
#define UMICOM_WEB_REACTIVE_EDIT_H

#include "umicom/ui/reactive/observable_store.h"
#include "umicom/web/service.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_WEB_REACTIVE_EDIT_CONTENT_TYPE "application/x-umicom-reactive-edit"

typedef struct UmiWebReactiveIntegerEdit {
    const char *path;
    const char *key;
    UmiUiReactiveObservableStore *store;
    const UmiWebOriginPolicy *origins;
    UmiWebRequestGate authorize;
    void *authorizeContext;
    int64_t minimum;
    int64_t maximum;
} UmiWebReactiveIntegerEdit;

/* A checkbox edits an existing Boolean key selected by the trusted host. */
typedef struct UmiWebReactiveBooleanEdit {
    const char *path;
    const char *key;
    UmiUiReactiveObservableStore *store;
    const UmiWebOriginPolicy *origins;
    UmiWebRequestGate authorize;
    void *authorizeContext;
} UmiWebReactiveBooleanEdit;

/* Handle one exact POST path. Supply an Origin admitted by the explicit policy,
 * the content type above and the canonical ASCII body revision=<u64>&value=<i64>.
 * The server's configuration owns the target key, value range and store.
 * A successful commit returns the new revision as JSON decimal text.
 * A stale or future base revision receives HTTP 409. */
/* The embedding host selects the already scoped store, serializes every store
 * read and write, and keeps borrowed inputs live and disjoint through dispatch.
 * Supply an authorization callback that checks the caller and edit permission.
 * The origin alone establishes neither. The callback may return a refusal in
 * its response. The handler owns caching headers. The callback must not change
 * the store. Operational failure preserves the
 * response and the store. Following a lost reply, read before trying again. */
UmiStatus UmiWebReactiveIntegerEditHandle(const UmiWebRequest *request,
    UmiWebResponse *response, void *context);

/* Use the same exact path, origin, authorization and revision contract. The
 * canonical body is revision=<u64>&value=true or revision=<u64>&value=false.
 * Numeric, quoted and differently cased representations are refused. */
UmiStatus UmiWebReactiveBooleanEditHandle(const UmiWebRequest *request,
    UmiWebResponse *response, void *context);

#ifdef __cplusplus
}
#endif

#endif
