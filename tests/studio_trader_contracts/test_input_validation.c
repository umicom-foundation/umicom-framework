/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/studio_trader_contracts/test_input_validation.c
 *
 * PURPOSE:
 *   Regress shared editor, learning, financial and trading input contracts
 *   against the real production functions. No application-local validation,
 *   substitute data types, broker connections or live-order actions are used.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/document.h"
#include "umicom/finance/identifier.h"
#include "umicom/teacher/learning_plan.h"
#include "umicom/trading/instrument.h"
#include "umicom/trading/order_request.h"
#include "umicom/trading/pretrade_risk.h"
#include "umicom/trading/quote.h"
#include "umicom/trading/risk_limit.h"

#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Do not use assert: these checks must execute in Release/NDEBUG builds too. */
#define CHECK(condition) do { \
    if (!(condition)) { \
        (void)fprintf(stderr, "%s:%d: CHECK failed: %s\n", \
                      __FILE__, __LINE__, #condition); \
        return 1; \
    } \
} while (0)
#define ITEM_COUNT(items) (sizeof(items) / sizeof((items)[0]))

static UmiInstrument MakeInstrument(void)
{
    UmiInstrument instrument = {
        .instrument_id = {"regression.instrument"},
        .symbol = "TEST", .venue = "SIM", .currency = {"USD"},
        .multiplier = 1.0, .expiry_yyyymmdd = 0
    };
    return instrument;
}

static UmiOrderRequest MakeOrder(void)
{
    UmiOrderRequest request = {
        .client_order_id = {"regression.order"},
        .account_id = {"regression.account"},
        .side = UMI_SIDE_BUY, .type = UMI_ORDER_LIMIT, .tif = UMI_TIF_DAY,
        .quantity = 2.0, .limit_price = 100.0, .stop_price = 101.0,
        .environment = UMI_TRADING_SIMULATION
    };
    request.instrument = MakeInstrument();
    return request;
}

static UmiQuote MakeQuote(void)
{
    UmiQuote quote = {.bid = 100.0, .ask = 102.0,
                     .bid_size = 0.0, .ask_size = 2.0};
    quote.instrument = MakeInstrument();
    return quote;
}

static UmiRiskLimit MakeLimits(void)
{
    UmiRiskLimit limit = {10.0, 10000.0, 20.0, 1000.0};
    return limit;
}

static UmiEditorDocumentSnapshot MakeDocument(void)
{
    UmiEditorDocumentSnapshot item;
    memset(&item, 0, sizeof(item));
    memcpy(item.id, "document.one", sizeof("document.one"));
    memcpy(item.uri, "file:///test.c", sizeof("file:///test.c"));
    memcpy(item.language_id, "c", sizeof("c"));
    memcpy(item.title, "test.c", sizeof("test.c"));
    item.version = 3U;
    item.dirty = 1;
    return item;
}

/* Test every fixed-size field separately, including its last legal terminator. */
static void MalformDocumentField(UmiEditorDocumentSnapshot *item, size_t index)
{
    switch (index) {
        case 0U: memset(item->id, 'X', sizeof(item->id)); break;
        case 1U: memset(item->uri, 'X', sizeof(item->uri)); break;
        case 2U: memset(item->language_id, 'X', sizeof(item->language_id)); break;
        default: memset(item->title, 'X', sizeof(item->title)); break;
    }
}

static int EditorMetadataInsert(void)
{
    UmiEditorDocumentRegistry *registry = NULL;
    UmiEditorDocumentSnapshot item;
    size_t index;
    CHECK(umi_editor_document_registry_create(&registry) == UMI_STATUS_OK);
    CHECK(umi_editor_document_registry_upsert(registry, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    for (index = 0U; index < 4U; ++index) {
        item = MakeDocument();
        MalformDocumentField(&item, index);
        CHECK(umi_editor_document_registry_upsert(registry, &item) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_editor_document_registry_count(registry) == 0U);
        CHECK(umi_editor_document_registry_revision(registry) == 1U);
    }
    item = MakeDocument();
    item.id[0] = '\0';
    CHECK(umi_editor_document_registry_upsert(registry, &item) == UMI_STATUS_INVALID_ARGUMENT);
    item = MakeDocument();
    /* Preserve optional empty metadata: only the identifier must be non-empty. */
    item.uri[0] = item.language_id[0] = item.title[0] = '\0';
    CHECK(umi_editor_document_registry_upsert(registry, &item) == UMI_STATUS_OK);
    CHECK(umi_editor_document_registry_remove(registry, item.id) == UMI_STATUS_OK);
    memset(item.id, 'I', sizeof(item.id)); item.id[sizeof(item.id) - 1U] = '\0';
    memset(item.uri, 'U', sizeof(item.uri)); item.uri[sizeof(item.uri) - 1U] = '\0';
    memset(item.language_id, 'L', sizeof(item.language_id)); item.language_id[sizeof(item.language_id) - 1U] = '\0';
    memset(item.title, 'T', sizeof(item.title)); item.title[sizeof(item.title) - 1U] = '\0';
    CHECK(umi_editor_document_registry_upsert(registry, &item) == UMI_STATUS_OK);
    CHECK(umi_editor_document_registry_count(registry) == 1U);
    umi_editor_document_registry_destroy(registry);
    return 0;
}

static int EditorRejectedReplacement(void)
{
    UmiEditorDocumentRegistry *registry = NULL;
    UmiEditorDocumentSnapshot item = MakeDocument();
    UmiEditorDocumentSnapshot before, after;
    uint64_t revision;
    size_t index;
    memset(&before, 0, sizeof(before));
    memset(&after, 0, sizeof(after));
    CHECK(umi_editor_document_registry_create(&registry) == UMI_STATUS_OK);
    CHECK(umi_editor_document_registry_upsert(registry, &item) == UMI_STATUS_OK);
    CHECK(umi_editor_document_registry_find(registry, item.id, &before) == UMI_STATUS_OK);
    revision = umi_editor_document_registry_revision(registry);
    for (index = 1U; index < 4U; ++index) {
        item = MakeDocument();
        item.version = 999U;
        item.dirty = 0;
        MalformDocumentField(&item, index);
        CHECK(umi_editor_document_registry_upsert(registry, &item) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(umi_editor_document_registry_count(registry) == 1U);
        CHECK(umi_editor_document_registry_revision(registry) == revision);
        CHECK(umi_editor_document_registry_find(registry, before.id, &after) == UMI_STATUS_OK);
        CHECK(memcmp(&before, &after, sizeof(before)) == 0);
    }
    item = MakeDocument(); item.version = 4U;
    CHECK(umi_editor_document_registry_upsert(registry, &item) == UMI_STATUS_OK);
    CHECK(umi_editor_document_registry_revision(registry) == revision + 1U);
    CHECK(umi_editor_document_registry_at(registry, 0U, &after) == UMI_STATUS_OK);
    CHECK(after.version == 4U && after.revision == revision + 1U);
    CHECK(after.struct_size == sizeof(after) && after.api_version == 1U);
    umi_editor_document_registry_destroy(registry);
    return 0;
}

static int FinancialIdentifierBounds(void)
{
    UmiFinancialId left = {"ABC"}, right = {"ABC"}, empty = {{0}};
    UmiFinancialId *unterminated;
    CHECK(umi_financial_id_valid(&left));
    CHECK(umi_financial_id_equal(&left, &right));
    right.value[0] = 'D';
    CHECK(!umi_financial_id_equal(&left, &right));
    CHECK(!umi_financial_id_valid(NULL));
    CHECK(!umi_financial_id_valid(&empty));
    CHECK(!umi_financial_id_equal(NULL, &left));
    memset(left.value, 'A', sizeof(left.value));
    left.value[sizeof(left.value) - 1U] = '\0';
    CHECK(umi_financial_id_valid(&left));
    /* Exact heap object lets AddressSanitizer catch reads beyond its capacity. */
    unterminated = malloc(sizeof(*unterminated));
    CHECK(unterminated != NULL);
    memset(unterminated->value, 'A', sizeof(unterminated->value));
    CHECK(!umi_financial_id_valid(unterminated));
    CHECK(!umi_financial_id_equal(unterminated, unterminated));
    CHECK(!umi_financial_id_equal(&left, unterminated));
    free(unterminated);
    CHECK(!umi_financial_id_equal(&empty, &empty));
    return 0;
}

static int InstrumentValidation(void)
{
    const double invalid[] = {NAN, INFINITY, -INFINITY, 0.0, -1.0};
    UmiInstrument instrument = MakeInstrument(), other = instrument;
    size_t index;
    CHECK(umi_instrument_valid(&instrument));
    CHECK(umi_instrument_same(&instrument, &other));
    CHECK(!umi_instrument_valid(NULL));
    CHECK(!umi_instrument_same(NULL, &other));
    for (index = 0U; index < ITEM_COUNT(invalid); ++index) {
        instrument = MakeInstrument(); instrument.multiplier = invalid[index];
        CHECK(!umi_instrument_valid(&instrument));
    }
    instrument = MakeInstrument();
    memset(instrument.symbol, 'S', sizeof(instrument.symbol));
    CHECK(!umi_instrument_valid(&instrument));
    instrument.symbol[sizeof(instrument.symbol) - 1U] = '\0';
    CHECK(umi_instrument_valid(&instrument));
    memset(instrument.venue, 'V', sizeof(instrument.venue));
    CHECK(!umi_instrument_valid(&instrument));
    instrument.venue[sizeof(instrument.venue) - 1U] = '\0';
    CHECK(umi_instrument_valid(&instrument));
    instrument.currency.code[3] = 'X';
    CHECK(!umi_instrument_valid(&instrument));
    instrument = MakeInstrument(); instrument.symbol[0] = '\0';
    CHECK(!umi_instrument_valid(&instrument));
    instrument = MakeInstrument(); instrument.venue[0] = '\0';
    CHECK(!umi_instrument_valid(&instrument));
    instrument = MakeInstrument();
    memset(instrument.instrument_id.value, 'I', sizeof(instrument.instrument_id.value));
    CHECK(!umi_instrument_valid(&instrument));
    CHECK(!umi_instrument_same(&instrument, &instrument));
    /* Identity remains ID-only: no new equality rule for other metadata. */
    instrument = MakeInstrument(); other.multiplier = 2.0;
    CHECK(umi_instrument_same(&instrument, &other));
    return 0;
}

static int LearningIdentifierBounds(void)
{
    UmiTeacherLearningPlan sequence, before;
    char id[UMI_TEACHER_ID_CAPACITY + 1U];
    size_t index;
    umi_teacher_learning_plan_init(&sequence);
    CHECK(umi_teacher_learning_plan_append(&sequence, "first") == UMI_STATUS_OK);
    memcpy(&before, &sequence, sizeof(before));
    memset(id, 'A', sizeof(id)); id[sizeof(id) - 1U] = '\0';
    CHECK(umi_teacher_learning_plan_append(&sequence, id) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&before, &sequence, sizeof(before)) == 0);
    CHECK(umi_teacher_learning_plan_append(&sequence, "") == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_teacher_learning_plan_append(&sequence, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(memcmp(&before, &sequence, sizeof(before)) == 0);
    id[UMI_TEACHER_ID_CAPACITY - 1U] = '\0';
    CHECK(umi_teacher_learning_plan_append(&sequence, id) == UMI_STATUS_OK);
    CHECK(strcmp(sequence.ids[1], id) == 0);
    CHECK(sequence.revision == 2U);
    memcpy(&before, &sequence, sizeof(before));
    CHECK(umi_teacher_learning_plan_append(&sequence, id) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(memcmp(&before, &sequence, sizeof(before)) == 0);
    /* Fill the real capacity and preserve duplicate status even when full. */
    for (index = sequence.count; index < UMI_TEACHER_MEDIUM_CAPACITY; ++index) {
        (void)snprintf(id, sizeof(id), "lesson.%zu", index);
        CHECK(umi_teacher_learning_plan_append(&sequence, id) == UMI_STATUS_OK);
    }
    memcpy(&before, &sequence, sizeof(before));
    CHECK(umi_teacher_learning_plan_append(&sequence, "first") == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_teacher_learning_plan_append(&sequence, "overflow") == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(memcmp(&before, &sequence, sizeof(before)) == 0);
    return 0;
}

static int LearningPlanIntegrity(void)
{
    UmiTeacherLearningPlan sequence, before;
    const size_t badCounts[] = {UMI_TEACHER_MEDIUM_CAPACITY + 1U, SIZE_MAX};
    size_t index;
    for (index = 0U; index < ITEM_COUNT(badCounts); ++index) {
        umi_teacher_learning_plan_init(&sequence);
        sequence.count = badCounts[index];
        memcpy(&before, &sequence, sizeof(before));
        CHECK(umi_teacher_learning_plan_current(&sequence) == NULL);
        CHECK(umi_teacher_learning_plan_count(&sequence) == 0U);
        CHECK(umi_teacher_learning_plan_append(&sequence, "new") == UMI_STATUS_INVALID_STATE);
        CHECK(umi_teacher_learning_plan_next(&sequence) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_teacher_learning_plan_previous(&sequence) == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(&before, &sequence, sizeof(before)) == 0);
    }
    umi_teacher_learning_plan_init(&sequence);
    CHECK(umi_teacher_learning_plan_append(&sequence, "first") == UMI_STATUS_OK);
    memset(sequence.ids[0], 'X', sizeof(sequence.ids[0]));
    memcpy(&before, &sequence, sizeof(before));
    CHECK(umi_teacher_learning_plan_current(&sequence) == NULL);
    CHECK(umi_teacher_learning_plan_append(&sequence, "second") == UMI_STATUS_INVALID_STATE);
    CHECK(umi_teacher_learning_plan_next(&sequence) == UMI_STATUS_INVALID_STATE);
    CHECK(memcmp(&before, &sequence, sizeof(before)) == 0);
    sequence.ids[0][0] = '\0';
    CHECK(umi_teacher_learning_plan_current(&sequence) == NULL);
    CHECK(umi_teacher_learning_plan_append(&sequence, "second") == UMI_STATUS_INVALID_STATE);
    return 0;
}

static int LearningNavigation(void)
{
    UmiTeacherLearningPlan sequence, before;
    const size_t badCursors[] = {2U, UMI_TEACHER_MEDIUM_CAPACITY, SIZE_MAX};
    size_t index;
    umi_teacher_learning_plan_init(&sequence);
    CHECK(umi_teacher_learning_plan_current(&sequence) == NULL);
    CHECK(umi_teacher_learning_plan_next(&sequence) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_teacher_learning_plan_previous(&sequence) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_teacher_learning_plan_append(&sequence, "first") == UMI_STATUS_OK);
    CHECK(umi_teacher_learning_plan_append(&sequence, "second") == UMI_STATUS_OK);
    CHECK(strcmp(umi_teacher_learning_plan_current(&sequence), "first") == 0);
    CHECK(umi_teacher_learning_plan_next(&sequence) == UMI_STATUS_OK);
    CHECK(strcmp(umi_teacher_learning_plan_current(&sequence), "second") == 0);
    CHECK(umi_teacher_learning_plan_next(&sequence) == UMI_STATUS_NOT_FOUND);
    CHECK(umi_teacher_learning_plan_previous(&sequence) == UMI_STATUS_OK);
    CHECK(sequence.revision == 2U);
    for (index = 0U; index < ITEM_COUNT(badCursors); ++index) {
        sequence.cursor = badCursors[index];
        memcpy(&before, &sequence, sizeof(before));
        CHECK(umi_teacher_learning_plan_current(&sequence) == NULL);
        CHECK(umi_teacher_learning_plan_next(&sequence) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_teacher_learning_plan_previous(&sequence) == UMI_STATUS_INVALID_STATE);
        CHECK(umi_teacher_learning_plan_append(&sequence, "third") == UMI_STATUS_INVALID_STATE);
        CHECK(memcmp(&before, &sequence, sizeof(before)) == 0);
    }
    umi_teacher_learning_plan_init(&sequence); sequence.cursor = SIZE_MAX;
    CHECK(umi_teacher_learning_plan_next(&sequence) == UMI_STATUS_INVALID_STATE);
    CHECK(umi_teacher_learning_plan_next(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_teacher_learning_plan_previous(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_teacher_learning_plan_count(NULL) == 0U);
    return 0;
}

static int OrderNumericFields(void)
{
    const double badNumbers[] = {NAN, INFINITY, -INFINITY};
    const UmiOrderType types[] = {UMI_ORDER_MARKET, UMI_ORDER_LIMIT, UMI_ORDER_STOP, UMI_ORDER_STOP_LIMIT};
    size_t typeIndex, numberIndex, fieldIndex;
    for (typeIndex = 0U; typeIndex < ITEM_COUNT(types); ++typeIndex) {
        for (numberIndex = 0U; numberIndex < ITEM_COUNT(badNumbers); ++numberIndex) {
            for (fieldIndex = 0U; fieldIndex < 3U; ++fieldIndex) {
                UmiOrderRequest request = MakeOrder(), before;
                double *fields[] = {&request.quantity, &request.limit_price, &request.stop_price};
                request.type = types[typeIndex];
                *fields[fieldIndex] = badNumbers[numberIndex];
                memcpy(&before, &request, sizeof(before));
                CHECK(umi_order_request_validate(&request) == UMI_STATUS_INVALID_ARGUMENT);
                CHECK(memcmp(&before, &request, sizeof(before)) == 0);
            }
        }
    }
    {
        UmiOrderRequest request = MakeOrder();
        request.quantity = 0.0;
        CHECK(umi_order_request_validate(&request) == UMI_STATUS_INVALID_ARGUMENT);
        request.quantity = -1.0;
        CHECK(umi_order_request_validate(&request) == UMI_STATUS_INVALID_ARGUMENT);
    }
    return 0;
}

static int OrderEnumerations(void)
{
    const UmiSide sides[] = {UMI_SIDE_BUY, UMI_SIDE_SELL};
    const UmiOrderType types[] = {UMI_ORDER_MARKET, UMI_ORDER_LIMIT, UMI_ORDER_STOP, UMI_ORDER_STOP_LIMIT};
    const UmiTimeInForce tifs[] = {UMI_TIF_DAY, UMI_TIF_GTC, UMI_TIF_IOC, UMI_TIF_FOK};
    const UmiTradingEnvironment environments[] = {UMI_TRADING_SIMULATION, UMI_TRADING_PAPER, UMI_TRADING_LIVE};
    const int badValues[] = {-100, 100, 999};
    size_t s, t, f, e, index;
    for (s = 0U; s < ITEM_COUNT(sides); ++s)
        for (t = 0U; t < ITEM_COUNT(types); ++t)
            for (f = 0U; f < ITEM_COUNT(tifs); ++f)
                for (e = 0U; e < ITEM_COUNT(environments); ++e) {
                    UmiOrderRequest request = MakeOrder();
                    request.side = sides[s]; request.type = types[t];
                    request.tif = tifs[f]; request.environment = environments[e];
                    CHECK(umi_order_request_validate(&request) == UMI_STATUS_OK);
                }
    for (index = 0U; index < ITEM_COUNT(badValues); ++index) {
        UmiOrderRequest request = MakeOrder();
        request.side = (UmiSide)badValues[index];
        CHECK(umi_order_request_validate(&request) == UMI_STATUS_INVALID_ARGUMENT);
        request = MakeOrder(); request.type = (UmiOrderType)badValues[index];
        CHECK(umi_order_request_validate(&request) == UMI_STATUS_INVALID_ARGUMENT);
        request = MakeOrder(); request.tif = (UmiTimeInForce)badValues[index];
        CHECK(umi_order_request_validate(&request) == UMI_STATUS_INVALID_ARGUMENT);
        request = MakeOrder(); request.environment = (UmiTradingEnvironment)badValues[index];
        CHECK(umi_order_request_validate(&request) == UMI_STATUS_INVALID_ARGUMENT);
    }
    {
        UmiOrderRequest request = MakeOrder(); request.side = (UmiSide)0;
        CHECK(umi_order_request_validate(&request) == UMI_STATUS_INVALID_ARGUMENT);
    }
    return 0;
}

static int OrderPriceRequirements(void)
{
    const UmiOrderType types[] = {UMI_ORDER_MARKET, UMI_ORDER_LIMIT, UMI_ORDER_STOP, UMI_ORDER_STOP_LIMIT};
    size_t index;
    CHECK(umi_order_request_validate(NULL) == UMI_STATUS_INVALID_ARGUMENT);
    for (index = 0U; index < ITEM_COUNT(types); ++index) {
        UmiOrderRequest request = MakeOrder();
        const int needsLimit = types[index] == UMI_ORDER_LIMIT || types[index] == UMI_ORDER_STOP_LIMIT;
        const int needsStop = types[index] == UMI_ORDER_STOP || types[index] == UMI_ORDER_STOP_LIMIT;
        request.type = types[index];
        request.limit_price = needsLimit ? 1.0 : 0.0;
        request.stop_price = needsStop ? 1.0 : 0.0;
        CHECK(umi_order_request_validate(&request) == UMI_STATUS_OK);
        request.limit_price = 0.0;
        CHECK(umi_order_request_validate(&request) == (needsLimit ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK));
        request.limit_price = needsLimit ? 1.0 : -1.0;
        request.stop_price = -1.0;
        CHECK(umi_order_request_validate(&request) == (needsStop ? UMI_STATUS_INVALID_ARGUMENT : UMI_STATUS_OK));
    }
    {
        UmiOrderRequest request = MakeOrder(); request.account_id.value[0] = '\0';
        CHECK(umi_order_request_validate(&request) == UMI_STATUS_INVALID_ARGUMENT);
        request = MakeOrder(); request.client_order_id.value[0] = '\0';
        CHECK(umi_order_request_validate(&request) == UMI_STATUS_INVALID_ARGUMENT);
    }
    return 0;
}

static int QuoteNumericFields(void)
{
    const double invalid[] = {NAN, INFINITY, -INFINITY};
    size_t numberIndex, fieldIndex;
    CHECK(!umi_quote_valid(NULL));
    CHECK(umi_quote_mid(NULL) == 0.0 && umi_quote_spread(NULL) == 0.0);
    for (numberIndex = 0U; numberIndex < ITEM_COUNT(invalid); ++numberIndex) {
        for (fieldIndex = 0U; fieldIndex < 4U; ++fieldIndex) {
            UmiQuote quote = MakeQuote();
            double *fields[] = {&quote.bid, &quote.ask, &quote.bid_size, &quote.ask_size};
            *fields[fieldIndex] = invalid[numberIndex];
            CHECK(!umi_quote_valid(&quote));
            CHECK(umi_quote_mid(&quote) == 0.0 && umi_quote_spread(&quote) == 0.0);
        }
    }
    {
        UmiQuote quote = MakeQuote();
        CHECK(umi_quote_valid(&quote));
        CHECK(umi_quote_mid(&quote) == 101.0 && umi_quote_spread(&quote) == 2.0);
        quote.ask = 99.0; CHECK(!umi_quote_valid(&quote));
        quote = MakeQuote(); quote.bid = 0.0; CHECK(!umi_quote_valid(&quote));
        quote = MakeQuote(); quote.bid_size = -1.0; CHECK(!umi_quote_valid(&quote));
        quote = MakeQuote(); quote.ask_size = -1.0; CHECK(!umi_quote_valid(&quote));
    }
    return 0;
}

static int QuoteArithmetic(void)
{
    const double endpoints[][2] = {
        {DBL_MAX, DBL_MAX}, {DBL_MAX * 0.75, DBL_MAX},
        {DBL_MIN, DBL_MAX}, {DBL_MIN, DBL_MIN},
        {DBL_TRUE_MIN, DBL_TRUE_MIN}, {100.0, 102.0}
    };
    size_t index;
    for (index = 0U; index < ITEM_COUNT(endpoints); ++index) {
        UmiQuote quote = MakeQuote();
        double midpoint, spread;
        quote.bid = endpoints[index][0]; quote.ask = endpoints[index][1];
        CHECK(umi_quote_valid(&quote));
        midpoint = umi_quote_mid(&quote); spread = umi_quote_spread(&quote);
        CHECK(isfinite(midpoint) && midpoint >= quote.bid && midpoint <= quote.ask);
        CHECK(isfinite(spread) && spread >= 0.0);
        if (quote.bid == quote.ask) CHECK(midpoint == quote.bid && spread == 0.0);
    }
    return 0;
}

static int RiskInputValidation(void)
{
    const double invalid[] = {NAN, INFINITY, -INFINITY};
    UmiRiskLimit limit = MakeLimits();
    UmiOrderRequest request = MakeOrder();
    size_t index, fieldIndex;
    CHECK(umi_pretrade_risk_evaluate(&request, &limit, 0.0, 0.0).allowed);
    CHECK(!umi_pretrade_risk_evaluate(NULL, &limit, 0.0, 0.0).allowed);
    CHECK(!umi_pretrade_risk_evaluate(&request, NULL, 0.0, 0.0).allowed);
    for (index = 0U; index < ITEM_COUNT(invalid); ++index) {
        CHECK(!umi_pretrade_risk_evaluate(&request, &limit, invalid[index], 0.0).allowed);
        CHECK(!umi_pretrade_risk_evaluate(&request, &limit, 0.0, invalid[index]).allowed);
        for (fieldIndex = 0U; fieldIndex < 4U; ++fieldIndex) {
            UmiRiskLimit broken = MakeLimits();
            double *fields[] = {&broken.max_order_quantity, &broken.max_order_notional,
                               &broken.max_position_quantity, &broken.max_daily_loss};
            *fields[fieldIndex] = invalid[index];
            CHECK(!umi_risk_limit_valid(&broken));
            CHECK(!umi_pretrade_risk_evaluate(&request, &broken, 0.0, 0.0).allowed);
        }
    }
    request.side = (UmiSide)0;
    CHECK(!umi_pretrade_risk_evaluate(&request, &limit, 0.0, 0.0).allowed);
    request = MakeOrder(); request.quantity = NAN;
    CHECK(!umi_pretrade_risk_evaluate(&request, &limit, 0.0, 0.0).allowed);
    request = MakeOrder(); request.instrument.multiplier = INFINITY;
    CHECK(!umi_pretrade_risk_evaluate(&request, &limit, 0.0, 0.0).allowed);
    request = MakeOrder(); request.client_order_id.value[0] = '\0';
    CHECK(!umi_pretrade_risk_evaluate(&request, &limit, 0.0, 0.0).allowed);
    limit = MakeLimits(); limit.max_daily_loss = 0.0;
    CHECK(umi_risk_limit_valid(&limit));
    return 0;
}

static int RiskArithmetic(void)
{
    UmiOrderRequest request = MakeOrder();
    UmiRiskLimit limit = {DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX};
    UmiRiskDecision decision;
    request.quantity = 2.0; request.limit_price = DBL_MAX;
    decision = umi_pretrade_risk_evaluate(&request, &limit, 0.0, 0.0);
    CHECK(!decision.allowed);
    CHECK(strcmp(decision.reason, "non-finite risk calculation") == 0);
    request = MakeOrder(); request.quantity = DBL_MAX;
    request.type = UMI_ORDER_MARKET; request.limit_price = 0.0;
    decision = umi_pretrade_risk_evaluate(&request, &limit, DBL_MAX, 0.0);
    CHECK(!decision.allowed);
    CHECK(strcmp(decision.reason, "non-finite risk calculation") == 0);
    request.side = UMI_SIDE_SELL;
    CHECK(!umi_pretrade_risk_evaluate(&request, &limit, -DBL_MAX, 0.0).allowed);
    request = MakeOrder(); limit = MakeLimits();
    request.quantity = limit.max_order_quantity;
    CHECK(umi_pretrade_risk_evaluate(&request, &limit, 0.0, 0.0).allowed);
    request.quantity = limit.max_order_quantity + 1.0;
    CHECK(!umi_pretrade_risk_evaluate(&request, &limit, 0.0, 0.0).allowed);
    request = MakeOrder(); limit.max_order_notional = 199.0;
    CHECK(!umi_pretrade_risk_evaluate(&request, &limit, 0.0, 0.0).allowed);
    limit = MakeLimits();
    CHECK(!umi_pretrade_risk_evaluate(&request, &limit, 19.0, 0.0).allowed);
    CHECK(!umi_pretrade_risk_evaluate(&request, &limit, 0.0, -1001.0).allowed);
    CHECK(umi_pretrade_risk_evaluate(&request, &limit, 0.0, -1000.0).allowed);
    request.side = UMI_SIDE_SELL;
    CHECK(umi_pretrade_risk_evaluate(&request, &limit, 10.0, 0.0).allowed);
    /* Explicitly document, rather than silently change, the 1.0 fallback. */
    request = MakeOrder(); request.type = UMI_ORDER_MARKET; request.limit_price = 0.0;
    limit.max_order_notional = 2.0;
    CHECK(umi_pretrade_risk_evaluate(&request, &limit, 0.0, 0.0).allowed);
    limit.max_order_notional = 1.0;
    CHECK(!umi_pretrade_risk_evaluate(&request, &limit, 0.0, 0.0).allowed);
    return 0;
}

typedef int (*TestFunction)(void);
typedef struct TestCase { const char *name; TestFunction run; } TestCase;

static const TestCase TEST_CASES[] = {
    {"editor_metadata_insert", EditorMetadataInsert},
    {"editor_rejected_replacement", EditorRejectedReplacement},
    {"financial_identifier_bounds", FinancialIdentifierBounds},
    {"instrument_validation", InstrumentValidation},
    {"learning_identifier_bounds", LearningIdentifierBounds},
    {"learning_plan_integrity", LearningPlanIntegrity},
    {"learning_navigation", LearningNavigation},
    {"order_numeric_fields", OrderNumericFields},
    {"order_enumerations", OrderEnumerations},
    {"order_price_requirements", OrderPriceRequirements},
    {"quote_numeric_fields", QuoteNumericFields},
    {"quote_arithmetic", QuoteArithmetic},
    {"risk_input_validation", RiskInputValidation},
    {"risk_arithmetic", RiskArithmetic}
};

/* One named group per process makes a failing baseline independent of others. */
int main(int argc, char **argv)
{
    size_t index;
    if (argc != 2) {
        (void)fprintf(stderr, "Usage: %s <group|--list>\n", argv[0]);
        return 2;
    }
    if (strcmp(argv[1], "--list") == 0) {
        for (index = 0U; index < ITEM_COUNT(TEST_CASES); ++index)
            (void)puts(TEST_CASES[index].name);
        return 0;
    }
    for (index = 0U; index < ITEM_COUNT(TEST_CASES); ++index) {
        if (strcmp(argv[1], TEST_CASES[index].name) == 0) {
            const int result = TEST_CASES[index].run();
            (void)printf("%s: %s\n", TEST_CASES[index].name, result == 0 ? "PASS" : "FAIL");
            return result;
        }
    }
    (void)fprintf(stderr, "Unknown regression group: %s\n", argv[1]);
    return 2;
}
