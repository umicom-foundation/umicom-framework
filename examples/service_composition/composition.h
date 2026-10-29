/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/service_composition/composition.h
 *
 * PURPOSE:
 *   Describe a small application's owned controller and borrowed module state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EXAMPLE_SERVICE_COMPOSITION_H
#define UMICOM_EXAMPLE_SERVICE_COMPOSITION_H

#include "umicom/runtime/master_controller.h"

typedef struct ExampleCounter {
    unsigned value;
} ExampleCounter;

/* main owns this storage until checked controller release succeeds. Module
 * descriptors and destroy_user_data borrow it; neither registry frees it. */
typedef struct ExampleComposition {
    UmiMasterController *master;
    UmiModuleDescriptor provider;
    UmiModuleDescriptor consumer;
    ExampleCounter *borrowed_counter;
    unsigned counters_created;
    unsigned counters_destroyed;
    unsigned modules_destroyed;
    int service_registered;
    int consumer_running;
    int fail_start_once;
    int fail_stop_once;
} ExampleComposition;

/* Even if admission fails, the caller releases any non-NULL master before
 * allowing this storage to leave scope. Start/stop use Framework's public API. */
UmiStatus ExampleCompositionCreate(ExampleComposition *composition,
                                   int check_corrected_registration);

#endif
