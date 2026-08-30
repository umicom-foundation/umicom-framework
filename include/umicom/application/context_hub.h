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

typedef struct UmiApplicationContextHub UmiApplicationContextHub;

UmiStatus umi_application_context_hub_create(
    UmiApplicationContextHub **out_hub);
void umi_application_context_hub_destroy(UmiApplicationContextHub *hub);
UmiStatus umi_application_context_hub_subscribe(
    UmiApplicationContextHub *hub,
    const char *application_id,
    const char *group_id,
    UmiApplicationContextListener listener,
    void *listener_context);
UmiStatus umi_application_context_hub_publish(
    UmiApplicationContextHub *hub,
    const UmiApplicationContext *context);
UmiStatus umi_application_context_hub_snapshot(
    const UmiApplicationContextHub *hub,
    const char *group_id,
    UmiApplicationContext *out_context);
size_t umi_application_context_hub_group_count(
    const UmiApplicationContextHub *hub);

#ifdef __cplusplus
}
#endif

#endif
