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
typedef struct UmiUiEntColumnCatalogue { UmiUiEntColumnDescriptor items[UMI_UI_ENT_MAX_COLUMNS]; size_t count; uint64_t revision; } UmiUiEntColumnCatalogue;
void umi_ui_ent_column_catalogue_init(UmiUiEntColumnCatalogue *c);
UmiStatus umi_ui_ent_column_catalogue_add(UmiUiEntColumnCatalogue *c,const UmiUiEntColumnDescriptor *item);
const UmiUiEntColumnDescriptor *umi_ui_ent_column_catalogue_find(const UmiUiEntColumnCatalogue *c,const char *id);
UmiStatus umi_ui_ent_column_catalogue_move(UmiUiEntColumnCatalogue *c,size_t from,size_t to);

#endif
