/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/context_channel/test_04_trade.c
 *
 * PURPOSE:
 *   Exercise trade boundaries and stable status behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../value_archive/revision_cases.h"
#include <assert.h>
#include <string.h>
#include "umicom/context_channel/trade.h"
#include "umicom/context_channel/types.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char destination[32];
    assert(umi_context_copy_text(destination,sizeof(destination),"context") == UMI_STATUS_OK);
    assert(strcmp(destination,"context") == 0);
    assert(umi_context_text_is_valid(destination,sizeof(destination)));
    assert(umi_context_kind_text(UMI_CONTEXT_KIND_INSTRUMENT) != NULL);
    assert(umi_context_colour_text(UMI_CONTEXT_COLOUR_BLUE) != NULL);

    /* Exercise the context owner itself, including its revision publication. */
    UmiTradeContext context;
    umi_trade_context_init(&context);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiTradeContext, &context, revision,
        umi_trade_context_set_trade_id(&context, "trade-1"));
    assert(umi_trade_context_set_trade_id(&context, "trade-1") == UMI_STATUS_OK);
    assert(strcmp(context.trade_id, "trade-1") == 0);
    assert(umi_trade_context_set_trade_id(NULL, "trade-1") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiTradeContext, &context, revision,
        umi_trade_context_set_source_system(&context, "simulator"));
    assert(umi_trade_context_set_source_system(&context, "simulator") == UMI_STATUS_OK);
    assert(strcmp(context.source_system, "simulator") == 0);
    assert(umi_trade_context_set_source_system(NULL, "simulator") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiTradeContext, &context, revision,
        umi_trade_context_set_product_type(&context, "equity"));
    assert(umi_trade_context_set_product_type(&context, "equity") == UMI_STATUS_OK);
    assert(strcmp(context.product_type, "equity") == 0);
    assert(umi_trade_context_set_product_type(NULL, "equity") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiTradeContext, &context, revision,
        umi_trade_context_set_book_id(&context, "book-1"));
    assert(umi_trade_context_set_book_id(&context, "book-1") == UMI_STATUS_OK);
    assert(strcmp(context.book_id, "book-1") == 0);
    assert(umi_trade_context_set_book_id(NULL, "book-1") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiTradeContext, &context, revision,
        umi_trade_context_set_counterparty_id(&context, "synthetic"));
    assert(umi_trade_context_set_counterparty_id(&context, "synthetic") == UMI_STATUS_OK);
    assert(strcmp(context.counterparty_id, "synthetic") == 0);
    assert(umi_trade_context_set_counterparty_id(NULL, "synthetic") == UMI_STATUS_INVALID_ARGUMENT);
    UMI_TEST_VALUE_COUNTER_BOUNDARY(UmiTradeContext, &context, revision,
        umi_trade_context_set_version(&context, 2U));
    assert(umi_trade_context_set_version(&context, 2U) == UMI_STATUS_OK);
    assert(context.version == 2U);
    assert(umi_trade_context_set_version(NULL, 2U) == UMI_STATUS_INVALID_ARGUMENT);
    assert(context.revision == 7U);
    assert(umi_trade_context_validate(&context) == UMI_STATUS_OK);
    return 0;
}
