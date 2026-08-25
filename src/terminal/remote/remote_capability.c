/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/remote_capability.c
 *
 * PURPOSE:
 *   Implement capability-subset testing for remote capability.
 *
 * ARCHITECTURE:
 *   Framework owns this reusable terminal/process/remote-development capability.
 *   Applications consume the contract and do not duplicate operational logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_capability.h"
bool umi_terminal_remote_remote_capability_satisfies(UmiTerminalRemoteRemoteCapabilityFlags available,UmiTerminalRemoteRemoteCapabilityFlags required) { return (available & required)==required; }
size_t umi_terminal_remote_remote_capability_count(UmiTerminalRemoteRemoteCapabilityFlags flags) { size_t n=0U; while(flags) { n+=(size_t)(flags&UINT64_C(1)); flags>>=1U; } return n; }
