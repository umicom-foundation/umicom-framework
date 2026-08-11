/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/package_provider.h
 *
 * PURPOSE:
 *   Define a provider interface for packaging technologies and invoke providers through a stable C contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Providers keep external packaging tools behind one interface so product code never shells out directly.
 */

#ifndef INCLUDE_UMICOM_DELIVERY_PACKAGE_PROVIDER_H
#define INCLUDE_UMICOM_DELIVERY_PACKAGE_PROVIDER_H

#include "umicom/base/status.h"
#include "umicom/delivery/types.h"
#include "umicom/delivery/package.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiPackageCreateFn)(void *instance,
                                        const UmiPackageSpec *spec,
                                        UmiPackageResult *result);

typedef struct UmiPackageProvider {
    char provider_id[UMI_DELIVERY_ID_CAPACITY];
    UmiPackageFormat format;
    void *instance;
    UmiPackageCreateFn create;
} UmiPackageProvider;

UmiStatus umi_package_provider_init(UmiPackageProvider *provider,
                                    const char *provider_id,
                                    UmiPackageFormat format,
                                    void *instance,
                                    UmiPackageCreateFn create);
UmiStatus umi_package_provider_create(const UmiPackageProvider *provider,
                                      const UmiPackageSpec *spec,
                                      UmiPackageResult *result);

#ifdef __cplusplus
}
#endif

#endif
