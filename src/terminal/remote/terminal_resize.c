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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/terminal_resize.h"
void umi_terminal_remote_terminal_resize_init(UmiTerminalRemoteTerminalResize *value,uint32_t columns,uint32_t rows) { if(value) { value->columns=columns; value->rows=rows; value->revision=1U; } }
UmiStatus umi_terminal_remote_terminal_resize_set(UmiTerminalRemoteTerminalResize *value,uint32_t columns,uint32_t rows) { if(!value||columns<2U||rows<2U||columns>1000U||rows>500U) return UMI_STATUS_INVALID_ARGUMENT; value->columns=columns; value->rows=rows; value->revision++; return UMI_STATUS_OK; }
