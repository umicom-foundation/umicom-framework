/* Umicom Framework application production test | command_binding | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionBinding binding;
    UmiApplicationProductionCommandBindings commands;
    assert(umi_application_production_binding_init(&adoption, &binding) == UMI_STATUS_OK);
    assert(umi_application_production_command_bindings_build(&binding, &commands) == UMI_STATUS_OK);
    assert(commands.count > binding.experience->panel_count);
    assert(umi_application_production_command_bindings_find(&commands, UMI_APPLICATION_COMMAND_SELECT_LAYOUT, binding.experience->default_layout_id) != NULL);
    return 0;
}

