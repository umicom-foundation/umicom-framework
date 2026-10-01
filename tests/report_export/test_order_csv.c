/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/report_export/test_order_csv.c
 * PURPOSE: Verify applied-query exports remain independent of orders and workspace lifetime.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#include "../trading_execution/order_review_fixture.h"
#include "umicom/trading/order_csv.h"
#define CHECK REVIEW_CHECK
#define OK(x) CHECK((x) == UMI_STATUS_OK)
int main(int argc, char **argv)
{
    CHECK(argc == 2); ReviewFixture f; ReviewFixtureInit(&f);
    UmiTradingOrderQuery query = {UMI_TRADING_WORKSPACE_ORDERS_ALL, ""};
    UmiCsvDocument *doc = NULL;
    if (strcmp(argv[1], "filter") == 0) strcpy(query.text, "nq");
    else if (strcmp(argv[1], "empty") == 0) strcpy(query.text, "missing");
    else if (strcmp(argv[1], "formula") == 0) strcpy(query.text, "=1+2");
    OK(UmiTradingWorkspaceSetOrderQuery(f.workspace, &query));
    UmiTradingWorkspaceSnapshot before, after;
    OK(umi_trading_workspace_snapshot(f.workspace, &before));
    OK(UmiTradingWorkspaceExportOrdersCsv(f.workspace, &doc));
    OK(umi_trading_workspace_snapshot(f.workspace, &after));
    CHECK(before.revision == after.revision && before.order_count == after.order_count);
    CHECK(strcmp(before.selected_order_id, after.selected_order_id) == 0);
    CHECK(memcmp(&before.draft_order, &after.draft_order, sizeof(before.draft_order)) == 0);
    CHECK(UmiCsvDocumentRows(doc) == 2U + before.visible_order_count);
    const char *text = UmiCsvDocumentData(doc);
    CHECK(strstr(text, "retained-order-summary") != NULL);
    if (strcmp(argv[1], "filter") == 0) {
        CHECK(strstr(text, f.first) != NULL && strstr(text, f.second) == NULL);
    } else if (strcmp(argv[1], "empty") == 0 || strcmp(argv[1], "formula") == 0) {
        CHECK(UmiCsvDocumentRows(doc) == 2);
        CHECK(strstr(text, f.first) == NULL && strstr(text, f.second) == NULL);
        if (strcmp(argv[1], "formula") == 0) CHECK(strstr(text, "\"'=1+2\"") != NULL);
    } else if (strcmp(argv[1], "ownership") == 0) {
        size_t bytes = UmiCsvDocumentBytes(doc);
        char *copy = malloc(bytes + 1U); CHECK(copy != NULL); memcpy(copy, text, bytes + 1U);
        UmiExecutionReport fill = ReviewFixtureFill(f.first);
        OK(umi_trading_workspace_record_execution(f.workspace, &fill));
        umi_trading_workspace_destroy(f.workspace); f.workspace = NULL;
        CHECK(strcmp(copy, UmiCsvDocumentData(doc)) == 0); free(copy);
    } else {
        CHECK(strcmp(argv[1], "invalid") == 0);
        UmiCsvDocument *bad = doc;
        CHECK(UmiTradingWorkspaceExportOrdersCsv(NULL, &bad) == UMI_STATUS_INVALID_ARGUMENT && bad == NULL);
        CHECK(UmiTradingWorkspaceExportOrdersCsv(f.workspace, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    }
    UmiCsvDocumentDestroy(doc); umi_trading_workspace_destroy(f.workspace); return 0;
}
