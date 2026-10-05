/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/strategy_research/strategy_project.c
 *
 * PURPOSE:
 *   Render a C23 research strategy starting point without creating files,
 *   placing orders or granting broker authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/strategy_research/strategy_project.h"
#include "../base/value_archive_internal.h"

#include <stdio.h>
#include <string.h>

void umi_strategy_project_config_init(
    UmiStrategyProjectConfig *config)
{
    if (config == NULL) return;
    (void)memset(config, 0, sizeof(*config));
    (void)snprintf(config->projectName, sizeof(config->projectName),
                   "%s", "UmicomStrategy");
    (void)snprintf(config->strategyName, sizeof(config->strategyName),
                   "%s", "ResearchStrategy");
    (void)snprintf(config->instrument, sizeof(config->instrument),
                   "%s", "SIMULATION");
    (void)snprintf(config->timeframe, sizeof(config->timeframe),
                   "%s", "5m");
    config->warmupBars = 100U;
    config->simulationOnly = 1;
}

UmiStatus umi_strategy_project_config_validate(
    const UmiStrategyProjectConfig *config)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (config == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(config->projectName, '\0', sizeof(config->projectName)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(config->strategyName, '\0', sizeof(config->strategyName)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(config->instrument, '\0', sizeof(config->instrument)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(config->timeframe, '\0', sizeof(config->timeframe)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    if (config == NULL ||
        config->projectName[0] == '\0' ||
        config->strategyName[0] == '\0' ||
        config->instrument[0] == '\0' ||
        config->timeframe[0] == '\0' ||
        config->warmupBars == 0U ||
        !config->simulationOnly) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_strategy_project_render_c23(
    const UmiStrategyProjectConfig *config,
    char *outSource,
    size_t capacity,
    size_t *outRequired)
{
    int required;

    if (umi_strategy_project_config_validate(config) != UMI_STATUS_OK ||
        outRequired == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    required = snprintf(
        NULL, 0,
        "/* Generated Umicom research strategy: %s */\n"
        "#include <stddef.h>\n"
        "#include \"umicom/strategy_research/strategy_research.h\"\n\n"
        "/* Instrument: %s | Timeframe: %s | Warm-up bars: %zu */\n"
        "UmiStatus %sEvaluate(\n"
        "    const UmiStrategyResearchInput *input,\n"
        "    UmiStrategyResearchSnapshot *outSnapshot)\n"
        "{\n"
        "    /* Research-only: this function returns evidence, not an order. */\n"
        "    return umi_strategy_research_service_evaluate(\n"
        "        \"strategy.signal-score\", input, outSnapshot);\n"
        "}\n",
        config->projectName,
        config->instrument,
        config->timeframe,
        config->warmupBars,
        config->strategyName);

    if (required < 0) return UMI_STATUS_INTERNAL_ERROR;
    *outRequired = (size_t)required + 1U;

    if (outSource == NULL || capacity < *outRequired) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    (void)snprintf(
        outSource, capacity,
        "/* Generated Umicom research strategy: %s */\n"
        "#include <stddef.h>\n"
        "#include \"umicom/strategy_research/strategy_research.h\"\n\n"
        "/* Instrument: %s | Timeframe: %s | Warm-up bars: %zu */\n"
        "UmiStatus %sEvaluate(\n"
        "    const UmiStrategyResearchInput *input,\n"
        "    UmiStrategyResearchSnapshot *outSnapshot)\n"
        "{\n"
        "    /* Research-only: this function returns evidence, not an order. */\n"
        "    return umi_strategy_research_service_evaluate(\n"
        "        \"strategy.signal-score\", input, outSnapshot);\n"
        "}\n",
        config->projectName,
        config->instrument,
        config->timeframe,
        config->warmupBars,
        config->strategyName);
    return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiStrategyProjectConfigArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xbc9fec6d9a91d444);
    schema = (schema ^ (uint64_t)sizeof(((UmiStrategyProjectConfig *)0)->projectName)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiStrategyProjectConfig *)0)->strategyName)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiStrategyProjectConfig *)0)->instrument)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiStrategyProjectConfig *)0)->timeframe)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiStrategyProjectConfigArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiStrategyProjectConfig *)0)->projectName) - 1U +
        8U + sizeof(((UmiStrategyProjectConfig *)0)->strategyName) - 1U +
        8U + sizeof(((UmiStrategyProjectConfig *)0)->instrument) - 1U +
        8U + sizeof(((UmiStrategyProjectConfig *)0)->timeframe) - 1U +
        8U +
        8U;
}
static void UmiStrategyProjectConfigArchiveWrite(UmiArchiveWriter *writer, const UmiStrategyProjectConfig *value)
{
    UmiArchiveWriteText(writer, value->projectName, sizeof(value->projectName));
    UmiArchiveWriteText(writer, value->strategyName, sizeof(value->strategyName));
    UmiArchiveWriteText(writer, value->instrument, sizeof(value->instrument));
    UmiArchiveWriteText(writer, value->timeframe, sizeof(value->timeframe));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->warmupBars);
    UmiArchiveWriteSigned(writer, (int64_t)value->simulationOnly);
}
static void UmiStrategyProjectConfigArchiveRead(UmiArchiveReader *reader, UmiStrategyProjectConfig *value)
{
    UmiArchiveReadText(reader, value->projectName, sizeof(value->projectName));
    UmiArchiveReadText(reader, value->strategyName, sizeof(value->strategyName));
    UmiArchiveReadText(reader, value->instrument, sizeof(value->instrument));
    UmiArchiveReadText(reader, value->timeframe, sizeof(value->timeframe));
    value->warmupBars = (size_t)UmiArchiveReadUnsigned(reader, SIZE_MAX);
    value->simulationOnly = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiStrategyProjectConfigArchiveValidate(const UmiStrategyProjectConfig *value)
{
    return umi_strategy_project_config_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_strategy_project_config_archive_encode, umi_strategy_project_config_archive_decode,
    UmiStrategyProjectConfig, UmiStrategyProjectConfigArchiveSchema, UmiStrategyProjectConfigArchiveBound, UmiStrategyProjectConfigArchiveWrite, UmiStrategyProjectConfigArchiveRead, UmiStrategyProjectConfigArchiveValidate)
