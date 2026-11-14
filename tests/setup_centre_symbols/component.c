/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/setup_centre_symbols/component.c
 * PURPOSE: Exercise the private helper namespace in a separate translation unit.
 * EDUCATIONAL OVERVIEW: Small fixture functions isolate link-name behaviour;
 * these are not substitutes for the actual installation implementations.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#include "../../src/setup_centre/symbol_names.h"
/* Source spelling is retained, but the emitted symbol belongs to Setup Centre. */
int ScCopy(void) { return 11; }
/* A second private helper must not collide with another library's reader. */
int ScRead(void) { return 12; }
/* Text ownership helpers use the same qualified namespace. */
int ScTextFree(void) { return 13; }
/* Path helper names are qualified without changing their source spelling. */
int ScJoin(void) { return 14; }
