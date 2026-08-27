/* Umicom Framework application production test | control_plane | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include "test_fixture.h"
int main(void) {
    UmiApplicationProductionControlPlane *plane = calloc(1U, sizeof(*plane));
    assert(plane != NULL);
    assert(umi_application_production_control_plane_init(umi_test_application_production_all_available, NULL, plane) == UMI_STATUS_OK);
    assert(plane->registry.count == plane->portfolio.count);
    assert(umi_application_production_control_plane_runtime(plane, "org.umicom.studio") != NULL);
    free(plane);
    return 0;
}

