/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/desktop/context_synchronizer.h
 *
 * PURPOSE:
 *   Deliver linked-window context between hosted components through Framework.
 *   Applications register declarative interests and never address, call or
 *   retain one another directly.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
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

/**
 * Represent the desktop context subscription data shared with callers of this public
 * contract.
 */
typedef struct UmiDesktopContextSubscription {
    uint32_t structure_size;
    char window_id[UMI_DESKTOP_ID_CAPACITY];
    char context_type[UMI_CONTEXT_HUB_TEXT_CAPACITY];
    int refresh_on_delivery;
} UmiDesktopContextSubscription;

/**
 * Represent the desktop context delivery data shared with callers of this public contract.
 */
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

/**
 * Represent the desktop context synchronizer snapshot data shared with callers of this
 * public contract.
 */
typedef struct UmiDesktopContextSynchronizerSnapshot {
    uint32_t structure_size;
    size_t subscription_count;
    size_t delivery_count;
    size_t pending_count;
    uint64_t last_sequence;
    uint64_t revision;
} UmiDesktopContextSynchronizerSnapshot;

/**
 * Represent the desktop context synchronizer data shared with callers of this public
 * contract.
 */
typedef struct UmiDesktopContextSynchronizer UmiDesktopContextSynchronizer;

/**
 * Initialise desktop context synchronizer from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_desktop_context_synchronizer_create(
    UmiDesktopRuntime *desktop,
    UmiDesktopContentRuntime *content,
    UmiDesktopContextSynchronizer **out_synchronizer);
/**
 * Release or reset state held by desktop context synchronizer so the same storage can be
 * reused safely.
 */
void umi_desktop_context_synchronizer_destroy(
    UmiDesktopContextSynchronizer *synchronizer);
/**
 * Provide the desktop context synchronizer subscribe operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_context_synchronizer_subscribe(
    UmiDesktopContextSynchronizer *synchronizer,
    const UmiDesktopContextSubscription *subscription);
/**
 * Provide the desktop context synchronizer unsubscribe operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_context_synchronizer_unsubscribe(
    UmiDesktopContextSynchronizer *synchronizer,
    const char *window_id,
    const char *context_type);
/**
 * Provide the desktop context synchronizer publish operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_context_synchronizer_publish(
    UmiDesktopContextSynchronizer *synchronizer,
    const char *source_window_id,
    const char *context_type,
    const char *subject_id,
    const char *correlation_id);
/**
 * Provide the desktop context synchronizer next operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_context_synchronizer_next(
    const UmiDesktopContextSynchronizer *synchronizer,
    const char *destination_window_id,
    UmiDesktopContextDelivery *out_delivery);
/**
 * Provide the desktop context synchronizer acknowledge operation used by this module and
 * its client applications.
 */
UmiStatus umi_desktop_context_synchronizer_acknowledge(
    UmiDesktopContextSynchronizer *synchronizer,
    const char *destination_window_id,
    uint64_t sequence);
/**
 * Find desktop context synchronizer delivery while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_desktop_context_synchronizer_delivery_at(
    const UmiDesktopContextSynchronizer *synchronizer,
    size_t index,
    UmiDesktopContextDelivery *out_delivery);
/**
 * Provide the desktop context synchronizer snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_desktop_context_synchronizer_snapshot(
    const UmiDesktopContextSynchronizer *synchronizer,
    UmiDesktopContextSynchronizerSnapshot *out_snapshot);
/**
 * Return the number of records represented by desktop context synchronizer subscription
 * without changing their state.
 */
size_t umi_desktop_context_synchronizer_subscription_count(
    const UmiDesktopContextSynchronizer *synchronizer);

#ifdef __cplusplus
}
#endif

#endif
