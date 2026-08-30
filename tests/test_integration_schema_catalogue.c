/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_schema_catalogue.c
 *
 * PURPOSE:
 *   Implement the test integration schema catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework Tests | Schema catalogue | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "umicom/integration/schema_catalogue.h"
int main(void)
{
    UmiIntegrationSchemaCatalogue catalogue = {0};
    UmiIntegrationSchemaDescriptor provider = {0};
    UmiIntegrationSchemaDescriptor consumer = {0};
    (void)umi_integration_designer_copy(provider.id,sizeof(provider.id),"order-v1"); provider.kind = UMI_INTEGRATION_DESIGNER_JSON_SCHEMA; provider.major = 1U; provider.minor = 2U;
    consumer = provider; consumer.minor = 0U;
    assert(umi_integration_schema_catalogue_register(&catalogue,&provider) == UMI_STATUS_OK);
    assert(umi_integration_schema_compatible(&provider,&consumer));
    consumer.major = 2U;
    assert(!umi_integration_schema_compatible(&provider,&consumer));
    assert(umi_integration_schema_catalogue_find(&catalogue,"order-v1") != NULL);
    return 0;
}
