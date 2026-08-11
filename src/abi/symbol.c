/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/abi/symbol.c
 *
 * PURPOSE:
 *   Validate stable exported-symbol metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source file implements one small part of ABI validation. Keeping checks separate makes release failures easier to understand and test.
 */
#include "umicom/abi/symbol.h"
#include <stddef.h>
UmiStatus umi_abi_symbol_validate(const UmiAbiSymbol *s){if(s==NULL||s->name==NULL||s->name[0]=='\0'||s->since_abi==0U)return UMI_STATUS_INVALID_ARGUMENT;return UMI_STATUS_OK;}
