/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/strategy_research/test_strategy_project.c
 *
 * PURPOSE:
 *   Verify the C23 strategy project template remains simulation-only and
 *   renders a compilable research-service entry point without order authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>

#include "umicom/strategy_research/strategy_project.h"

#include "../value_archive/transfer_cases.h"

#include "umicom/strategy_research/strategy_project.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiStrategyProjectConfigTransferEqual(const UmiStrategyProjectConfig *a, const UmiStrategyProjectConfig *b)
{
    return strcmp(a->projectName, b->projectName) == 0 &&
        strcmp(a->strategyName, b->strategyName) == 0 &&
        strcmp(a->instrument, b->instrument) == 0 &&
        strcmp(a->timeframe, b->timeframe) == 0 &&
        a->warmupBars == b->warmupBars &&
        a->simulationOnly == b->simulationOnly;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiStrategyProjectConfigTransferTails(UmiStrategyProjectConfig *value)
{
    (void)value;
    {
        size_t used = strlen(value->projectName) + 1U;
        memset(value->projectName + used, 0xa5, sizeof(value->projectName) - used);
    }
    {
        size_t used = strlen(value->strategyName) + 1U;
        memset(value->strategyName + used, 0xa5, sizeof(value->strategyName) - used);
    }
    {
        size_t used = strlen(value->instrument) + 1U;
        memset(value->instrument + used, 0xa5, sizeof(value->instrument) - used);
    }
    {
        size_t used = strlen(value->timeframe) + 1U;
        memset(value->timeframe + used, 0xa5, sizeof(value->timeframe) - used);
    }
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiStrategyProjectConfigTransferMalformed(const UmiStrategyProjectConfig *sample)
{
    (void)sample;
    {
        UmiStrategyProjectConfig invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.projectName, 'x', sizeof(invalid.projectName));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_strategy_project_config_validate(&invalid) != UMI_STATUS_OK) ||
            umi_strategy_project_config_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated projectName was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiStrategyProjectConfig invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.strategyName, 'x', sizeof(invalid.strategyName));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_strategy_project_config_validate(&invalid) != UMI_STATUS_OK) ||
            umi_strategy_project_config_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated strategyName was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiStrategyProjectConfig invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.instrument, 'x', sizeof(invalid.instrument));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_strategy_project_config_validate(&invalid) != UMI_STATUS_OK) ||
            umi_strategy_project_config_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated instrument was not refused before publication.\n");
            return 1;
        }
    }
    {
        UmiStrategyProjectConfig invalid = *sample;
        unsigned char bytes[48], expected[48];
        size_t written = 77U;
        memset(invalid.timeframe, 'x', sizeof(invalid.timeframe));
        memset(bytes, 0xa5, sizeof(bytes)); memcpy(expected, bytes, sizeof(bytes));
        if (!(umi_strategy_project_config_validate(&invalid) != UMI_STATUS_OK) ||
            umi_strategy_project_config_archive_encode(&invalid, bytes, sizeof(bytes), &written) == UMI_STATUS_OK ||
            written != 77U || memcmp(bytes, expected, sizeof(bytes)) != 0) {
            fprintf(stderr, "Unterminated timeframe was not refused before publication.\n");
            return 1;
        }
    }
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiStrategyProjectConfigTransferCases, UmiStrategyProjectConfig,
    umi_strategy_project_config_archive_encode, umi_strategy_project_config_archive_decode,
    UmiStrategyProjectConfigTransferEqual, UmiStrategyProjectConfigTransferTails, UmiStrategyProjectConfigTransferMalformed)

int main(void)
{
    UmiStrategyProjectConfig config;
    char source[2048];
    size_t required = 0U;

    umi_strategy_project_config_init(&config);
    assert(umi_strategy_project_config_validate(&config) == UMI_STATUS_OK);
    if (UmiStrategyProjectConfigTransferCases(&config) != 0) return 1;

    assert(config.simulationOnly);
    assert(umi_strategy_project_render_c23(
               &config, source, sizeof(source), &required) == UMI_STATUS_OK);
    assert(required > 0U);
    assert(strstr(source, "strategy.signal-score") != NULL);
    assert(strstr(source, "ResearchStrategyEvaluate") != NULL);
    assert(strstr(source, "submit") == NULL);
    assert(strstr(source, "broker") == NULL);

    config.simulationOnly = 0;
    assert(umi_strategy_project_config_validate(&config) ==
           UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
