/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_resolver.c
 *
 * PURPOSE:
 *   Exercise one Suite and Inter-Application Runtime Foundation behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/resolver.h"

#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s\n", #expr); return 1; } } while (0)

int main(void) {
    UmiIntegrationRegistry r; UmiIntegrationApplication a;
    UmiIntegrationDependency d[2]; UmiIntegrationResolution x;
    umi_integration_registry_init(&r); umi_integration_application_init(&a);
    CHECK(umi_integration_application_set_identity(&a,"required.app","Required")==UMI_STATUS_OK);
    a.enabled=true;
    CHECK(umi_integration_registry_register(&r,&a)==UMI_STATUS_OK);
    d[0]=(UmiIntegrationDependency){UMI_INTEGRATION_TARGET_APPLICATION,UMI_INTEGRATION_DEPENDENCY_REQUIRED,"required.app"};
    d[1]=(UmiIntegrationDependency){UMI_INTEGRATION_TARGET_APPLICATION,UMI_INTEGRATION_DEPENDENCY_OPTIONAL,"optional.app"};
    CHECK(umi_integration_resolve(&r,d,2U,&x)==UMI_STATUS_OK);
    CHECK(x.missing_required==0U && x.missing_optional==1U);
    return 0;
}
