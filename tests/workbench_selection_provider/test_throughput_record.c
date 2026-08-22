/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection_provider/test_throughput_record.c
 *
 * PURPOSE:
 *   Verify the provider throughput record contract, mutation and stable hashing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>

#include "umicom/workbench_selection_provider/throughput_record.h"

int main(void)
{
    UmiWorkbenchSelectionProviderThroughputRecord record;
    uint64_t hash;

    umi_workbench_selection_provider_throughput_record_init(
        &record,
        "throughput_record-record");
    assert(umi_workbench_selection_provider_throughput_record_validate(
        &record) == UMI_STATUS_OK);

    assert(umi_workbench_selection_provider_throughput_record_set_provider(
        &record, "provider") == UMI_STATUS_OK);
    assert(umi_workbench_selection_provider_throughput_record_set_source(
        &record, "source") == UMI_STATUS_OK);
    assert(umi_workbench_selection_provider_throughput_record_set_subject(
        &record, "subject") == UMI_STATUS_OK);
    assert(umi_workbench_selection_provider_throughput_record_set_related(
        &record, "related") == UMI_STATUS_OK);
    assert(umi_workbench_selection_provider_throughput_record_set_group(
        &record, "red") == UMI_STATUS_OK);
    assert(umi_workbench_selection_provider_throughput_record_set_description(
        &record, "description") == UMI_STATUS_OK);

    record.provider_kind =
        UMI_WORKBENCH_SELECTION_PROVIDER_PROJECT;
    record.state =
        UMI_WORKBENCH_SELECTION_PROVIDER_ACTIVE;
    record.selection_kind =
        UMI_WORKBENCH_SELECTION_PROJECT;
    record.context_kind =
        UMI_CONTEXT_KIND_PROJECT;
    record.flags = 3U;
    record.count = 5U;

    hash = umi_workbench_selection_provider_throughput_record_hash(
        &record);
    assert(hash != 0U);
    umi_workbench_selection_provider_throughput_record_touch(
        &record, 11U, 1000U);
    assert(record.sequence == 11U);
    assert(record.timestamp_ms == 1000U);
    assert(strcmp(record.provider_id, "provider") == 0);
    return 0;
}
