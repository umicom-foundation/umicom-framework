/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/federation.h
 *
 * PURPOSE:
 *   Route cross-application commands, queries and events by Framework
 *   capability. The public request deliberately has no target-application
 *   field, preventing callers from bypassing Framework mediation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_FEDERATION_H
#define UMICOM_APPLICATION_FEDERATION_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/application/portfolio.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_FEDERATION_ID_CAPACITY 128U

typedef enum UmiFederationMessageKind {
    UMI_FEDERATION_COMMAND = 1,
    UMI_FEDERATION_QUERY = 2,
    UMI_FEDERATION_EVENT = 3
} UmiFederationMessageKind;

typedef struct UmiFederationRequest {
    uint32_t structure_size;
    const char *message_id;
    const char *source_application_id;
    const char *capability_id;
    const char *operation_id;
    const char *schema_id;
    const char *correlation_id;
    UmiFederationMessageKind kind;
    const void *payload;
    size_t payload_size;
} UmiFederationRequest;

typedef struct UmiFederationResponse {
    void *payload;
    size_t capacity;
    size_t payload_size;
    char provider_application_id[UMI_FEDERATION_ID_CAPACITY];
} UmiFederationResponse;

typedef UmiStatus (*UmiFederationHandler)(
    void *context,
    const UmiFederationRequest *request,
    UmiFederationResponse *response);

typedef int (*UmiFederationPolicy)(
    void *context,
    const UmiFederationRequest *request,
    const char *provider_application_id);

typedef struct UmiFederationStats {
    size_t application_count;
    size_t provider_count;
    uint64_t dispatch_count;
    uint64_t denied_count;
    uint64_t failed_count;
} UmiFederationStats;

typedef struct UmiFederationRouter UmiFederationRouter;

UmiStatus umi_federation_router_create(UmiFederationRouter **out_router);
void umi_federation_router_destroy(UmiFederationRouter *router);
UmiStatus umi_federation_router_register_application(
    UmiFederationRouter *router,
    const UmiApplicationDefinition *definition);
const UmiApplicationDefinition *umi_federation_router_find_application(
    const UmiFederationRouter *router,
    const char *application_id);
UmiStatus umi_federation_router_register_provider(
    UmiFederationRouter *router,
    const char *application_id,
    const char *capability_id,
    UmiFederationHandler handler,
    void *handler_context);
void umi_federation_router_set_policy(
    UmiFederationRouter *router,
    UmiFederationPolicy policy,
    void *policy_context);
UmiStatus umi_federation_router_dispatch(
    UmiFederationRouter *router,
    const UmiFederationRequest *request,
    UmiFederationResponse *response);
UmiFederationStats umi_federation_router_stats(
    const UmiFederationRouter *router);

#ifdef __cplusplus
}
#endif

#endif
