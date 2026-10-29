/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: examples/queue_shutdown/client/lifecycle_cases.h
 *
 * PURPOSE:
 *   Declare independent installed-consumer checks for task and queue ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_SHUTDOWN_CONSUMER_LIFECYCLE_CASES_H
#define UMICOM_SHUTDOWN_CONSUMER_LIFECYCLE_CASES_H

/* Run one named public-API scenario. Zero means its assertions succeeded;
 * one reports a contract failure, and two reports an unknown scenario. */
int UmiShutdownConsumerRunCase(const char *name);

#endif
