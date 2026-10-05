/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/transition_budget.c
 *
 * PURPOSE:
 *   Bound concurrent transitions and cumulative duration to avoid animation-heavy workstation surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/transition_budget.h"
#include "../../base/value_archive_internal.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_transition_budget_init(UmiAppearanceTransitionBudget *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->budget_id,sizeof item->budget_id,"transition.workstation");
    item->max_concurrent=8U;
    item->max_duration_ms=300U;
    item->max_delayed_ms=150U;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_transition_budget_is_valid(const UmiAppearanceTransitionBudget *item) {
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (item == NULL) return 0;
    if (memchr(item->budget_id, '\0', sizeof(item->budget_id)) == NULL) return 0;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->budget_id) && item->max_concurrent > 0U && item->max_duration_ms > 0U);
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAppearanceTransitionBudgetArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7f3c51b1cd495277);
    schema = (schema ^ (uint64_t)sizeof(((UmiAppearanceTransitionBudget *)0)->budget_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiAppearanceTransitionBudgetArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiAppearanceTransitionBudget *)0)->budget_id) - 1U +
        8U +
        8U +
        8U;
}
static void UmiAppearanceTransitionBudgetArchiveWrite(UmiArchiveWriter *writer, const UmiAppearanceTransitionBudget *value)
{
    UmiArchiveWriteText(writer, value->budget_id, sizeof(value->budget_id));
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->max_concurrent);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->max_duration_ms);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->max_delayed_ms);
}
static void UmiAppearanceTransitionBudgetArchiveRead(UmiArchiveReader *reader, UmiAppearanceTransitionBudget *value)
{
    UmiArchiveReadText(reader, value->budget_id, sizeof(value->budget_id));
    value->max_concurrent = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->max_duration_ms = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->max_delayed_ms = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
}
static UmiStatus UmiAppearanceTransitionBudgetArchiveValidate(const UmiAppearanceTransitionBudget *value)
{
    return umi_appearance_transition_budget_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_appearance_transition_budget_archive_encode, umi_appearance_transition_budget_archive_decode,
    UmiAppearanceTransitionBudget, UmiAppearanceTransitionBudgetArchiveSchema, UmiAppearanceTransitionBudgetArchiveBound, UmiAppearanceTransitionBudgetArchiveWrite, UmiAppearanceTransitionBudgetArchiveRead, UmiAppearanceTransitionBudgetArchiveValidate)
