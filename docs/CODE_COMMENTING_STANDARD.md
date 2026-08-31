<!--
Umicom Framework
File: docs/CODE_COMMENTING_STANDARD.md

PURPOSE:
Explain how Umicom code comments help new developers understand contracts,
functions, decisions, ownership and failure behaviour without prior knowledge.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Code Commenting Standard

Umicom code should be understandable to a new developer who is still learning
C. A comment must explain **why the code exists**, **what rule it enforces** and
**how another part of the program should use it**.

## Required comments

Every new public contract must explain:

- what the structure, enumeration, callback or constant represents;
- who owns any memory and how long borrowed values remain valid;
- which values are valid and what happens when input is invalid;
- whether the contract is toolkit-neutral, platform-neutral or adapter-owned.

Every new function must have a comment immediately above it that explains:

- the outcome the function provides;
- the important inputs and outputs;
- ownership, allocation and cleanup responsibilities;
- the important failure states a caller should handle.

Every new condition, loop or switch must have a nearby comment when it applies
a rule, protects memory, selects policy, changes state or handles failure. The
comment should explain the decision. It should not simply translate the C code
into English.

## Example

```c
/* Reject a zero capacity before multiplication so allocation size cannot wrap. */
if (capacity == 0U || capacity > SIZE_MAX / sizeof(*items)) {
    return UMI_STATUS_INVALID_ARGUMENT;
}
```

The comment explains the safety reason. A weak comment such as `Check capacity`
does not help a learner understand why the check is important.

## Review checklist

Before a change is ready for review, confirm that:

1. a new file contains the standard Umicom header;
2. every public contract and function explains its purpose;
3. ownership and borrowed-pointer lifetimes are explicit;
4. every important branch explains the rule it enforces;
5. capacity, overflow, null, state and cleanup paths are explained;
6. comments describe current behaviour and contain no batch or version label;
7. comments use Umicom language and do not depend on another product's name.

Clear names and comments work together. A comment is not a reason to shorten a
meaningful variable name, and a meaningful variable name is not a reason to
hide an important safety or business rule.
