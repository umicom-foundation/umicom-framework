/* Umicom Framework | Sammy Hegab, Umicom Foundation | MIT */
#ifndef UMICOM_TEST_SDK_FIXTURE_H
#define UMICOM_TEST_SDK_FIXTURE_H

/* Shared base operation makes the consumer require a real linked dependency. */
int sdk_base_value(void);
/* Compose the base result in a separate library to verify transitive linkage. */
int sdk_composed_value(void);
/* Optional late registration must also reach the installed consumer. */
int sdk_deferred_value(void);
#endif
