/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/delivery/package_provider.h
 *
 * PURPOSE:
 *   Define a provider interface for packaging technologies and invoke providers through a stable C contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
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

/**
 * Represent the package provider data shared with callers of this public contract.
 */
typedef struct UmiPackageProvider {
    char provider_id[UMI_DELIVERY_ID_CAPACITY];
    UmiPackageFormat format;
    void *instance;
    UmiPackageCreateFn create;
} UmiPackageProvider;

/**
 * Initialise package provider from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_package_provider_init(UmiPackageProvider *provider,
                                    const char *provider_id,
                                    UmiPackageFormat format,
                                    void *instance,
                                    UmiPackageCreateFn create);
/**
 * Initialise package provider from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_package_provider_create(const UmiPackageProvider *provider,
                                      const UmiPackageSpec *spec,
                                      UmiPackageResult *result);

#ifdef __cplusplus
}
#endif

#endif
