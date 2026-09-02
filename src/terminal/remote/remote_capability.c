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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/terminal/remote/remote_capability.h"
/*
 * Provide the terminal remote remote capability satisfies operation used by this module
 * and its client applications.
 */
bool umi_terminal_remote_remote_capability_satisfies(UmiTerminalRemoteRemoteCapabilityFlags available,UmiTerminalRemoteRemoteCapabilityFlags required) { return (available & required)==required; }
/*
 * Return the number of records represented by terminal remote remote capability without
 * changing their state.
 */
size_t umi_terminal_remote_remote_capability_count(UmiTerminalRemoteRemoteCapabilityFlags flags) { size_t n=0U; /* Continue only while work remains available; the loop body advances the state on each pass. */ while(flags) { n+=(size_t)(flags&UINT64_C(1)); flags>>=1U; } return n; }
