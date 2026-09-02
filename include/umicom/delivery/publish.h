/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/publish.h
 *
 * PURPOSE:
 *   Define publisher contracts that can upload or register verified release artifacts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * Publishing is separated from packaging so GitHub Releases, an internal server or another registry can implement the same boundary.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_PUBLISH_H
#define INCLUDE_UMICOM_DELIVERY_PUBLISH_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"
#include "umicom/delivery/release.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the publish result data shared with callers of this public contract.
 */
typedef struct UmiPublishResult {
    char location[UMI_DELIVERY_PATH_CAPACITY];
    int succeeded;
} UmiPublishResult;

typedef UmiStatus (*UmiPublishFn)(void *instance,
                                  const UmiRelease *release,
                                  UmiPublishResult *result);

/**
 * Represent the publisher data shared with callers of this public contract.
 */
typedef struct UmiPublisher {
    char publisher_id[UMI_DELIVERY_ID_CAPACITY];
    void *instance;
    UmiPublishFn publish;
} UmiPublisher;

/**
 * Initialise publisher from caller-provided values so later operations receive a known
 * state.
 */
UmiStatus umi_publisher_init(UmiPublisher *publisher,
                             const char *publisher_id,
                             void *instance,
                             UmiPublishFn publish);
/**
 * Provide the publisher publish operation used by this module and its client applications.
 */
UmiStatus umi_publisher_publish(const UmiPublisher *publisher,
                                const UmiRelease *release,
                                UmiPublishResult *result);

#ifdef __cplusplus
}
#endif

#endif
