/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout_data/test_backend_contract.c
 *
 * PURPOSE:
 *   Verify Data Server backend identity, record visitation, transaction state and authoritative namespace interoperability.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

typedef struct VisitState {
    size_t count;
    size_t owned_count;
    size_t foreign_count;
} VisitState;

static UmiStatus count_record(
    const char *key,
    const char *value,
    void *context)
{
    VisitState *state = (VisitState *)context;
    if (key == NULL || value == NULL || state == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    state->count += 1U;
    if (umi_workbench_layout_data_key_is_owned(key)) {
        state->owned_count += 1U;
    } else {
        state->foreign_count += 1U;
    }
    return UMI_STATUS_OK;
}

static int test_memory_backend_identity(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiDataServerSnapshot snapshot;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_REQUIRE(umi_data_server_backend(server) ==
        UMI_DATA_BACKEND_MEMORY, "memory backend enum");
    TEST_REQUIRE(strcmp(umi_data_server_backend_name(server),
        "memory") == 0, "memory backend name");
    TEST_STATUS_OK(umi_data_server_snapshot(server, &snapshot));
    TEST_REQUIRE(snapshot.backend == UMI_DATA_BACKEND_MEMORY,
                 "snapshot backend");
    TEST_REQUIRE(strcmp(snapshot.path, ":memory:") == 0,
                 "snapshot path");
    umi_data_server_destroy(server);
    return 0;
}

static int test_visitation_and_namespace(void)
{
    UmiDataServer *server = test_create_data_server();
    VisitState state;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_data_server_set(
        server, "workbench-layout/metadata/schema-version", "1"));
    TEST_STATUS_OK(umi_data_server_set(
        server, "workbench-layout/layout-manifest/layout.a", "manifest"));
    TEST_STATUS_OK(umi_data_server_set(
        server, "other/application/key", "value"));
    (void)memset(&state, 0, sizeof(state));
    TEST_STATUS_OK(umi_data_server_visit(server, count_record, &state));
    TEST_REQUIRE(state.count == 3U, "all records visited");
    TEST_REQUIRE(state.owned_count == 2U,
                 "owned records identified");
    TEST_REQUIRE(state.foreign_count == 1U,
                 "foreign records identified");
    umi_data_server_destroy(server);
    return 0;
}

static int test_transaction_state(void)
{
    UmiDataServer *server = test_create_data_server();
    TEST_REQUIRE(server != NULL, "create server");
    TEST_REQUIRE(!umi_data_server_in_transaction(server),
                 "initially no transaction");
    TEST_STATUS_OK(umi_data_server_begin(server));
    TEST_REQUIRE(umi_data_server_in_transaction(server),
                 "transaction active");
    TEST_STATUS_OK(umi_data_server_set(
        server, "workbench-layout/metadata/transaction", "pending"));
    TEST_STATUS_OK(umi_data_server_commit(server));
    TEST_REQUIRE(!umi_data_server_in_transaction(server),
                 "transaction closed after commit");
    umi_data_server_destroy(server);
    return 0;
}

static int test_server_snapshot_count(void)
{
    UmiDataServer *server = test_create_data_server();
    UmiDataServerSnapshot snapshot;
    TEST_REQUIRE(server != NULL, "create server");
    TEST_STATUS_OK(umi_data_server_set(server, "a", "1"));
    TEST_STATUS_OK(umi_data_server_set(server, "b", "2"));
    TEST_STATUS_OK(umi_data_server_snapshot(server, &snapshot));
    TEST_REQUIRE(snapshot.record_count == 2U,
                 "snapshot count accurate");
    TEST_REQUIRE(snapshot.transaction_active == 0,
                 "snapshot transaction state");
    umi_data_server_destroy(server);
    return 0;
}

int main(void)
{
    TEST_REQUIRE(test_memory_backend_identity() == 0,
                 "memory backend");
    TEST_REQUIRE(test_visitation_and_namespace() == 0,
                 "visitation namespace");
    TEST_REQUIRE(test_transaction_state() == 0,
                 "transaction state");
    TEST_REQUIRE(test_server_snapshot_count() == 0,
                 "snapshot count");
    return 0;
}
