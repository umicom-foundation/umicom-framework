/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/chart_checkpoint/test_store.c
 * PURPOSE: Verify transactional saves, scope isolation, conflicts and last-good recovery.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "fixture.h"
typedef struct Search { const char *suffix; char key[192]; } Search;
static UmiStatus FindKey(const char *key, const char *value, void *data)
{
    Search *search = data; (void)value;
    if (strstr(key, search->suffix) != NULL) { CHECK(search->key[0] == '\0'); strcpy(search->key, key); }
    return UMI_STATUS_OK;
}
static void Key(UmiDataServer *server, const char *suffix, char output[192])
{ Search search = {suffix, {0}}; CHECK(umi_data_server_visit(server, FindKey, &search) == UMI_STATUS_OK && search.key[0]); strcpy(output, search.key); }
int main(int argc, char **argv)
{
    CHECK(argc == 2); UmiDataServer *server = NULL; CHECK(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    UmiChartDocument *first = Document("NQ", 110), *second = Document("NQ", 120), *loaded = NULL;
    UmiChartCheckpointReport report;
    CHECK(UmiChartCheckpointLoad(server, "local", "NQ", &loaded, &report) == UMI_STATUS_NOT_FOUND && loaded == NULL && report.storage_revision_known);
    CHECK(UmiChartCheckpointSave(server, "local", first, 0U, 1000U, &report) == UMI_STATUS_OK && report.storage_revision == 1U && !report.durable);
    if (strcmp(argv[1], "conflict") == 0) {
        CHECK(UmiChartCheckpointSave(server, "local", second, 0U, 1001U, &report) == UMI_STATUS_INVALID_STATE);
        CHECK(UmiChartCheckpointLoad(server, "local", "NQ", &loaded, &report) == UMI_STATUS_OK && Value(loaded) == 110 && report.storage_revision == 1U);
    } else if (strcmp(argv[1], "transaction") == 0) {
        CHECK(umi_data_server_begin(server) == UMI_STATUS_OK);
        CHECK(UmiChartCheckpointSave(server, "local", second, 1U, 1001U, &report) == UMI_STATUS_BUSY);
        CHECK(UmiChartCheckpointLoad(server, "local", "NQ", &loaded, &report) == UMI_STATUS_BUSY && loaded == NULL);
        CHECK(umi_data_server_in_transaction(server)); CHECK(umi_data_server_rollback(server) == UMI_STATUS_OK);
    } else if (strcmp(argv[1], "scope") == 0) {
        CHECK(UmiChartCheckpointSave(server, "another", second, 0U, 1001U, &report) == UMI_STATUS_OK);
        CHECK(UmiChartCheckpointLoad(server, "local", "ES", &loaded, &report) == UMI_STATUS_NOT_FOUND);
        CHECK(UmiChartCheckpointLoad(server, "local", "NQ", &loaded, &report) == UMI_STATUS_OK && Value(loaded) == 110);
    } else if (strcmp(argv[1], "rollback") == 0) {
        /* Fill the bounded backend. The next save begins writing its backup
         * before capacity fails, so rollback must remove that partial side. */
        for (size_t i = 0U; ; ++i) { char key[64]; (void)snprintf(key, sizeof(key), "other.%zu", i);
            UmiStatus status = umi_data_server_set(server, key, "retained");
            if (status == UMI_STATUS_CAPACITY_EXCEEDED) break;
            CHECK(status == UMI_STATUS_OK && i < 10000U); }
        CHECK(umi_data_server_delete(server, "other.0") == UMI_STATUS_OK);
        size_t count = umi_data_server_count(server);
        CHECK(UmiChartCheckpointSave(server, "local", second, 1U, 1001U, &report) == UMI_STATUS_CAPACITY_EXCEEDED);
        CHECK(umi_data_server_count(server) == count && !umi_data_server_in_transaction(server));
        CHECK(UmiChartCheckpointLoad(server, "local", "NQ", &loaded, &report) == UMI_STATUS_OK && Value(loaded) == 110 && report.storage_revision == 1U);
    } else {
        CHECK(UmiChartCheckpointSave(server, "local", second, 1U, 2000U, &report) == UMI_STATUS_OK && report.storage_revision == 2U);
        char key[192];
        if (strcmp(argv[1], "recovery") == 0 || strcmp(argv[1], "double-corruption") == 0 || strcmp(argv[1], "digest") == 0) {
            Key(server, ".primary.drawing.0", key);
            if (strcmp(argv[1], "digest") == 0) {
                char value[3900]; CHECK(umi_data_server_get(server, key, value, sizeof(value)) == UMI_STATUS_OK);
                char *identity = strstr(value, "id=drawing.alpha"); CHECK(identity != NULL); identity[3] = 'z';
                CHECK(umi_data_server_set(server, key, value) == UMI_STATUS_OK);
            } else CHECK(umi_data_server_set(server, key, "broken") == UMI_STATUS_OK);
            if (strcmp(argv[1], "double-corruption") == 0) {
                Key(server, ".last-good.manifest", key); CHECK(umi_data_server_set(server, key, "broken") == UMI_STATUS_OK);
                CHECK(UmiChartCheckpointLoad(server, "local", "NQ", &loaded, &report) == UMI_STATUS_PARSE_ERROR && loaded == NULL);
            } else {
                CHECK(UmiChartCheckpointLoad(server, "local", "NQ", &loaded, &report) == UMI_STATUS_OK && Value(loaded) == 110);
                CHECK(report.recovered_last_good && !report.storage_revision_known && report.primary_status == UMI_STATUS_PARSE_ERROR && report.saved_at_ms == 1000U && report.checkpoint_revision == 1U);
                CHECK(UmiChartCheckpointSave(server, "local", second, 2U, 3000U, &report) == UMI_STATUS_PARSE_ERROR);
            }
        } else if (strcmp(argv[1], "missing-primary") == 0) {
            Key(server, ".primary.manifest", key); CHECK(umi_data_server_delete(server, key) == UMI_STATUS_OK);
            CHECK(UmiChartCheckpointLoad(server, "local", "NQ", &loaded, &report) == UMI_STATUS_OK && report.recovered_last_good && !report.storage_revision_known);
            CHECK(UmiChartCheckpointSave(server, "local", second, 0U, 3000U, &report) == UMI_STATUS_INVALID_STATE);
        } else {
            CHECK(strcmp(argv[1], "empty") == 0);
            UmiChartNavigation navigation = {0}; UmiChartDocument *empty = NULL;
            CHECK(UmiChartDocumentCreate("NQ", &navigation, NULL, 0U, 8U, &empty) == UMI_STATUS_OK);
            CHECK(UmiChartCheckpointSave(server, "local", empty, 2U, 3000U, &report) == UMI_STATUS_OK);
            CHECK(UmiChartCheckpointLoad(server, "local", "NQ", &loaded, &report) == UMI_STATUS_OK && report.drawing_count == 0U && report.storage_revision == 3U);
            CHECK(umi_data_server_count(server) == 3U); UmiChartDocumentDestroy(empty);
        }
    }
    UmiChartDocumentDestroy(first); UmiChartDocumentDestroy(second); UmiChartDocumentDestroy(loaded);
    umi_data_server_destroy(server); return 0;
}
