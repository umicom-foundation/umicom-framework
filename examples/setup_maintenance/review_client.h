/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#ifndef UMICOM_MAINTENANCE_REVIEW_EXAMPLE_H
#define UMICOM_MAINTENANCE_REVIEW_EXAMPLE_H
#include "umicom/setup_centre/maintenance.h"
#include <stdio.h>
/* A complete read-only consumer. No private Framework header is included. */
UmiStatus UmiMaintenanceReviewExample(const char *installation,
    const char *release, FILE *output);
#endif
