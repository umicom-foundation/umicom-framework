/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/column_catalogue.h
 *
 * PURPOSE:
 *   Maintain an ordered bounded catalogue of advanced column descriptors.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_COLUMN_CATALOGUE_H
#define UMICOM_UI_ENTERPRISE_COLUMN_CATALOGUE_H

#include "umicom/ui/enterprise/column_descriptor.h"
/**
 * Represent the ui ent column catalogue data shared with callers of this public contract.
 */
typedef struct UmiUiEntColumnCatalogue { UmiUiEntColumnDescriptor items[UMI_UI_ENT_MAX_COLUMNS]; size_t count; uint64_t revision; } UmiUiEntColumnCatalogue;
/**
 * Initialise ui ent column catalogue from caller-provided values so later operations
 * receive a known state.
 */
void umi_ui_ent_column_catalogue_init(UmiUiEntColumnCatalogue *c);
/**
 * Add ui ent column catalogue only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_ui_ent_column_catalogue_add(UmiUiEntColumnCatalogue *c,const UmiUiEntColumnDescriptor *item);
/**
 * Find ui ent column catalogue while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiUiEntColumnDescriptor *umi_ui_ent_column_catalogue_find(const UmiUiEntColumnCatalogue *c,const char *id);
/**
 * Provide the ui ent column catalogue move operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_column_catalogue_move(UmiUiEntColumnCatalogue *c,size_t from,size_t to);

#endif
