/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/row_key.c
 *
 * PURPOSE:
 *   Implement deterministic FNV-1a based row keys.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/row_key.h"
uint64_t umi_ui_ent_row_key_from_text(const char *text){uint64_t h=UINT64_C(1469598103934665603);const unsigned char *p=(const unsigned char*)text;if(!p)return 0U;while(*p){h^=(uint64_t)*p++;h*=UINT64_C(1099511628211);}return h?h:1U;}
uint64_t umi_ui_ent_row_key_combine(uint64_t a,uint64_t b){uint64_t x=a^(b+UINT64_C(0x9e3779b97f4a7c15)+(a<<6U)+(a>>2U));return x?x:1U;}
