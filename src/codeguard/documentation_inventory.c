/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/documentation_inventory.c
 *
 * PURPOSE:
 *   Combine bounded documentation reports into workspace-level evidence while
 *   rejecting malformed counters and arithmetic overflow.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/codeguard/documentation_inventory.h"

#include <stdint.h>
#include <string.h>

/* Convert a valid count ratio without multiplying the whole numerator by 100. */
static unsigned inventory_percent(size_t value, size_t total)
{
    size_t accumulator = 0U;
    unsigned percent = 0U;
    unsigned step;
    /* An empty category has no missing documentation and is treated as complete. */
    if (total == 0U) {
        return 100U;
    }
    /* One hundred bounded additions avoid value * 100 integer overflow. */
    for (step = 0U; step < 100U; ++step) {
        /* Subtraction before addition keeps the accumulator below total. */
        if (accumulator >= total - value) {
            accumulator -= total - value;
            percent += 1U;
        } else {
            accumulator += value;
        }
    }
    return percent;
}

/* Validate public structure identity before reading any of its aggregate fields. */
static bool inventory_is_valid(
    const UmiCodeGuardDocumentationInventory *inventory)
{
    /* Older, unknown or internally inconsistent structures cannot be trusted. */
    if (inventory == NULL || inventory->structure_size < sizeof(*inventory) ||
        inventory->api_version != UMI_CODEGUARD_DOCUMENTATION_INVENTORY_API_VERSION) {
        return false;
    }
    /* Passing and header counts describe subsets of the recorded files. */
    if (inventory->passing_file_count > inventory->file_count ||
        inventory->standard_header_count > inventory->file_count) {
        return false;
    }
    /* Documented candidates must always be subsets of their candidate totals. */
    if (inventory->documented_function_count > inventory->function_count ||
        inventory->explained_decision_count > inventory->decision_count) {
        return false;
    }
    return true;
}

/* Check one size_t addition before changing any aggregate evidence. */
static bool inventory_can_add(size_t current, size_t addition)
{
    /* SIZE_MAX is the largest representable counter on the current target. */
    return addition <= SIZE_MAX - current;
}

/* Validate all additions first so recording is atomic from the caller's view. */
static bool inventory_can_record(
    const UmiCodeGuardDocumentationInventory *inventory,
    const UmiCodeGuardDocumentationReport *report)
{
    /* Every field is checked before the first counter is modified. */
    return inventory_can_add(inventory->file_count, 1U) &&
        inventory_can_add(inventory->passing_file_count, report->passes ? 1U : 0U) &&
        inventory_can_add(inventory->standard_header_count,
                          report->has_standard_file_header ? 1U : 0U) &&
        inventory_can_add(inventory->line_count, report->line_count) &&
        inventory_can_add(inventory->function_count, report->function_count) &&
        inventory_can_add(inventory->documented_function_count,
                          report->documented_function_count) &&
        inventory_can_add(inventory->decision_count, report->decision_count) &&
        inventory_can_add(inventory->explained_decision_count,
                          report->explained_decision_count);
}

/* Prepare an empty inventory with explicit ABI identity. */
void umi_codeguard_documentation_inventory_init(
    UmiCodeGuardDocumentationInventory *inventory)
{
    /* NULL-safe initialisation simplifies optional quality-service startup. */
    if (inventory == NULL) {
        return;
    }
    (void)memset(inventory, 0, sizeof(*inventory));
    inventory->structure_size = (uint32_t)sizeof(*inventory);
    inventory->api_version = UMI_CODEGUARD_DOCUMENTATION_INVENTORY_API_VERSION;
}

/* Expose validation so applications can reject corrupted persisted totals safely. */
UmiStatus umi_codeguard_documentation_inventory_validate(
    const UmiCodeGuardDocumentationInventory *inventory)
{
    /* The private validator owns all structural consistency rules. */
    if (!inventory_is_valid(inventory)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return UMI_STATUS_OK;
}

/* Add a single file report without retaining its path or borrowing its memory. */
UmiStatus umi_codeguard_documentation_inventory_record(
    UmiCodeGuardDocumentationInventory *inventory,
    const UmiCodeGuardDocumentationReport *report)
{
    /* Structure and subset checks prevent malformed reports corrupting totals. */
    if (!inventory_is_valid(inventory) || report == NULL ||
        report->structure_size < sizeof(*report) ||
        report->documented_function_count > report->function_count ||
        report->explained_decision_count > report->decision_count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Refuse the complete operation before any counter can wrap around. */
    if (!inventory_can_record(inventory, report)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    inventory->file_count += 1U;
    inventory->passing_file_count += report->passes ? 1U : 0U;
    inventory->standard_header_count += report->has_standard_file_header ? 1U : 0U;
    inventory->line_count += report->line_count;
    inventory->function_count += report->function_count;
    inventory->documented_function_count += report->documented_function_count;
    inventory->decision_count += report->decision_count;
    inventory->explained_decision_count += report->explained_decision_count;
    return UMI_STATUS_OK;
}

/* Combine independent scan totals after proving every addition is representable. */
UmiStatus umi_codeguard_documentation_inventory_merge(
    UmiCodeGuardDocumentationInventory *inventory,
    const UmiCodeGuardDocumentationInventory *source)
{
    /* Both inventories must use the current, internally consistent contract. */
    if (!inventory_is_valid(inventory) || !inventory_is_valid(source)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* A self-merge would double every count accidentally and is not meaningful. */
    if (inventory == source) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Check the entire merge before modifying the destination. */
    if (!inventory_can_add(inventory->file_count, source->file_count) ||
        !inventory_can_add(inventory->passing_file_count,
                           source->passing_file_count) ||
        !inventory_can_add(inventory->standard_header_count,
                           source->standard_header_count) ||
        !inventory_can_add(inventory->line_count, source->line_count) ||
        !inventory_can_add(inventory->function_count, source->function_count) ||
        !inventory_can_add(inventory->documented_function_count,
                           source->documented_function_count) ||
        !inventory_can_add(inventory->decision_count, source->decision_count) ||
        !inventory_can_add(inventory->explained_decision_count,
                           source->explained_decision_count)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    inventory->file_count += source->file_count;
    inventory->passing_file_count += source->passing_file_count;
    inventory->standard_header_count += source->standard_header_count;
    inventory->line_count += source->line_count;
    inventory->function_count += source->function_count;
    inventory->documented_function_count += source->documented_function_count;
    inventory->decision_count += source->decision_count;
    inventory->explained_decision_count += source->explained_decision_count;
    return UMI_STATUS_OK;
}

/* Calculate workspace file compliance from validated aggregate evidence. */
unsigned umi_codeguard_documentation_inventory_file_percent(
    const UmiCodeGuardDocumentationInventory *inventory)
{
    /* Invalid evidence returns zero so a quality dashboard fails visibly. */
    if (!inventory_is_valid(inventory)) {
        return 0U;
    }
    return inventory_percent(inventory->passing_file_count, inventory->file_count);
}

/* Calculate function guidance across all files rather than averaging percentages. */
unsigned umi_codeguard_documentation_inventory_function_percent(
    const UmiCodeGuardDocumentationInventory *inventory)
{
    /* Invalid evidence returns zero instead of presenting false completeness. */
    if (!inventory_is_valid(inventory)) {
        return 0U;
    }
    return inventory_percent(
        inventory->documented_function_count, inventory->function_count);
}

/* Calculate decision guidance across all files rather than averaging percentages. */
unsigned umi_codeguard_documentation_inventory_decision_percent(
    const UmiCodeGuardDocumentationInventory *inventory)
{
    /* Invalid evidence returns zero instead of presenting false completeness. */
    if (!inventory_is_valid(inventory)) {
        return 0U;
    }
    return inventory_percent(
        inventory->explained_decision_count, inventory->decision_count);
}

/* Require real evidence and complete per-file compliance for a passing inventory. */
bool umi_codeguard_documentation_inventory_passes(
    const UmiCodeGuardDocumentationInventory *inventory)
{
    /* An empty workspace has not produced evidence and must not pass a release gate. */
    if (!inventory_is_valid(inventory) || inventory->file_count == 0U) {
        return false;
    }
    return inventory->passing_file_count == inventory->file_count;
}
