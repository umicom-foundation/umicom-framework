/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/frontend/conformance/command_contract.c
 *
 * PURPOSE:
 *   stable command exposure expectations independent of frontend toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/frontend/conformance/command_contract.h"

bool umi_fc_command_contract_validate(const UmiFcCommandContract *item){if(item==NULL)return false;return item->required_commands==0U || (item->command_fingerprint!=0U && item->all_have_stable_ids);}
