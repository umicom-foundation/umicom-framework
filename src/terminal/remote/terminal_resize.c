/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/terminal_resize.c
 *
 * PURPOSE:
 *   Implement bounded terminal resize validation.
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
#include "umicom/terminal/remote/terminal_resize.h"
/*
 * Initialise terminal remote terminal resize from caller-provided values so later
 * operations receive a known state.
 */
void umi_terminal_remote_terminal_resize_init(UmiTerminalRemoteTerminalResize *value,uint32_t columns,uint32_t rows) { /* Apply this branch only when its contract condition is satisfied. */ if(value) { value->columns=columns; value->rows=rows; value->revision=1U; } }
/*
 * Copy terminal remote terminal resize into module-owned storage so callers keep ownership
 * of their input values.
 */
UmiStatus umi_terminal_remote_terminal_resize_set(UmiTerminalRemoteTerminalResize *value,uint32_t columns,uint32_t rows) { /* Preserve the original failure result so the caller can respond to the correct cause. */ if(!value||columns<2U||rows<2U||columns>1000U||rows>500U) return UMI_STATUS_INVALID_ARGUMENT; value->columns=columns; value->rows=rows; value->revision++; return UMI_STATUS_OK; }
