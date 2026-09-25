/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/broker_connectivity/provider_registry.h
 *
 * PURPOSE:
 *   Maintain a bounded provider-neutral broker adapter catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_BROKER_CONNECTIVITY_PROVIDER_REGISTRY_H
#define UMICOM_BROKER_CONNECTIVITY_PROVIDER_REGISTRY_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_BROKER_PROVIDER_CAPACITY 16U

typedef struct UmiBrokerProviderDescriptor {
    char id[64];
    char displayName[96];
    int paperSupported;
    int liveSupported;
    int accountSupported;
    int orderSupported;
    int executionSupported;
    uint64_t capabilities;
} UmiBrokerProviderDescriptor;

typedef struct UmiBrokerProviderRegistry {
    UmiBrokerProviderDescriptor providers[UMI_BROKER_PROVIDER_CAPACITY];
    size_t count;
    uint64_t revision;
} UmiBrokerProviderRegistry;

void umi_broker_provider_registry_init(UmiBrokerProviderRegistry *registry);
UmiStatus umi_broker_provider_registry_register(
    UmiBrokerProviderRegistry *registry,
    const UmiBrokerProviderDescriptor *descriptor);
const UmiBrokerProviderDescriptor *umi_broker_provider_registry_find(
    const UmiBrokerProviderRegistry *registry,
    const char *id);

#ifdef __cplusplus
}
#endif
#endif
