/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ibkr_connection/rule_report_fixture.h
 * PURPOSE: Provide invented metadata shared by pure and native report regressions.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_TEST_RULE_REPORT_FIXTURE_H
#define UMICOM_TEST_RULE_REPORT_FIXTURE_H
/* No account, real contract or broker credentials are needed to read a file. */
#define RULE_REPORT_FIXTURE                                                                        \
  "{\"generated_utc\":\"2026-10-12T10:11:12.123456+00:00\",\"side\":\"BUY\","                      \
  "\"contract\":{\"conid\":123,\"instrument_type\":\"STK\",\"symbol\":\"EXAMPLE\","                \
  "\"local_symbol\":\"EXAMPLE\",\"currency\":\"GBP\"},"                                            \
  "\"routes\":[{\"requested_route\":\"SMART\",\"status\":\"metadata_received\","                   \
  "\"time_in_force_entries\":[\"DAY/o,a\",\"FOK/"                                                  \
  "LIMIT,a\"],\"order_types\":[\"limit\",\"market\"],"                                             \
  "\"priceMagnifier\":100,\"sizeIncrement\":1,\"increment\":0.01}]}"
#endif
