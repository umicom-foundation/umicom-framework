/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: templates/compiler/polyglot/src/rules.rs
 *
 * PURPOSE:
 *   Provide reusable rules source for projects created with Umicom tooling.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

// Umicom polyglot Rust unit | Sammy Hegab | Umicom Foundation | MIT

/// Clamp a C caller's signed value to the non-negative range.
#[unsafe(no_mangle)]
pub extern "C" fn umicom_rule(value: i32) -> i32 {
    // The explicit rule demonstrates a stable result across the language boundary.
    value.max(0)
}
