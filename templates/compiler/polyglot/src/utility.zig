/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/compiler/polyglot/src/utility.zig
 *
 * PURPOSE:
 *   Provide reusable utility source for projects created with Umicom tooling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

// Umicom polyglot Zig unit | Sammy Hegab | Umicom Foundation | MIT

/// Clamp negative foreign-function input to zero and preserve non-negative values.
export fn umicom_clamp(value: c_int) c_int {
    // The exported boundary never returns a value below the documented minimum.
    return if (value < 0) 0 else value;
}
