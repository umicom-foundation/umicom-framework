/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/codeguard/rules/rules.h
 *
 * PURPOSE:
 *   Include every built-in CodeGuard vulnerability and architecture rule pack.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is intentionally small and focused. CodeGuard separates scanning
 * rules, analysis engines and reports so new developers can understand one
 * responsibility at a time and test it independently.
 */
#ifndef UMICOM_CODEGUARD_RULES_RULES_H
#define UMICOM_CODEGUARD_RULES_RULES_H
#include "umicom/codeguard/rules/memory.h"
#include "umicom/codeguard/rules/buffer.h"
#include "umicom/codeguard/rules/string.h"
#include "umicom/codeguard/rules/allocation.h"
#include "umicom/codeguard/rules/pointer.h"
#include "umicom/codeguard/rules/lifetime_rules.h"
#include "umicom/codeguard/rules/format.h"
#include "umicom/codeguard/rules/integer.h"
#include "umicom/codeguard/rules/arithmetic.h"
#include "umicom/codeguard/rules/file.h"
#include "umicom/codeguard/rules/path.h"
#include "umicom/codeguard/rules/temp_file.h"
#include "umicom/codeguard/rules/command.h"
#include "umicom/codeguard/rules/process.h"
#include "umicom/codeguard/rules/environment.h"
#include "umicom/codeguard/rules/secret.h"
#include "umicom/codeguard/rules/logging.h"
#include "umicom/codeguard/rules/network.h"
#include "umicom/codeguard/rules/sql.h"
#include "umicom/codeguard/rules/concurrency.h"
#include "umicom/codeguard/rules/atomic.h"
#include "umicom/codeguard/rules/lock.h"
#include "umicom/codeguard/rules/resource.h"
#include "umicom/codeguard/rules/error.h"
#include "umicom/codeguard/rules/input.h"
#include "umicom/codeguard/rules/portability.h"
#include "umicom/codeguard/rules/global_state.h"
#include "umicom/codeguard/rules/private_header.h"
#include "umicom/codeguard/rules/database_boundary.h"
#include "umicom/codeguard/rules/ui_boundary.h"
#include "umicom/codeguard/rules/coupling_rules.h"
#include "umicom/codeguard/rules/duplicate_rules.h"
#endif
