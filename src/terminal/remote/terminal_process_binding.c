/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/terminal_process_binding.c
 *
 * PURPOSE:
 *   Implement deterministic terminal process binding validation and identity.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/terminal_process_binding.h"
#include <string.h>
void umi_terminal_remote_terminal_process_binding_init(UmiTerminalRemoteTerminalProcessBinding *value,const char *left_id,const char *right_id) { if(!value) return; (void)memset(value,0,sizeof(*value)); if(left_id) (void)umi_terminal_remote_copy_text(value->left_id,sizeof(value->left_id),left_id); if(right_id) (void)umi_terminal_remote_copy_text(value->right_id,sizeof(value->right_id),right_id); value->revision=1U; value->enabled=true; }
bool umi_terminal_remote_terminal_process_binding_valid(const UmiTerminalRemoteTerminalProcessBinding *value) { return value && value->enabled && value->left_id[0]!='\0' && value->right_id[0]!='\0' && strcmp(value->left_id,value->right_id)!=0; }
uint64_t umi_terminal_remote_terminal_process_binding_fingerprint(const UmiTerminalRemoteTerminalProcessBinding *value) { if(!umi_terminal_remote_terminal_process_binding_valid(value)) return 0U; return umi_terminal_remote_fingerprint_text(value->left_id) ^ (umi_terminal_remote_fingerprint_text(value->right_id)<<1U); }
