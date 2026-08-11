/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/position.c
 *
 * PURPOSE:
 *   Apply fills to a position using average-cost accounting for same-direction exposure.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of position. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/position.h"

static double absolute_quantity(double value)
{
    return value < 0.0 ? -value : value;
}

UmiStatus umi_position_apply_fill(UmiPosition *position,
                                  UmiSide side,
                                  double quantity,
                                  double price)
{
    if (position == NULL || quantity <= 0.0 || price <= 0.0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    const double signed_quantity =
        side == UMI_SIDE_BUY ? quantity : -quantity;
    const double old_quantity = position->quantity;

    const int same_direction =
        old_quantity == 0.0 ||
        (old_quantity > 0.0 && signed_quantity > 0.0) ||
        (old_quantity < 0.0 && signed_quantity < 0.0);

    if (same_direction) {
        const double old_absolute = absolute_quantity(old_quantity);
        const double added_absolute = absolute_quantity(signed_quantity);
        const double total_absolute = old_absolute + added_absolute;

        position->average_price =
            total_absolute > 0.0
                ? ((position->average_price * old_absolute) +
                   (price * added_absolute)) /
                      total_absolute
                : 0.0;
        position->quantity = old_quantity + signed_quantity;
        return UMI_STATUS_OK;
    }

    double closing_quantity = quantity;
    const double old_absolute = absolute_quantity(old_quantity);
    if (closing_quantity > old_absolute) {
        closing_quantity = old_absolute;
    }

    const double direction = old_quantity > 0.0 ? 1.0 : -1.0;
    position->realised_pnl +=
        (price - position->average_price) * closing_quantity * direction;
    position->quantity = old_quantity + signed_quantity;

    if (position->quantity == 0.0) {
        position->average_price = 0.0;
    } else if ((old_quantity > 0.0 && position->quantity < 0.0) ||
               (old_quantity < 0.0 && position->quantity > 0.0)) {
        position->average_price = price;
    }

    return UMI_STATUS_OK;
}
