/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/setup_centre_symbols/main.c
 * PURPOSE: Link and call both helper namespaces to detect duplicate-symbol regressions.
 * EDUCATIONAL OVERVIEW: A successful compile alone cannot establish linkage;
 * all eight independently defined fixture functions are resolved and invoked.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* Declare the independently owned copy fixture. */
int ScCopy(void);
/* Declare the independently owned read fixture. */
int ScRead(void);
/* Declare the independently owned text fixture. */
int ScTextFree(void);
/* Declare the independently owned path fixture. */
int ScJoin(void);
/* Declare the emitted Setup Centre copy fixture. */
int UmiSetupCentreInternalCopy(void);
/* Declare the emitted Setup Centre read fixture. */
int UmiSetupCentreInternalRead(void);
/* Declare the emitted Setup Centre text fixture. */
int UmiSetupCentreInternalTextFree(void);
/* Declare the emitted Setup Centre path fixture. */
int UmiSetupCentreInternalJoin(void);

/* Verify dispatch reaches each component rather than resolving everything to one alias. */
int main(void)
{
    return !(UmiSetupCentreInternalCopy() == 11 && UmiSetupCentreInternalRead() == 12 &&
        UmiSetupCentreInternalTextFree() == 13 && UmiSetupCentreInternalJoin() == 14 &&
        ScCopy() == 21 && ScRead() == 22 && ScTextFree() == 23 && ScJoin() == 24);
}
