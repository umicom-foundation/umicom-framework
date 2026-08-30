/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/codeguard/category.c
 *
 * PURPOSE:
 *   Convert CodeGuard category values into stable machine-readable names.
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

#include "umicom/codeguard/category.h"
const char *umi_codeguard_category_text(UmiCodeGuardCategory category)
{
    static const char *const names[] = {
        "memory","buffer","string","integer","resource","concurrency","input",
        "command","filesystem","secret","network","database","format","architecture",
        "duplication","portability","quality"
    };
    return (unsigned)category < (sizeof(names) / sizeof(names[0])) ? names[(unsigned)category] : "unknown";
}
