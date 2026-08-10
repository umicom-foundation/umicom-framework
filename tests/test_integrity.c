/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integrity.c
 *
 * PURPOSE:
 *   Verify portable Data Server integrity reports for the memory backend.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiDataServer *server = NULL;
    UmiDataIntegrityReport report;
    assert(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    assert(umi_data_server_set(server, "a", "b") == UMI_STATUS_OK);
    assert(umi_data_integrity_check(server, &report) == UMI_STATUS_OK);
    assert(report.backend == UMI_DATA_BACKEND_MEMORY);
    assert(report.records == 1U);
    assert(strcmp(report.backend_name, "memory") == 0);
    umi_data_server_destroy(server);
    return 0;
}
