/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_execution/test_draft_admission.c
 *
 * PURPOSE:
 *   Verify draft command admission preserves the complete owner and existing preview evidence at revision capacity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../trading/test_trading_common.h"
/* Inject boundary state into the real opaque owner. This does not introduce
 * a mutable capacity setter into the installed public API. */
#include "../../src/trading/workspace.c"
#define DRAFT_CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #x); result = 1; goto done; } } while (0)

typedef enum DraftOperation { SIDE, TYPE, QUANTITY, PRICES, POLICY, RESET } DraftOperation;
static UmiStatus ApplyDraft(UmiTradingWorkspace *w, DraftOperation op, int repeated, int invalid)
{
    switch (op) {
    case SIDE:
        return umi_trading_workspace_set_draft_side(w,
            invalid ? (UmiSide)99 : repeated ? w->draft_order.side : UMI_SIDE_SELL);
    case TYPE:
        return umi_trading_workspace_set_draft_type(w,
            invalid ? (UmiOrderType)99 : repeated ? w->draft_order.type : UMI_ORDER_MARKET,
            repeated ? w->draft_order.tif : UMI_TIF_GTC);
    case QUANTITY:
        return umi_trading_workspace_set_draft_quantity(w,
            invalid ? NAN : repeated ? w->draft_order.quantity : 7.0);
    case PRICES:
        return umi_trading_workspace_set_draft_prices(w,
            invalid ? -1.0 : repeated ? w->draft_order.limit_price : 90.0,
            repeated ? w->draft_order.stop_price : 95.0);
    case POLICY: {
        UmiRiskPricePolicy p = w->pricePolicy;
        if (invalid) p.maxQuoteAgeMs = 0;
        else if (!repeated) p.maxQuoteAgeMs = 1234;
        return UmiTradingWorkspaceSetRiskPricePolicy(w, &p);
    }
    case RESET:
        return UmiTradingWorkspaceResetDraft(invalid ? NULL : w);
    }
    return UMI_STATUS_INVALID_ARGUMENT;
}
static int SameEvidence(const UmiPretradeRiskEvidence *a, const UmiPretradeRiskEvidence *b)
{
    return a->decision.allowed == b->decision.allowed &&
        !strcmp(a->decision.reason, b->decision.reason) &&
        a->priceSource == b->priceSource && a->referencePrice == b->referencePrice &&
        a->notional == b->notional && a->projectedPosition == b->projectedPosition &&
        a->quoteTimeMs == b->quoteTimeMs && a->evaluatedAtMs == b->evaluatedAtMs &&
        a->quoteAgeMs == b->quoteAgeMs && a->hasValuation == b->hasValuation;
}

int main(int argc, char **argv)
{
    const char *operations[] = {"side", "type", "quantity", "prices", "policy", "reset"};
    const char *kinds[] = {"ordinary", "last", "exhausted", "repeated-exhausted", "invalid-exhausted"};
    if (argc != 2) return 2;
    size_t op = 6U, kind = 5U;
    for (size_t i = 0U; i < 6U; ++i) {
        size_t n = strlen(operations[i]);
        if (!strncmp(argv[1], operations[i], n) && argv[1][n] == '-') {
            op = i;
            for (size_t k = 0U; k < 5U; ++k) if (!strcmp(argv[1] + n + 1U, kinds[k])) kind = k;
            break;
        }
    }
    if (op == 6U || kind == 5U) return 2;
    int result = 0;
    UmiTradingWorkspace *w = NULL, *before = NULL;
    UmiTradingWorkspaceConfig config = umi_trading_workspace_config_default();
    config.risk_limit = (UmiRiskLimit){1000.0, 1.0e12, 5000.0, 1.0e12};
    UmiInstrument instrument = test_instrument();
    UmiRiskDecision decision;
    UmiPretradeRiskEvidence evidence, previousEvidence;
    DRAFT_CHECK(umi_trading_workspace_create(&config, &w) == UMI_STATUS_OK);
    DRAFT_CHECK(umi_trading_workspace_add_instrument(w, &instrument) == UMI_STATUS_OK);
    DRAFT_CHECK(umi_trading_workspace_set_health(w, 1, 0, 1) == UMI_STATUS_OK);
    DRAFT_CHECK(umi_trading_workspace_set_draft_quantity(w, 2.0) == UMI_STATUS_OK);
    DRAFT_CHECK(umi_trading_workspace_set_draft_prices(w, 100.0, 5.0) == UMI_STATUS_OK);
    DRAFT_CHECK(UmiTradingWorkspacePreviewOrderAt(w, 1000, &decision) == UMI_STATUS_OK && decision.allowed);
    DRAFT_CHECK(UmiTradingWorkspaceRiskEvidence(w, &previousEvidence) == UMI_STATUS_OK);
    w->revision = kind == 1U ? UINT64_MAX - 1U : kind >= 2U ? UINT64_MAX : 42U;
    before = malloc(sizeof(*before));
    DRAFT_CHECK(before != NULL); memcpy(before, w, sizeof(*before));
    UmiStatus expected = kind == 4U ? UMI_STATUS_INVALID_ARGUMENT :
        kind >= 2U ? UMI_STATUS_CAPACITY_EXCEEDED : UMI_STATUS_OK;
    DRAFT_CHECK(ApplyDraft(w, (DraftOperation)op, kind == 3U, kind == 4U) == expected);
    if (expected != UMI_STATUS_OK) {
        DRAFT_CHECK(!memcmp(w, before, sizeof(*w)));
        DRAFT_CHECK(UmiTradingWorkspaceRiskEvidence(w, &evidence) == UMI_STATUS_OK);
        DRAFT_CHECK(SameEvidence(&evidence, &previousEvidence));
    } else {
        DRAFT_CHECK(w->revision == before->revision + 1U && !w->has_draft_risk);
        memset(&evidence, 0xa5, sizeof(evidence));
        UmiPretradeRiskEvidence untouched; memcpy(&untouched, &evidence, sizeof(evidence));
        DRAFT_CHECK(UmiTradingWorkspaceRiskEvidence(w, &evidence) == UMI_STATUS_NOT_FOUND);
        DRAFT_CHECK(!memcmp(&evidence, &untouched, sizeof(evidence)));
        DRAFT_CHECK(!memcmp(&w->oms, &before->oms, sizeof(w->oms)));
        DRAFT_CHECK(!memcmp(&w->positions, &before->positions, sizeof(w->positions)));
        DRAFT_CHECK(!memcmp(&w->executions, &before->executions, sizeof(w->executions)));
        DRAFT_CHECK(w->next_order_sequence == before->next_order_sequence);
        if (op == SIDE) DRAFT_CHECK(w->draft_order.side == UMI_SIDE_SELL);
        if (op == TYPE) DRAFT_CHECK(w->draft_order.type == UMI_ORDER_MARKET &&
            w->draft_order.tif == UMI_TIF_GTC && w->draft_order.stop_price == 0.0);
        if (op == QUANTITY) DRAFT_CHECK(w->draft_order.quantity == 7.0);
        if (op == PRICES) DRAFT_CHECK(w->draft_order.limit_price == 90.0 && w->draft_order.stop_price == 95.0);
        if (op == POLICY) DRAFT_CHECK(w->pricePolicy.maxQuoteAgeMs == 1234);
        if (op == RESET) DRAFT_CHECK(w->draft_order.quantity == 1.0 &&
            w->draft_order.side == UMI_SIDE_BUY && w->draft_order.type == UMI_ORDER_LIMIT &&
            w->draft_order.tif == UMI_TIF_DAY && w->draft_order.stop_price == 0.0 &&
            !strcmp(w->draft_order.instrument.instrument_id.value, instrument.instrument_id.value));
        if (kind == 1U) {
            memcpy(before, w, sizeof(*before));
            DRAFT_CHECK(ApplyDraft(w, (DraftOperation)op, 1, 0) == UMI_STATUS_CAPACITY_EXCEEDED);
            DRAFT_CHECK(!memcmp(before, w, sizeof(*w)));
        }
    }
done:
    free(before);
    if (w != NULL) umi_trading_workspace_destroy(w);
    return result;
}
