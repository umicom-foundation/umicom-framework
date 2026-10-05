/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/provider_connection_checks/internal.h
 * PURPOSE: Keep transport injection private and limit model-check response storage.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PROVIDER_CONNECTION_CHECKS_INTERNAL_H
#define UMICOM_PROVIDER_CONNECTION_CHECKS_INTERNAL_H
#include "umicom/provider_connections/check.h"
#include "umicom/security/profile_secrets.h"
#define UMI_CONNECTION_CHECK_BODY_CAPACITY 65536U
typedef UmiStatus (*UmiConnectionCheckSecretsFactory)(const char *, const char *, UmiProfileSecrets **);
typedef UmiStatus (*UmiConnectionCheckTransport)(const UmiProviderConnectionCheckPlan *,
    const char *, const UmiCancellationToken *, UmiProviderConnectionCheckResult *);
/* Tests inject isolated adapters here, not through a user-visible endpoint or
 * plugin override. The public operation always selects the native adapters. */
UmiStatus UmiConnectionCheckRunWith(const UmiProviderConnectionCheckPlan *plan,
    UmiProviderConnections *store, bool approved, const char *password,
    const UmiCancellationToken *cancellation, UmiProviderConnectionCheckResult *out,
    UmiConnectionCheckSecretsFactory factory, UmiConnectionCheckTransport transport);
UmiStatus UmiConnectionCheckHttp(const UmiProviderConnectionCheckPlan *plan,
    const char *key, const UmiCancellationToken *cancellation, UmiProviderConnectionCheckResult *out);
UmiStatus UmiConnectionCheckDecode(const char *body, size_t length,
    const char *model, UmiProviderConnectionCheckResult *out);
bool UmiConnectionCheckKeyValid(const char *key);
/* Reuse the same review/credential boundary for catalogue and chat adapters.
 * Callbacks are private trusted implementation hooks, never user plugins. */
typedef UmiStatus (*UmiConnectionAuthorizedDispatch)(const UmiProviderConnectionCheckPlan *,
    const char *, const UmiCancellationToken *, void *);
UmiStatus UmiConnectionAuthorize(const UmiProviderConnectionCheckPlan *plan,
    UmiProviderConnections *store, bool approved, const char *password,
    const UmiCancellationToken *cancellation, UmiConnectionCheckSecretsFactory factory,
    UmiConnectionAuthorizedDispatch dispatch, void *context);
/* A null request body selects the catalogue GET. A non-null body selects a
 * chat POST at the already validated saved endpoint. No arbitrary URL enters. */
UmiStatus UmiConnectionHttpExchange(const UmiProviderConnectionCheckPlan *plan,
    const char *request_body, const char *key, const UmiCancellationToken *cancellation,
    char *out_body, size_t capacity, unsigned *out_http_status);
#endif
