/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/row_descriptor.h
 *
 * PURPOSE:
 *   Describe a stable enterprise row identity, display label and revision.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_ROW_DESCRIPTOR_H
#define UMICOM_UI_ENTERPRISE_ROW_DESCRIPTOR_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiUiEntRowDescriptor {
    uint64_t row_key;
    char label[UMI_UI_ENT_TEXT_CAPACITY];
    int selectable;
    int editable;
    int enabled;
    uint64_t revision;
} UmiUiEntRowDescriptor;
UmiStatus umi_ui_ent_row_descriptor_init(UmiUiEntRowDescriptor *value);
int umi_ui_ent_row_descriptor_validate(const UmiUiEntRowDescriptor *value);
#ifdef __cplusplus
}
#endif

#endif
