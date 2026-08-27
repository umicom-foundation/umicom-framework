/* Umicom Framework application production test | acceptance_report | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include <stdlib.h>
#include "test_fixture.h"
int main(void) {
    UmiProductApplicationAdoption adoption = umi_test_application_production_studio_adoption();
    UmiApplicationProductionRuntime *runtime = calloc(1U, sizeof(*runtime));
    assert(runtime != NULL);
    assert(umi_application_production_runtime_init(&adoption, umi_test_application_production_all_available, NULL, runtime) == UMI_STATUS_OK);
    assert(runtime->acceptance.accepted);
    assert(runtime->acceptance.requirement_count == runtime->acceptance.accepted_count);
    free(runtime);
    return 0;
}

