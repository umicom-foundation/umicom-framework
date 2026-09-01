/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/publish.c
 *
 * PURPOSE:
 *   Implement publisher contracts that can upload or register verified release artifacts.
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

#include "umicom/delivery/publish.h"
#include "delivery_internal.h"
#include <string.h>

UmiStatus umi_publisher_init(UmiPublisher *publisher,
                             const char *publisher_id,
                             void *instance,
                             UmiPublishFn publish)
{
    if (publisher == NULL || publisher_id == NULL || publish == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(publisher, 0, sizeof(*publisher));
    if (umi_delivery_copy_text(publisher->publisher_id,
                               sizeof(publisher->publisher_id),
                               publisher_id) != UMI_STATUS_OK) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    publisher->instance = instance;
    publisher->publish = publish;
    return UMI_STATUS_OK;
}

UmiStatus umi_publisher_publish(const UmiPublisher *publisher,
                                const UmiRelease *release,
                                UmiPublishResult *result)
{
    if (publisher == NULL || publisher->publish == NULL ||
        release == NULL || result == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(result, 0, sizeof(*result));
    return publisher->publish(publisher->instance, release, result);
}
