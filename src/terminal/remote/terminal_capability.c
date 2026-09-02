/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/terminal/remote/terminal_capability.c
 *
 * PURPOSE:
 *   Implement capability-subset testing for terminal capability.
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
#include "umicom/terminal/remote/terminal_capability.h"
/*
 * Provide the terminal remote terminal capability satisfies operation used by this module
 * and its client applications.
 */
bool umi_terminal_remote_terminal_capability_satisfies(UmiTerminalRemoteTerminalCapabilityFlags available,UmiTerminalRemoteTerminalCapabilityFlags required) { return (available & required)==required; }
/*
 * Return the number of records represented by terminal remote terminal capability without
 * changing their state.
 */
size_t umi_terminal_remote_terminal_capability_count(UmiTerminalRemoteTerminalCapabilityFlags flags) { size_t n=0U; /* Continue only while work remains available; the loop body advances the state on each pass. */ while(flags) { n+=(size_t)(flags&UINT64_C(1)); flags>>=1U; } return n; }
