/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/sdk/consumer.h
 *
 * PURPOSE:
 *   Describe one external application that consumes the installed Framework SDK.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * SDK APIs verify that an application can consume an installed Umicom Framework package without depending on private source-tree details.
 */
#ifndef UMICOM_SDK_CONSUMER_H
#define UMICOM_SDK_CONSUMER_H
#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/sdk/compatibility.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiSdkConsumer { const char *application_id; UmiSdkRequirement requirement; const char *const *required_targets; size_t target_count; } UmiSdkConsumer;
UmiStatus umi_sdk_consumer_validate(const UmiSdkConsumer *consumer);
#ifdef __cplusplus
}
#endif
#endif
