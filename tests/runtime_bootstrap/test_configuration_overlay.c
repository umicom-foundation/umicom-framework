/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_configuration_overlay.c
 *
 * PURPOSE:
 *   Focused regression coverage for Merge configuration overlays while preserving highest-precedence values.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/configuration_property.h"
#include "umicom/runtime/bootstrap/configuration_overlay.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapPropertySet set; UmiBootstrapConfigurationProperty low,high;
    umi_bootstrap_property_set_init(&set);
    CHECK(umi_bootstrap_configuration_property_init(&low,"app.mode","dev",1,false)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_configuration_property_init(&high,"app.mode","prod",10,false)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_configuration_overlay_apply(&set,&low)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_configuration_overlay_apply(&set,&high)==UMI_STATUS_OK);
    CHECK(strcmp(umi_bootstrap_property_set_find(&set,"app.mode")->value,"prod")==0);
    return 0;
}
