/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/compatibility.h
 *
 * PURPOSE:
 *   Evaluate Framework ABI and application compatibility before an update is activated.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Compatibility checks stop a newer package from being installed when it requires an ABI the current host cannot provide.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_COMPATIBILITY_H
#define INCLUDE_UMICOM_DELIVERY_COMPATIBILITY_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiCompatibility {
    uint32_t required_framework_abi;
    uint32_t available_framework_abi;
    uint32_t minimum_schema;
    uint32_t available_schema;
} UmiCompatibility;
int umi_compatibility_passed(const UmiCompatibility *compatibility);

#ifdef __cplusplus
}
#endif

#endif
