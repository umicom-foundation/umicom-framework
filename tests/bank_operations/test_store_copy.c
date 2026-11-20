/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/bank_operations/test_store_copy.c
 *
 * PURPOSE:
 *   Verify new-file banking copies, preserved originals and source admission.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../data_history/readonly_fixture.h"
#include "umicom/bank_operations/operations.h"
#include "umicom/platform/input_file.h"

/* CTest supplies an isolated existing directory. Reserve a unique sibling
 * using the shared fixture helper, then keep all evidence under that root. */
static bool StorePaths(const char *directory, const char *name, char source[1024], char destination[1024])
{
    char marker[1024];
    if (!ro_path(directory, name, marker, true)) return false;
    int source_length = snprintf(source, 1024, "%s.source.sqlite", marker);
    int target_length = snprintf(destination, 1024, "%s.destination.sqlite", marker);
    return source_length > 0 && source_length < 1024 && target_length > 0 && target_length < 1024;
}

/* A real accepted fictional customer gives replay an observable revision.
 * Both stores have their own handles and can change independently later. */
static UmiStatus AddPracticeCustomer(UmiBankOperations *store, const char *id, uint64_t revision)
{
    UmiBankActor maker = {.capabilities = UMI_BANK_CAP_ALL};
    UmiBankCommand command;
    UmiBankReceipt receipt;
    UmiBankCommandInit(&command, UMI_BANK_CUSTOMER_CREATE);
    UmiStatus status = umi_financial_id_assign(&maker.id, "copy-test-maker");
    if (status == UMI_STATUS_OK) status = umi_financial_id_assign(&command.id, id);
    if (status == UMI_STATUS_OK) status = umi_financial_id_assign(&command.requestId, id);
    if (status != UMI_STATUS_OK) return status;
    (void)snprintf(command.name, sizeof command.name, "Practice customer %s", id);
    command.businessDate = (UmiFinancialDate){2026, 1U, 2U};
    command.timestampMillis = 1;
    command.expectedRevision = revision;
    return UmiBankOperationsExecute(store, &maker, &command, &receipt);
}

int main(int argc, char **argv)
{
    const char *const cases[] = {"roundtrip", "existing", "same-source", "missing-source", "unrelated",
        "corrupt-history", "relative-path", "unwritable-parent"};
    if (argc != 3 || !ro_case(argv[1], cases, sizeof cases / sizeof cases[0])) return 2;
    const char *name = argv[1];
    int result = 0;
    UmiBankOperations *original = NULL, *copy = NULL;
    UmiDataServer *foreign = NULL;
    unsigned char *source_bytes = NULL, *after_bytes = NULL;
    size_t source_length = 0, after_length = 0;
    char source[1024], destination[1024];
    UmiBankCounts counts = {.revision = 999U, .events = 7U};
    unsigned char sentinel[sizeof counts];
    memcpy(sentinel, &counts, sizeof counts);

    CHECK(StorePaths(argv[2], name, source, destination));
    if (strcmp(name, "relative-path") == 0) {
        CHECK(UmiBankOperationsCopyExistingSqlite("relative.sqlite", destination, &counts) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBankOperationsCopyExistingSqlite(source, "relative.sqlite", &counts) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(UmiBankOperationsCopyExistingSqlite(source, destination, NULL) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(memcmp(&counts, sentinel, sizeof counts) == 0);
        goto cleanup;
    }

    /* Probe the actual backend while keeping both copy paths untouched for the
     * missing-source check. SQLite-disabled builds report their explicit skip. */
    char backend_marker[1024];
    CHECK(ro_path(argv[2], "copy-backend", backend_marker, true));
    UmiStatus backend = umi_data_server_create_sqlite(backend_marker, &foreign);
    if (backend == UMI_STATUS_UNAVAILABLE) { puts("SKIP: SQLite unavailable"); return 77; }
    OK(backend);
    umi_data_server_destroy(foreign);
    foreign = NULL;

    if (strcmp(name, "unrelated") == 0 || strcmp(name, "corrupt-history") == 0) {
        /* Model valid SQLite files that Bank cannot admit. Neither failure may
         * create the destination or change the original schema or history. */
        OK(umi_data_server_create_sqlite(source, &foreign));
        if (strcmp(name, "unrelated") == 0)
            OK(umi_data_server_execute(foreign, "DROP TABLE umicom_kv; CREATE TABLE other(value TEXT);"));
        else
            OK(umi_data_server_set(foreign, "bank.operations.revision", "broken ledger"));
        umi_data_server_destroy(foreign);
        foreign = NULL;
    } else if (strcmp(name, "missing-source") != 0) {
        OK(UmiBankOperationsOpenSqlite(source, &original));
        OK(AddPracticeCustomer(original, "copied-customer", 0U));
        UmiBankOperationsDestroy(original);
        original = NULL;
    }
    bool source_present = strcmp(name, "missing-source") != 0;
    if (source_present) OK(UmiInputFileRead(source, 1048576U, &source_bytes, &source_length));

    if (strcmp(name, "roundtrip") == 0) {
        OK(UmiBankOperationsCopyExistingSqlite(source, destination, &counts));
        CHECK(counts.revision == 1U && counts.events == 1U && counts.customers == 1U);
        OK(UmiBankOperationsOpenExistingSqlite(destination, &copy));
        OK(AddPracticeCustomer(copy, "new-copy-customer", 1U));
        OK(UmiBankOperationsCounts(copy, &counts));
        CHECK(counts.revision == 2U && counts.customers == 2U);
        OK(UmiBankOperationsOpenExistingSqlite(source, &original));
        OK(UmiBankOperationsCounts(original, &counts));
        CHECK(counts.revision == 1U && counts.customers == 1U);
    } else if (strcmp(name, "existing") == 0 || strcmp(name, "same-source") == 0) {
        const char *target = source;
        if (strcmp(name, "existing") == 0) {
            /* A complete separate ledger proves collision refusal preserves
             * an already valid destination and its independent history. */
            target = destination;
            OK(UmiBankOperationsOpenSqlite(destination, &copy));
            OK(AddPracticeCustomer(copy, "keep-existing-customer", 0U));
            OK(AddPracticeCustomer(copy, "keep-another-customer", 1U));
            UmiBankOperationsDestroy(copy);
            copy = NULL;
        }
        CHECK(UmiBankOperationsCopyExistingSqlite(source, target, &counts) == UMI_STATUS_ALREADY_EXISTS);
        CHECK(memcmp(&counts, sentinel, sizeof counts) == 0);
        if (target == destination) {
            OK(UmiBankOperationsOpenExistingSqlite(destination, &copy));
            OK(UmiBankOperationsCounts(copy, &counts));
            CHECK(counts.revision == 2U && counts.customers == 2U);
        }
    } else {
        if (strcmp(name, "unwritable-parent") == 0) {
            /* A regular file cannot act as the parent of the requested copy. */
            int written = snprintf(destination, sizeof destination, "%s/copy.sqlite", source);
            CHECK(written > 0 && (size_t)written < sizeof destination);
        }
        CHECK(UmiBankOperationsCopyExistingSqlite(source, destination, &counts) != UMI_STATUS_OK);
        CHECK(memcmp(&counts, sentinel, sizeof counts) == 0);
        CHECK(UmiInputFileRead(destination, 1048576U, &after_bytes, &after_length) != UMI_STATUS_OK);
    }
    /* Close the SQLite handles before observing the raw file. A Windows
     * reader that checks file identity will refuse a still-open writer. */
    UmiBankOperationsDestroy(original);
    original = NULL;
    UmiBankOperationsDestroy(copy);
    copy = NULL;
    if (source_present) {
        OK(UmiInputFileRead(source, 1048576U, &after_bytes, &after_length));
        CHECK(after_length == source_length && memcmp(after_bytes, source_bytes, source_length) == 0);
    } else {
        CHECK(UmiInputFileRead(source, 1048576U, &after_bytes, &after_length) != UMI_STATUS_OK);
    }

cleanup:
    UmiBankOperationsDestroy(original);
    UmiBankOperationsDestroy(copy);
    umi_data_server_destroy(foreign);
    UmiInputFileFree(source_bytes);
    UmiInputFileFree(after_bytes);
    return result;
}
