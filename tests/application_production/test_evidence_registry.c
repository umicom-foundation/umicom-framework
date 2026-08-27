/* Umicom Framework application production test | evidence_registry | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "test_fixture.h"
int main(void) {
    UmiApplicationProductionEvidenceRegistry registry;
    umi_application_production_evidence_registry_init(&registry);
    assert(umi_application_production_evidence_registry_record(&registry, "org.umicom.studio:tests", UMI_APPLICATION_PRODUCTION_EVIDENCE_TEST, UMI_APPLICATION_PRODUCTION_EVIDENCE_ACCEPTED, "ctest:studio") == UMI_STATUS_OK);
    assert(umi_application_production_evidence_registry_find(&registry, "org.umicom.studio:tests") != NULL);
    assert(registry.count == 1U);
    return 0;
}

