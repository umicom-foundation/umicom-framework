/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_history_identity.c
 * PURPOSE: Check operation identities across asynchronous and synchronous history writers.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/build/history.h"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(test) do { if (!(test)) { fprintf(stderr, "line %d: %s\n", __LINE__, #test); return EXIT_FAILURE; } } while (0)
int main(void)
{
    UmiBuildHistory *history = NULL;
    UmiBuildResult *result = NULL;
    uint64_t id = 0U;
    CHECK(umi_build_history_create(2U, &history) == UMI_STATUS_OK);
    CHECK(umi_build_result_create(&result) == UMI_STATUS_OK);
    CHECK(UmiBuildHistoryReserveOperationId(history, &id) == UMI_STATUS_OK && id == 1U);
    CHECK(UmiBuildHistoryReserveOperationId(history, &id) == UMI_STATUS_OK && id == 2U);
    umi_build_result_init(result, 20U, UMI_BUILD_PHASE_BUILD, "notes");
    CHECK(umi_build_history_append(history, result) == UMI_STATUS_OK);
    CHECK(UmiBuildHistoryReserveOperationId(history, &id) == UMI_STATUS_OK && id == 21U);
    umi_build_history_clear(history);
    CHECK(UmiBuildHistoryReserveOperationId(history, &id) == UMI_STATUS_OK && id == 22U);
    result->operation_id = UINT64_MAX - 1U;
    CHECK(umi_build_history_append(history, result) == UMI_STATUS_OK);
    CHECK(UmiBuildHistoryReserveOperationId(history, &id) == UMI_STATUS_OK && id == UINT64_MAX);
    id = 77U;
    CHECK(UmiBuildHistoryReserveOperationId(history, &id) == UMI_STATUS_CAPACITY_EXCEEDED && id == 77U);
    umi_build_history_clear(history);
    CHECK(UmiBuildHistoryReserveOperationId(history, &id) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(UmiBuildHistoryReserveOperationId(NULL, &id) == UMI_STATUS_INVALID_ARGUMENT);
    umi_build_history_destroy(history);
    umi_build_result_destroy(result);
    puts("History identities remain unique across retained and cleared history.");
    return EXIT_SUCCESS;
}
