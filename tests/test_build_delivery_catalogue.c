/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_build_delivery_catalogue.c
 *
 * PURPOSE:
 *   Verify canonical build, task, artifact and deployment contributions are
 *   stable, unique and discoverable by every frontend.
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

#include "umicom/build/delivery_catalogue.h"

int main(void)
{
    size_t index;
    size_t other;
    assert(umi_build_delivery_command_count() == 36U);
    assert(umi_build_delivery_view_count() == 8U);
    for (index = 0U; index < umi_build_delivery_command_count(); ++index) {
        const UmiBuildDeliveryCommandDescriptor *command =
            umi_build_delivery_command_at(index);
        assert(command != NULL);
        assert(command->command_id[0] != '\0');
        assert(command->framework_contract[0] != '\0');
        for (other = index + 1U;
             other < umi_build_delivery_command_count(); ++other) {
            assert(strcmp(command->command_id,
                          umi_build_delivery_command_at(other)->command_id) != 0);
        }
    }
    for (index = 0U; index < umi_build_delivery_view_count(); ++index) {
        const UmiBuildDeliveryViewDescriptor *view =
            umi_build_delivery_view_at(index);
        assert(view != NULL);
        assert(view->view_id[0] != '\0');
        for (other = index + 1U;
             other < umi_build_delivery_view_count(); ++other) {
            assert(strcmp(view->view_id,
                          umi_build_delivery_view_at(other)->view_id) != 0);
        }
    }
    assert(umi_build_delivery_command_find("deployment.rollback") != NULL);
    assert(umi_build_delivery_command_find("pipeline.export-evidence") != NULL);
    assert(umi_build_delivery_view_find("build.delivery-pipeline") != NULL);
    assert(umi_build_delivery_command_at(
               umi_build_delivery_command_count()) == NULL);
    assert(umi_build_delivery_view_at(umi_build_delivery_view_count()) == NULL);
    return 0;
}
