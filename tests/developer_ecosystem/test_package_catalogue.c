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
#include "../value_archive/revision_cases.h"
#include <assert.h>

#include "umicom/developer/ecosystem/package_catalogue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
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
    UMI_TEST_REVISION_EXHAUSTION(UmiEcosystemPackageCatalogue, &catalogue,
        (umi_ecosystem_package_catalogue_upsert(&catalogue, &record)));
    /* Replacing an existing child must not wrap its independent token. */
    {
        uint64_t input_revision = record.revision;
        uint64_t owner_revision = catalogue.revision;
        unsigned char child_before[sizeof(catalogue.packages[0])];
        memcpy(child_before, &catalogue.packages[0], sizeof(child_before));
        record.revision = UINT64_MAX;
        UmiStatus replace_status = umi_ecosystem_package_catalogue_upsert(&catalogue, &record);
        record.revision = input_revision;
        if (replace_status != UMI_STATUS_CAPACITY_EXCEEDED ||
            catalogue.revision != owner_revision ||
            memcmp(child_before, &catalogue.packages[0], sizeof(child_before)) != 0)
            return 1;
    }

    assert(umi_ecosystem_package_has_update(&catalogue.packages[0]));
    assert(umi_ecosystem_package_catalogue_statistics(&catalogue, &statistics) ==
        UMI_STATUS_OK);
    assert(statistics.total == 1U && statistics.installed == 1U);
    assert(statistics.updates == 1U && statistics.extensions == 1U);
    return 0;
}
