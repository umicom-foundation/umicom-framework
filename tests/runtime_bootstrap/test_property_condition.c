/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/runtime_bootstrap/test_property_condition.c
 *
 * PURPOSE:
 *   Focused regression coverage for Evaluate configuration-property equality and presence.
 *---------------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "umicom/runtime/bootstrap/configuration_property.h"
#include "umicom/runtime/bootstrap/configuration_overlay.h"
#include "umicom/runtime/bootstrap/property_condition.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {

    UmiBootstrapPropertySet s; UmiBootstrapConfigurationProperty p;
    umi_bootstrap_property_set_init(&s);
    CHECK(umi_bootstrap_configuration_property_init(&p,"app.mode","prod",1,false)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_configuration_overlay_apply(&s,&p)==UMI_STATUS_OK);
    CHECK(umi_bootstrap_property_condition_match(&s,"app.mode","prod"));
    return 0;
}
