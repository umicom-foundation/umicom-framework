/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/position.c
 *
 * PURPOSE:
 *   Apply fills to a position using average-cost accounting for same-direction exposure.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This source implements the small deterministic core of position. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/position.h"
#include "umicom/trading/instrument.h"
#include <math.h>

/* Provide the absolute quantity operation used by this module and its client applications. */
static double absolute_quantity(double value)
{
    return value < 0.0 ? -value : value;
}

/*
 * Provide the position apply fill operation used by this module and its client
 * applications.
 */
UmiStatus umi_position_apply_fill(UmiPosition *position,
                                  UmiSide side,
                                  double quantity,
                                  double price)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (position == NULL || !umi_instrument_valid(&position->instrument) ||
        (side != UMI_SIDE_BUY && side != UMI_SIDE_SELL) ||
        !isfinite(quantity) || !isfinite(price) || quantity <= 0.0 || price <= 0.0 ||
        !isfinite(position->quantity) || !isfinite(position->average_price) ||
        !isfinite(position->realised_pnl) || position->average_price < 0.0 ||
        (position->quantity != 0.0 && position->average_price == 0.0)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    UmiPosition candidate = *position;
    const double signed_quantity =
        side == UMI_SIDE_BUY ? quantity : -quantity;
    const double old_quantity = position->quantity;
    const double next_quantity = old_quantity + signed_quantity;
    if (!isfinite(next_quantity)) return UMI_STATUS_CAPACITY_EXCEEDED;

    const int same_direction =
        old_quantity == 0.0 ||
        (old_quantity > 0.0 && signed_quantity > 0.0) ||
        (old_quantity < 0.0 && signed_quantity < 0.0);

    /* Apply this branch only when its contract condition is satisfied. */
    if (same_direction) {
        const double old_absolute = absolute_quantity(old_quantity);
        const double added_absolute = absolute_quantity(signed_quantity);
        const double total_absolute = old_absolute + added_absolute;

        /* Interpolate positive endpoints instead of multiplying price by
         * quantity first. Their difference cannot overflow. */
        const double old_price = position->average_price;
        candidate.average_price = old_absolute == 0.0 ? price :
            (old_price <= price
                ? old_price + (price - old_price) * (added_absolute / total_absolute)
                : price + (old_price - price) * (old_absolute / total_absolute));
        candidate.quantity = next_quantity;
        if (!isfinite(candidate.average_price)) return UMI_STATUS_CAPACITY_EXCEEDED;
        *position = candidate;
        return UMI_STATUS_OK;
    }

    double closing_quantity = quantity;
    const double old_absolute = absolute_quantity(old_quantity);
    /* Apply this branch only when its contract condition is satisfied. */
    if (closing_quantity > old_absolute) {
        closing_quantity = old_absolute;
    }

    const double direction = old_quantity > 0.0 ? 1.0 : -1.0;
    const double realised = (price - position->average_price) * closing_quantity *
        direction * position->instrument.multiplier;
    candidate.realised_pnl += realised;
    candidate.quantity = next_quantity;
    if (!isfinite(realised) || !isfinite(candidate.realised_pnl))
        return UMI_STATUS_CAPACITY_EXCEEDED;

    /* Apply this branch only when its contract condition is satisfied. */
    if (candidate.quantity == 0.0) {
        candidate.average_price = 0.0;
    } else /* Apply this branch only when its contract condition is satisfied. */ if ((old_quantity > 0.0 && candidate.quantity < 0.0) ||
               (old_quantity < 0.0 && candidate.quantity > 0.0)) {
        candidate.average_price = price;
    }
    *position = candidate;

    return UMI_STATUS_OK;
}
