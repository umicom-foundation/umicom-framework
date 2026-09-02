/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/row_key.c
 *
 * PURPOSE:
 *   Implement deterministic FNV-1a based row keys.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/row_key.h"
/*
 * Provide the ui ent row key from text operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_ent_row_key_from_text(const char *text){uint64_t h=UINT64_C(1469598103934665603);const unsigned char *p=(const unsigned char*)text;/* Apply this branch only when its contract condition is satisfied. */ if(!p)return 0U;/* Continue only while work remains available; the loop body advances the state on each pass. */ while(*p){h^=(uint64_t)*p++;h*=UINT64_C(1099511628211);}return h?h:1U;}
/*
 * Provide the ui ent row key combine operation used by this module and its client
 * applications.
 */
uint64_t umi_ui_ent_row_key_combine(uint64_t a,uint64_t b){uint64_t x=a^(b+UINT64_C(0x9e3779b97f4a7c15)+(a<<6U)+(a>>2U));return x?x:1U;}
