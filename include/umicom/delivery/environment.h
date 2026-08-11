/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/environment.h
 *
 * PURPOSE:
 *   Describe the target operating system, architecture and build configuration of a release.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Release compatibility depends on where a package is intended to run, so this metadata is explicit rather than inferred from filenames.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_ENVIRONMENT_H
#define INCLUDE_UMICOM_DELIVERY_ENVIRONMENT_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDeliveryEnvironment {
    char operating_system[UMI_DELIVERY_ID_CAPACITY];
    char architecture[UMI_DELIVERY_ID_CAPACITY];
    char configuration[UMI_DELIVERY_ID_CAPACITY];
} UmiDeliveryEnvironment;
UmiStatus umi_delivery_environment_init(UmiDeliveryEnvironment *environment,
                                        const char *operating_system,
                                        const char *architecture,
                                        const char *configuration);

#ifdef __cplusplus
}
#endif

#endif
