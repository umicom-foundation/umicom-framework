/* Umicom Foundation | Sammy Hegab | MIT
 * A real linker-generated PE bootstrap with no imports. Format fixture only:
 * not the Setup Centre GUI, not a functioning installer, never executed. */
volatile int UmiBootstrapFixture;
void mainCRTStartup(void) { UmiBootstrapFixture = 23; }
