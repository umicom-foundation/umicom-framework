/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/frontend_conformance/test_surface_catalogue.c
 *
 * PURPOSE:
 *   Focused regression coverage for bounded catalogue of semantic surface requirements shared by all frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/surface_catalogue.h"
#include "umicom/frontend/conformance/surface_requirement.h"

#define CHECK(expr) do { if (!(expr)) return __LINE__; } while (0)

int main(void) {
    UmiFcSurfaceCatalogue c; UmiFcSurfaceRequirement r,o; umi_fc_surface_catalogue_init(&c); CHECK(umi_fc_surface_requirement_make("chart",1U,2U,true,&r)==UMI_STATUS_OK); CHECK(umi_fc_surface_catalogue_add(&c,&r)==UMI_STATUS_OK); CHECK(umi_fc_surface_catalogue_find(&c,"chart",&o)==UMI_STATUS_OK); CHECK(o.required==1U);
    return 0;
}
