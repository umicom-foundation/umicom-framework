/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/command_catalogue.h
 *
 * PURPOSE:
 *   Publish stable Framework command descriptors for future Studio and Desk
 *   productisation, coverage and completion views.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_COMMAND_CATALOGUE_H
#define UMICOM_APPLICATION_PRODUCTISATION_COMMAND_CATALOGUE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the productisation command definition data shared with callers of this public
 * contract.
 */
typedef struct UmiProductisationCommandDefinition {
    const char *command_id;
    const char *title;
    const char *category;
    const char *description;
    int mutates_evidence;
} UmiProductisationCommandDefinition;

/**
 * Return the number of stable commands in the productisation catalogue.
 *
 * The count is the exclusive upper bound accepted by
 * `umi_productisation_command_catalogue_at`.
 *
 * @return Number of command definitions available for menus and palettes.
 */
size_t umi_productisation_command_catalogue_count(void);

/**
 * Read one productisation command by its zero-based catalogue position.
 *
 * The returned definition is Framework-owned and remains valid for the life
 * of the process. The caller must not change or free it.
 *
 * @param index Position smaller than the catalogue count.
 * @return Command definition, or `NULL` when `index` is outside the catalogue.
 */
const UmiProductisationCommandDefinition *
umi_productisation_command_catalogue_at(size_t index);

/**
 * Find a productisation command by its stable identifier.
 *
 * @param command_id Non-empty identifier used by command dispatch and menus.
 * @return Framework-owned definition, or `NULL` when no command matches.
 */
const UmiProductisationCommandDefinition *
umi_productisation_command_catalogue_find(const char *command_id);

#ifdef __cplusplus
}
#endif

#endif
