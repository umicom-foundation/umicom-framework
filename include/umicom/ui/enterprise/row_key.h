/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/row_key.h
 *
 * PURPOSE:
 *   Create stable row keys for virtualised data models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_ROW_KEY_H
#define UMICOM_UI_ENTERPRISE_ROW_KEY_H

#include <stdint.h>
#include "umicom/ui/enterprise/types.h"
uint64_t umi_ui_ent_row_key_from_text(const char *text);
uint64_t umi_ui_ent_row_key_combine(uint64_t parent,uint64_t child);

#endif
