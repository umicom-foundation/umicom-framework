/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/setup_centre_symbols/foreign.c
 * PURPOSE: Represent an independent library using the original short spellings.
 * EDUCATIONAL OVERVIEW: This unit deliberately does not include Setup Centre's
 * private header; both sets of fixture functions must coexist in one executable.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
/* An unrelated library retains its own copy helper symbol. */
int ScCopy(void) { return 21; }
/* The independent reader is not renamed by another component's private policy. */
int ScRead(void) { return 22; }
/* A separately owned text helper remains callable. */
int ScTextFree(void) { return 23; }
/* The independent path helper still resolves to its own implementation. */
int ScJoin(void) { return 24; }
