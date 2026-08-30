/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/config.c
 *
 * PURPOSE:
 *   Construct a scan configuration with conservative defaults.
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

#include "umicom/codeguard/config.h"
UmiCodeGuardConfig umi_codeguard_config_default(const char *root){UmiCodeGuardConfig c;c.root=root;c.profile=umi_codeguard_profile_default();c.baseline_path=NULL;c.suppression_path=NULL;return c;}
