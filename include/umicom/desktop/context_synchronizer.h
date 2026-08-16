/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/context_synchronizer.h
 *
 * PURPOSE:
 *   Deliver linked-window context between hosted components through Framework.
 *   Applications register declarative interests and never address, call or
 *   retain one another directly.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESKTOP_CONTEXT_SYNCHRONIZER_H
#define UMICOM_DESKTOP_CONTEXT_SYNCHRONIZER_H

#include <stddef.h>
#include <stdint.h>

#include "umicom/desktop/content_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESKTOP_CONTEXT_SUBSCRIPTION_MAX 128U
#define UMI_DESKTOP_CONTEXT_DELIVERY_MAX 256U

typedef struct UmiDesktopContextSubscription {
    uint32_t structure_size;
    char window_id[UMI_DESKTOP_ID_CAPACITY];
    char context_type[UMI_CONTEXT_HUB_TEXT_CAPACITY];
    int refresh_on_delivery;
} UmiDesktopContextSubscription;

typedef struct UmiDesktopContextDelivery {
    uint32_t structure_size;
    uint64_t sequence;
    char group_id[UMI_CONTEXT_HUB_TEXT_CAPACITY];
    char source_window_id[UMI_DESKTOP_ID_CAPACITY];
    char source_application_id[UMI_CONTEXT_HUB_TEXT_CAPACITY];
    char destination_window_id[UMI_DESKTOP_ID_CAPACITY];
    char destination_application_id[UMI_CONTEXT_HUB_TEXT_CAPACITY];
    char context_type[UMI_CONTEXT_HUB_TEXT_CAPACITY];
    char subject_id[UMI_CONTEXT_HUB_TEXT_CAPACITY];
    char correlation_id[UMI_CONTEXT_HUB_TEXT_CAPACITY];
    UmiStatus refresh_status;
    int acknowledged;
    uint64_t revision;
} UmiDesktopContextDelivery;

typedef struct UmiDesktopContextSynchronizerSnapshot {
    uint32_t structure_size;
    size_t subscription_count;
    size_t delivery_count;
    size_t pending_count;
    uint64_t last_sequence;
    uint64_t revision;
} UmiDesktopContextSynchronizerSnapshot;

typedef struct UmiDesktopContextSynchronizer UmiDesktopContextSynchronizer;

UmiStatus umi_desktop_context_synchronizer_create(
    UmiDesktopRuntime *desktop,
    UmiDesktopContentRuntime *content,
    UmiDesktopContextSynchronizer **out_synchronizer);
void umi_desktop_context_synchronizer_destroy(
    UmiDesktopContextSynchronizer *synchronizer);
UmiStatus umi_desktop_context_synchronizer_subscribe(
    UmiDesktopContextSynchronizer *synchronizer,
    const UmiDesktopContextSubscription *subscription);
UmiStatus umi_desktop_context_synchronizer_unsubscribe(
    UmiDesktopContextSynchronizer *synchronizer,
    const char *window_id,
    const char *context_type);
UmiStatus umi_desktop_context_synchronizer_publish(
    UmiDesktopContextSynchronizer *synchronizer,
    const char *source_window_id,
    const char *context_type,
    const char *subject_id,
    const char *correlation_id);
UmiStatus umi_desktop_context_synchronizer_next(
    const UmiDesktopContextSynchronizer *synchronizer,
    const char *destination_window_id,
    UmiDesktopContextDelivery *out_delivery);
UmiStatus umi_desktop_context_synchronizer_acknowledge(
    UmiDesktopContextSynchronizer *synchronizer,
    const char *destination_window_id,
    uint64_t sequence);
UmiStatus umi_desktop_context_synchronizer_delivery_at(
    const UmiDesktopContextSynchronizer *synchronizer,
    size_t index,
    UmiDesktopContextDelivery *out_delivery);
UmiStatus umi_desktop_context_synchronizer_snapshot(
    const UmiDesktopContextSynchronizer *synchronizer,
    UmiDesktopContextSynchronizerSnapshot *out_snapshot);
size_t umi_desktop_context_synchronizer_subscription_count(
    const UmiDesktopContextSynchronizer *synchronizer);

#ifdef __cplusplus
}
#endif

#endif
