/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_schema_catalogue.c
 *
 * PURPOSE:
 *   Verify seeded Data Server collections, stable schema hashing, duplicate detection and structural validation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

static int test_seeded_catalogue(void)
{
    UmiWorkbenchLayoutDataSchemaCatalogue catalogue;
    uint64_t hash;
    umi_workbench_layout_data_schema_catalogue_init(&catalogue);
    TEST_STATUS_OK(umi_workbench_layout_data_schema_catalogue_seed(
        &catalogue));
    TEST_STATUS_OK(umi_workbench_layout_data_schema_catalogue_validate(
        &catalogue));
    TEST_REQUIRE(catalogue.count > 10U,
                 "all persistence collections seeded");
    TEST_REQUIRE(catalogue.current_version ==
        UMI_WORKBENCH_LAYOUT_DATA_SCHEMA_VERSION,
        "catalogue schema version");
    hash = umi_workbench_layout_data_schema_catalogue_hash(&catalogue);
    TEST_REQUIRE(hash != 0U, "catalogue hash available");
    TEST_REQUIRE(hash ==
        umi_workbench_layout_data_schema_catalogue_hash(&catalogue),
        "catalogue hash deterministic");
    return 0;
}

static int test_custom_collection(void)
{
    UmiWorkbenchLayoutDataSchemaCatalogue catalogue;
    UmiWorkbenchLayoutDataCollectionDescriptor descriptor;
    umi_workbench_layout_data_schema_catalogue_init(&catalogue);
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = sizeof(descriptor);
    test_copy_text(descriptor.name,
                   sizeof(descriptor.name), "layout-audit");
    test_copy_text(descriptor.key_prefix,
                   sizeof(descriptor.key_prefix),
                   "workbench-layout/audit/");
    descriptor.kind = UMI_WORKBENCH_LAYOUT_DATA_RECORD_METADATA;
    descriptor.schema_version = 1U;
    descriptor.schema_hash =
        umi_workbench_layout_data_hash_text("layout-audit");
    descriptor.authoritative = true;
    descriptor.durable = true;
    descriptor.replicated = true;
    descriptor.retention_managed = true;
    TEST_STATUS_OK(umi_workbench_layout_data_schema_catalogue_add(
        &catalogue, &descriptor));
    TEST_REQUIRE(catalogue.count == 1U,
                 "custom collection added");
    TEST_REQUIRE(umi_workbench_layout_data_schema_catalogue_add(
        &catalogue, &descriptor) == UMI_STATUS_ALREADY_EXISTS,
        "duplicate collection rejected");
    TEST_STATUS_OK(umi_workbench_layout_data_schema_catalogue_validate(
        &catalogue));
    return 0;
}

static int test_invalid_collection(void)
{
    UmiWorkbenchLayoutDataSchemaCatalogue catalogue;
    UmiWorkbenchLayoutDataCollectionDescriptor descriptor;
    umi_workbench_layout_data_schema_catalogue_init(&catalogue);
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = sizeof(descriptor);
    descriptor.schema_version = 1U;
    TEST_REQUIRE(umi_workbench_layout_data_schema_catalogue_add(
        &catalogue, &descriptor) == UMI_STATUS_INVALID_ARGUMENT,
        "missing collection identity rejected");
    test_copy_text(descriptor.name,
                   sizeof(descriptor.name), "invalid");
    test_copy_text(descriptor.key_prefix,
                   sizeof(descriptor.key_prefix), "foreign/");
    TEST_REQUIRE(umi_workbench_layout_data_schema_catalogue_add(
        &catalogue, &descriptor) == UMI_STATUS_INVALID_ARGUMENT,
        "foreign namespace rejected");
    return 0;
}

static int test_tampered_catalogue(void)
{
    UmiWorkbenchLayoutDataSchemaCatalogue catalogue;
    umi_workbench_layout_data_schema_catalogue_init(&catalogue);
    TEST_STATUS_OK(umi_workbench_layout_data_schema_catalogue_seed(
        &catalogue));
    catalogue.collections[0].schema_version = 0U;
    TEST_REQUIRE(umi_workbench_layout_data_schema_catalogue_validate(
        &catalogue) == UMI_STATUS_INVALID_STATE,
        "zero collection schema rejected");
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_seeded_catalogue() == 0,
                 "seeded catalogue");
    TEST_REQUIRE(test_custom_collection() == 0,
                 "custom collection");
    TEST_REQUIRE(test_invalid_collection() == 0,
                 "invalid collection");
    TEST_REQUIRE(test_tampered_catalogue() == 0,
                 "tampered catalogue");
    return 0;
}
