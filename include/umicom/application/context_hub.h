/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/context_hub.h
 *
 * PURPOSE:
 *   Share linked-window context through Framework. Charts, editors, AI panes,
 *   TMS tickets and media timelines observe a named link group without holding
 *   pointers to one another.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_CONTEXT_HUB_H
#define UMICOM_APPLICATION_CONTEXT_HUB_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_CONTEXT_HUB_MAX_GROUPS 32U
#define UMI_CONTEXT_HUB_MAX_SUBSCRIBERS 128U
#define UMI_CONTEXT_HUB_TEXT_CAPACITY 128U

/**
 * Represent the application context data shared with callers of this public contract.
 */
typedef struct UmiApplicationContext {
    char group_id[UMI_CONTEXT_HUB_TEXT_CAPACITY];
    char source_application_id[UMI_CONTEXT_HUB_TEXT_CAPACITY];
    char context_type[UMI_CONTEXT_HUB_TEXT_CAPACITY];
    char subject_id[UMI_CONTEXT_HUB_TEXT_CAPACITY];
    char correlation_id[UMI_CONTEXT_HUB_TEXT_CAPACITY];
    uint64_t revision;
} UmiApplicationContext;

typedef void (*UmiApplicationContextListener)(
    void *listener_context,
    const UmiApplicationContext *context);

/**
 * Represent the application context hub data shared with callers of this public contract.
 */
typedef struct UmiApplicationContextHub UmiApplicationContextHub;

/**
 * Initialise application context hub from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_application_context_hub_create(
    UmiApplicationContextHub **out_hub);
/**
 * Release or reset state held by application context hub so the same storage can be reused
 * safely.
 */
void umi_application_context_hub_destroy(UmiApplicationContextHub *hub);
/**
 * Provide the application context hub subscribe operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_context_hub_subscribe(
    UmiApplicationContextHub *hub,
    const char *application_id,
    const char *group_id,
    UmiApplicationContextListener listener,
    void *listener_context);
/**
 * Provide the application context hub publish operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_context_hub_publish(
    UmiApplicationContextHub *hub,
    const UmiApplicationContext *context);
/**
 * Provide the application context hub snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_context_hub_snapshot(
    const UmiApplicationContextHub *hub,
    const char *group_id,
    UmiApplicationContext *out_context);
/**
 * Return the number of records represented by application context hub group without
 * changing their state.
 */
size_t umi_application_context_hub_group_count(
    const UmiApplicationContextHub *hub);

#ifdef __cplusplus
}
#endif

#endif
