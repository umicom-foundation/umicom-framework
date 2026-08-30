<!--
Umicom Framework
File: docs/PUBLIC_HEADER_GOVERNANCE.md

PURPOSE:
Explain public header comments, include guards and include-order safety to new
Umicom developers.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Public Header Governance

A public header is the front door to a reusable part of Umicom Framework. If
that door is unclear or broken, every application that uses it can fail.

This update gives Framework, Studio and Trader one enforceable public-header
standard. It does not rename functions or change data structures.

## What went wrong in Studio

Framework has two different experience catalogue families:

- `umicom/application/experience_catalogue.h` exposes application definitions,
  including product panels, features and layouts;
- `umicom/application/experience/catalogue.h` exposes learning, standard and
  focus experience profiles.

Both headers accidentally used the same include guard. A guard is a unique
preprocessor name that prevents one header being included twice. Because the
two different files shared a name, including the profile catalogue first made
the compiler skip the application-definition catalogue. Studio then called a
function whose declaration had been hidden, producing the implicit-declaration
and pointer-conversion errors.

The profile header now uses
`UMICOM_APPLICATION_EXPERIENCE_PROFILE_CATALOGUE_H`. The existing application
catalogue keeps `UMICOM_APPLICATION_EXPERIENCE_CATALOGUE_H` for compatibility.

## The combined portfolio header

Applications that need both catalogue families can include:

```c
#include "umicom/application/experience_portfolio.h"
```

The name makes the intention clear and prevents a developer from having to
remember two very similar paths.

## The required comment

Every public header starts with:

```text
Umicom product or module name
File path
Purpose
Author and organisation
Licence
```

This is not decoration. A new developer should be able to open any public
contract and understand where it belongs before reading C declarations.

## Automated audit

The `UmicomPublicHeaderAudit.cmake` script checks every public header for:

- a file description;
- a purpose;
- the author and organisation;
- the licence;
- a matching `#ifndef` and `#define` guard;
- a guard that is not used by another header.

Framework registers the audit for its SDK. The integrated Studio and Trader
builds register the same audit for their product headers. One audit covers an
entire include tree, so thousands of separate compilation targets are not
created.

## Adding a header

1. Copy the standard comment from a nearby public header.
2. Give the guard a name derived from the complete include path.
3. Include the direct dependencies used by the declarations.
4. Keep toolkit objects out of portable Framework contracts.
5. Add a coexistence test when a header has a similar name or compatibility
   relationship with another API.

Do not work around a collision by changing include order. The same pair of
headers may be included in a different order by another application.

## Compatibility

Changing an include guard does not change the compiled ABI. No public function,
structure or enumeration was removed. The unused `contracts (2).h` file was an
older duplicate of the active `ui/contracts.h`, contained no unique declaration
and had no references, so it was removed as superseded duplication.
