/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_ecosystem/test_source_catalogue.c
 * PURPOSE: Verify remote, cached and offline source availability.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "../value_archive/revision_cases.h"
#include <assert.h>

#include "umicom/developer/ecosystem/source_catalogue.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEcosystemSourceCatalogue catalogue;
    UmiEcosystemSource remote;
    UmiEcosystemSource offline;
    umi_ecosystem_source_catalogue_init(&catalogue);
    umi_ecosystem_source_init(&remote, "official", "Official",
        UMI_ECOSYSTEM_SOURCE_REMOTE, "https://packages.example.test");
    remote.supports_offline = true;
    assert(umi_ecosystem_copy_text(remote.cache_path,
        sizeof(remote.cache_path), "cache/official") == UMI_STATUS_OK);
    umi_ecosystem_source_init(&offline, "media", "Offline Media",
        UMI_ECOSYSTEM_SOURCE_OFFLINE, "D:/umicom-feed");
    assert(umi_ecosystem_source_catalogue_upsert(&catalogue, &remote) ==
        UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiEcosystemSourceCatalogue, &catalogue,
        (umi_ecosystem_source_catalogue_upsert(&catalogue, &remote)));
    /* Replacing an existing child must not wrap its independent token. */
    {
        uint64_t input_revision = remote.revision;
        uint64_t owner_revision = catalogue.revision;
        unsigned char child_before[sizeof(catalogue.sources[0])];
        memcpy(child_before, &catalogue.sources[0], sizeof(child_before));
        remote.revision = UINT64_MAX;
        UmiStatus replace_status = umi_ecosystem_source_catalogue_upsert(&catalogue, &remote);
        remote.revision = input_revision;
        if (replace_status != UMI_STATUS_CAPACITY_EXCEEDED ||
            catalogue.revision != owner_revision ||
            memcmp(child_before, &catalogue.sources[0], sizeof(child_before)) != 0)
            return 1;
    }

    assert(umi_ecosystem_source_catalogue_upsert(&catalogue, &offline) ==
        UMI_STATUS_OK);
    assert(umi_ecosystem_source_available_count(&catalogue) == 2U);
    assert(umi_ecosystem_source_catalogue_set_offline(&catalogue, true) ==
        UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiEcosystemSourceCatalogue, &catalogue,
        (umi_ecosystem_source_catalogue_set_offline(&catalogue, true)));

    assert(umi_ecosystem_source_available_count(&catalogue) == 2U);
    remote.cache_path[0] = '\0';
    assert(umi_ecosystem_source_catalogue_upsert(&catalogue, &remote) ==
        UMI_STATUS_OK);
    assert(umi_ecosystem_source_available_count(&catalogue) == 1U);
    assert(umi_ecosystem_source_complete_sync(&catalogue, "media",
        UMI_STATUS_OK, 1000U) == UMI_STATUS_OK);
    UMI_TEST_REVISION_EXHAUSTION(UmiEcosystemSourceCatalogue, &catalogue,
        (umi_ecosystem_source_complete_sync(&catalogue, "media",
        UMI_STATUS_OK, 1000U)));

    return 0;
}
