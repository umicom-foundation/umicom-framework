# Catalogue Contracts and Test Diagnostics

This guide explains how Umicom Framework keeps application features, panels,
layouts, capabilities and reusable components in agreement. It also explains
how to read a failed native test without mistaking an assertion for a memory
error.

## Why catalogue changes must be complete

An application experience describes what a person can see and use. A panel
such as an editor, profiler or research window names a Framework capability.
The reusable component catalogue then maps that capability to a shared UI
building block.

The relationship is:

```text
application feature or panel
        -> Framework capability
        -> reusable component
        -> thin application presentation
```

Adding only the application panel leaves the chain incomplete. The panel may
appear in a layout definition, but productisation and production-readiness
checks cannot prove that Framework knows how to provide it.

When adding a new shared surface:

1. Add or reuse one capability in the Framework capability catalogue.
2. Add or reuse one component that provides the panel capability.
3. Include the component record in the component catalogue.
4. Add the panel and feature to the canonical application experience.
5. Add the panel to one or more layouts when it should be visible by default.
6. Test the relationship by capability and stable identifier, not by a copied
   total from an earlier catalogue.

This order keeps applications thin. The application chooses and arranges
Framework components; it does not create another private implementation.

## Stable tests instead of stale totals

A literal such as `panel_count == 263` becomes incorrect as soon as a valid new
panel is added. It tests an old inventory size rather than a useful product
rule.

Prefer relationships such as:

- every canonical panel has one surface projection;
- every projected surface has a reusable component;
- every required capability exists in the capability catalogue;
- covered surfaces equal total surfaces when the closure is complete;
- a named layout contains its essential semantic panels;
- application profile counts agree with the live canonical layout catalogue.

Exact totals remain appropriate only when the number itself is part of a
published contract. In that unusual case, the source should explain why the
count must not change.

## Understanding Windows exit code `0xc0000409`

This exit code means the process ended through a fast-fail path. It can be
reported for a genuine stack or memory safety failure, but a failed C runtime
assertion can produce the same visible code. The code alone therefore does not
identify the cause.

Umicom tests that use `UMI_TEST_REQUIRE` print:

- the source file;
- the line number;
- the exact expression that failed.

The test then returns a normal failure status. This turns an opaque process
termination into a useful message for a new contributor.

When a test still ends unexpectedly:

1. Run that one test with failure output enabled.
2. Read the first reported failed expression.
3. If no expression is printed, inspect stack size and memory ownership.
4. Move large temporary bounded catalogues to the heap with `calloc`.
5. Free each temporary allocation after the consumer has copied the data it
   needs.
6. Check pointer ownership before freeing a source object.

## Safe temporary catalogue ownership

Whole-suite catalogues can contain hundreds of records. They should not be
placed on a small native thread stack merely for a short calculation.

The production runtime therefore allocates its temporary surface portfolio on
the heap. Panel bindings copy each surface projection by value. Pointers inside
the copied projection refer to immutable Framework catalogue records, not to
the temporary container, so the container can be freed after binding.

This ownership rule should be documented whenever a future function copies a
record that contains pointers:

- say whether the record is copied or borrowed;
- say who owns the pointed-to data;
- say how long that data remains valid;
- free only objects whose consumers no longer borrow their storage.

## Review checklist

Before committing a catalogue update, confirm:

- every panel capability exists;
- every panel capability has a reusable component;
- component records are included exactly once;
- layouts refer only to known panel identifiers;
- profile counts come from the canonical catalogue;
- tests verify identifiers and relationships rather than historical totals;
- large temporary portfolio objects use explicit heap ownership;
- test failures print the expression that failed;
- new functions and decisions have beginner-friendly comments.

Following this checklist lets the suite grow like a set of compatible building
blocks while keeping failures understandable to contributors of every skill
level.
