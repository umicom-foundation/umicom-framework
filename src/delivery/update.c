/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/delivery/update.c
 *
 * PURPOSE:
 *   Compare installed and available generations and produce a safe update decision.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Updates are based on immutable generation numbers so the application can explain why a newer release is or is not eligible.
 */

#include "umicom/delivery/update.h"

UmiUpdateDecision umi_update_decide(uint64_t installed_generation,
                                    uint64_t available_generation,
                                    int compatible,
                                    int policy_allows)
{
    if (available_generation <= installed_generation) return UMI_UPDATE_NONE;
    if (!compatible || !policy_allows) return UMI_UPDATE_BLOCKED;
    return UMI_UPDATE_AVAILABLE;
}
