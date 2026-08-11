/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/trading/movement_event.c
 *
 * PURPOSE:
 *   Detect price movements that exceed a configured point threshold within a time window.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This source implements the small deterministic core of movement event. Product-specific UI and vendor details stay outside this file.
 */

#include "umicom/trading/movement_event.h"

UmiMovementEvent umi_movement_event_measure(double start_price,
                                            double end_price,
                                            int64_t start_ms,
                                            int64_t end_ms,
                                            double minimum_points,
                                            int64_t maximum_duration_ms)
{
    UmiMovementEvent event = {0};
    event.start_price = start_price;
    event.end_price = end_price;
    event.absolute_points = end_price - start_price;

    if (event.absolute_points < 0.0) {
        event.absolute_points = -event.absolute_points;
    }

    event.start_time_ms = start_ms;
    event.end_time_ms = end_ms;

    const int64_t elapsed_ms = end_ms - start_ms;
    event.qualifies =
        start_price > 0.0 &&
        end_price > 0.0 &&
        elapsed_ms >= 0 &&
        elapsed_ms <= maximum_duration_ms &&
        event.absolute_points >= minimum_points;

    return event;
}
