/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_platform/context_source.c
 *
 * PURPOSE:
 *   Describe a source of developer context such as repository files, diagnostics, history or symbols.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable AI developer capability. Studio, Desk and
 *   future applications consume it through stable C23 contracts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai/developer_platform/context_source.h"
#include "../base/value_archive_internal.h"

#include <string.h>

/*
 * Copy ai dev context source into module-owned storage so callers keep ownership of their
 * input values.
 */
static void umi_ai_dev_context_source_copy(char *dst, size_t cap, const char *src) {
    size_t i = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (dst == NULL || cap == 0U) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (src != NULL) {
        /*
         * Continue only while work remains available; the loop body advances the state on each
         * pass.
         */
        while (i + 1U < cap && src[i] != '\0') { dst[i] = src[i]; ++i; }
    }
    dst[i] = '\0';
}

/*
 * Initialise ai dev context source from caller-provided values so later operations receive
 * a known state.
 */
void umi_ai_dev_context_source_init(UmiAiDevContextSource *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return;
    memset(value, 0, sizeof(*value));
    value->enabled = 1;
}
/*
 * Provide the ai dev context source configure operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_dev_context_source_configure(UmiAiDevContextSource *value, const char *id, const char *label, uint32_t priority, uint64_t flags) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || id == NULL || id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    umi_ai_dev_context_source_init(value);
    umi_ai_dev_context_source_copy(value->id, sizeof(value->id), id);
    umi_ai_dev_context_source_copy(value->label, sizeof(value->label), label);
    value->priority = priority; value->flags = flags; value->revision = 1U;
    return UMI_STATUS_OK;
}
/*
 * Check that ai dev context source satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_ai_dev_context_source_validate(const UmiAiDevContextSource *value) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->id, '\0', sizeof(value->id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(value->label, '\0', sizeof(value->label)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || value->id[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    return value->enabled ? UMI_STATUS_OK : UMI_STATUS_UNAVAILABLE;
}
/*
 * Provide the ai dev context source evidence score operation used by this module and its
 * client applications.
 */
uint32_t umi_ai_dev_context_source_evidence_score(const UmiAiDevContextSource *value, uint32_t relevance) {
    uint32_t bonus;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL || !value->enabled) return 0U;
    bonus = value->priority > 20U ? 20U : value->priority;
    relevance = relevance > 80U ? 80U : relevance;
    return relevance + bonus;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAiDevContextSourceArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xcb536fe433240d4e);
    schema = (schema ^ (uint64_t)sizeof(((UmiAiDevContextSource *)0)->id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiAiDevContextSource *)0)->label)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAiDevContextSourceArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAiDevContextSource *)0)->id) - 1U +
        8U + sizeof(((UmiAiDevContextSource *)0)->label) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiAiDevContextSourceArchiveWrite(UmiArchiveWriter *writer, const UmiAiDevContextSource *value)
{
    UmiArchiveWriteText(writer, value->id, sizeof(value->id));
    UmiArchiveWriteText(writer, value->label, sizeof(value->label));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->flags);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->priority);
    UmiArchiveWriteSigned(writer, (int64_t)value->enabled);
}
static void UmiAiDevContextSourceArchiveRead(UmiArchiveReader *reader, UmiAiDevContextSource *value)
{
    UmiArchiveReadText(reader, value->id, sizeof(value->id));
    UmiArchiveReadText(reader, value->label, sizeof(value->label));
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->flags = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
    value->priority = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->enabled = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAiDevContextSourceArchiveValidate(const UmiAiDevContextSource *value)
{
    return umi_ai_dev_context_source_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_ai_dev_context_source_archive_encode, umi_ai_dev_context_source_archive_decode,
    UmiAiDevContextSource, UmiAiDevContextSourceArchiveSchema, UmiAiDevContextSourceArchiveBound, UmiAiDevContextSourceArchiveWrite, UmiAiDevContextSourceArchiveRead, UmiAiDevContextSourceArchiveValidate)
