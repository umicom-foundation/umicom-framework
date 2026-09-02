/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_toolchain.c
 *
 * PURPOSE:
 *   Implement deterministic remote toolchain validation and identity.
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
#include "umicom/terminal/remote/remote_toolchain.h"
#include <string.h>
/*
 * Initialise terminal remote remote toolchain from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_remote_toolchain_init(UmiTerminalRemoteRemoteToolchain *value,const char *left_id,const char *right_id) { /* Use the stable identifier comparison to choose the matching record or policy. */ if(!value) return; (void)memset(value,0,sizeof(*value)); /* Use the stable identifier comparison to choose the matching record or policy. */ if(left_id) (void)umi_terminal_remote_copy_text(value->left_id,sizeof(value->left_id),left_id); /* Use the stable identifier comparison to choose the matching record or policy. */ if(right_id) (void)umi_terminal_remote_copy_text(value->right_id,sizeof(value->right_id),right_id); value->revision=1U; value->enabled=true; }
/*
 * Check that terminal remote remote toolchain satisfies its contract before another
 * service relies on it.
 */
bool umi_terminal_remote_remote_toolchain_valid(const UmiTerminalRemoteRemoteToolchain *value) { return value && value->enabled && value->left_id[0]!='\0' && value->right_id[0]!='\0' && strcmp(value->left_id,value->right_id)!=0; }
/*
 * Provide the terminal remote remote toolchain fingerprint operation used by this module
 * and its client applications.
 */
uint64_t umi_terminal_remote_remote_toolchain_fingerprint(const UmiTerminalRemoteRemoteToolchain *value) { /* Use the stable identifier comparison to choose the matching record or policy. */ if(!umi_terminal_remote_remote_toolchain_valid(value)) return 0U; return umi_terminal_remote_fingerprint_text(value->left_id) ^ (umi_terminal_remote_fingerprint_text(value->right_id)<<1U); }
