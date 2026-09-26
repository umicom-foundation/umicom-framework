/* Umicom Foundation | Sammy Hegab | MIT
 * PE format-only consumer. No CRT/Windows SDK is linked. Never execute this
 * fixture: even its delay helper is deliberately not a functioning loader. */
__declspec(dllimport) int UmiPracticeValue(void);
volatile int UmiFixtureResult;
void mainCRTStartup(void) { UmiFixtureResult = UmiPracticeValue(); }
#ifdef UMICOM_FORMAT_DELAY
/* Satisfies the linker's delay thunk only, to inspect its real metadata.
 * The fixture is not a runtime test of delay loading. */
void *__delayLoadHelper2(const void *descriptor, void **slot)
{
    (void)descriptor; (void)slot;
    return (void *)0;
}
#endif
