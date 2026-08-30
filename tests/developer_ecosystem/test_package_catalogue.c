/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_ecosystem/test_package_catalogue.c
 * PURPOSE: Verify package upsert, version state and catalogue summaries.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer/ecosystem/package_catalogue.h"

int main(void)
{
    UmiEcosystemPackageCatalogue catalogue;
    UmiEcosystemPackageRecord record;
    UmiEcosystemPackageStatistics statistics;
    umi_ecosystem_package_catalogue_init(&catalogue);
    umi_ecosystem_package_record_init(&record, "org.umicom.c23", "C23 Tools",
        UMI_ECOSYSTEM_PACKAGE_EXTENSION, "official", "2.0.0");
    record.installed = true;
    record.state = UMI_ECOSYSTEM_UPDATE_AVAILABLE;
    record.compatibility = UMI_ECOSYSTEM_COMPATIBLE;
    record.evidence = UMI_ECOSYSTEM_EVIDENCE_VERIFIED;
    assert(umi_ecosystem_copy_text(record.installed_version,
        sizeof(record.installed_version), "1.0.0") == UMI_STATUS_OK);
    assert(umi_ecosystem_package_catalogue_upsert(&catalogue, &record) ==
        UMI_STATUS_OK);
    assert(umi_ecosystem_package_has_update(&catalogue.packages[0]));
    assert(umi_ecosystem_package_catalogue_statistics(&catalogue, &statistics) ==
        UMI_STATUS_OK);
    assert(statistics.total == 1U && statistics.installed == 1U);
    assert(statistics.updates == 1U && statistics.extensions == 1U);
    return 0;
}
