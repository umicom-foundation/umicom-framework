/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/broker_connectivity/test_provider_registry.c
 *
 * PURPOSE:
 *   Verify provider registration, duplicate rejection and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/broker_connectivity/provider_registry.h"

int main(void)
{
    UmiBrokerProviderRegistry registry;
    UmiBrokerProviderDescriptor ibkr = {0};

    umi_broker_provider_registry_init(&registry);
    (void)strcpy(ibkr.id, "ibkr");
    (void)strcpy(ibkr.displayName, "Interactive Brokers");
    ibkr.paperSupported = 1;
    ibkr.liveSupported = 1;
    ibkr.accountSupported = 1;
    ibkr.orderSupported = 1;
    ibkr.executionSupported = 1;

    assert(umi_broker_provider_registry_register(&registry, &ibkr) == UMI_STATUS_OK);
    assert(registry.count == 1U);
    assert(umi_broker_provider_registry_find(&registry, "ibkr") != NULL);
    assert(umi_broker_provider_registry_register(&registry, &ibkr) ==
           UMI_STATUS_ALREADY_EXISTS);
    return 0;
}
