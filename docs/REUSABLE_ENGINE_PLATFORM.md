<!--
Umicom Framework
File: docs/REUSABLE_ENGINE_PLATFORM.md

PURPOSE:
Explain the reusable engine platform to beginners, including what works now,
what needs an adapter, and how a thin application should consume it.

AUTHOR AND ORGANISATION:
Sammy Hegab
Umicom Foundation

LICENCE:
MIT
-->

# Reusable Engine Platform

All engine contributions follow the beginner-friendly rules in
`CODE_COMMENTING_STANDARD.md`. New contracts explain ownership and valid use;
new functions explain their outcome and failure behaviour; important branches
explain the policy or safety rule they enforce.

## The simple idea

An engine is a reusable machine inside Umicom Framework. An application gives
the machine data and asks it to perform a job. The application should not copy
the engine code.

For example, a kitchen designer and a game can both use the same 3D scene
engine. They use different objects and screens, but the rules for owning a
scene, attaching children and changing transforms are the same.

`umicom/engine/engine.h` is the catalogue. It tells an application which
engines exist, what each engine can do and whether it is a contract, a working
foundation or an operational implementation.

## Current engines

| Engine | Current maturity | Working foundation |
|---|---|---|
| Game | Foundation | Stable entities, bounded worlds, 2D/3D/retro profiles and deterministic participant tick acknowledgement. |
| 3D rendering | Foundation | Toolkit-neutral scene nodes, parent-child safety, transforms, visibility and portable mesh/material resource IDs. |
| Chart and analytics | Operational | Chart panes, series, scales, indicators, drawings, annotations, streams, workspaces, safe data-to-pixel projection and a backend-neutral render scene. |
| Document generation | Foundation | Output plans, text/script/storyboard generation and adapters for binary document containers. |
| Media rendering | Foundation | Owned RGBA image surfaces and deterministic step/linear animation tracks. |
| Trading and matching | Operational | Order books, matching, brokerage, risk, surveillance, positions, replay and execution evidence. |
| Social community | Foundation | Membership, roles, channels, immutable message evidence, suspension and non-destructive moderation. |
| Data connectivity | Foundation | SQL and NoSQL connector families, copied driver descriptors and safe connection/query handles. |

## What “foundation” means

A foundation is real working code, but it is not the final product.

The 3D scene engine can safely own objects today. A graphics adapter is still
needed to draw those objects with a chosen graphics API. The document engine
can generate text today. A format adapter is still needed to create a PDF,
word-processing package or presentation package.

The catalogue reports that difference honestly. A new application can require
an operational engine and receive a clear “not ready” report when only a
foundation exists.

## Ownership rules

- Engines own their internal arrays and allocations.
- Public lookup functions copy records into caller-owned memory.
- Numeric identities are used instead of pointers into movable storage.
- Every capacity multiplication is checked before allocation.
- Removing a 3D parent is refused while a child still references it.
- A suspended community member keeps historical authorship but cannot post.
- Database profiles pass a local credential reference, never a password or API
  key value.
- Closing a database handle clears its callbacks to prevent accidental reuse.

These rules reduce dangling pointers, buffer overflows, accidental data loss
and secret leakage.

## How a thin application uses an engine

1. Include the engine’s public header.
2. Ask the catalogue whether the required maturity and capability exist.
3. Create the engine object with an explicit capacity.
4. Store application-specific names and business data in the application.
5. Send reusable records to the engine.
6. Render copied snapshots through the application’s frontend adapter.
7. Destroy the engine owner during application shutdown.

Studio’s Engine Explorer follows this model. It reads the Framework catalogue
and builds a view. It does not maintain a second list of engines.

## Publication workflow

The document engine will grow from individual output requests into source
ingestion, unique chapter manifests, ordered assembly, asset provenance, covers,
contents, previews, output adapters and build evidence. Authoring tools remain
replaceable processes behind safe argument and working-directory contracts.

## Interactive media and games

The game, 3D and media engines next need collision and spatial queries,
navigation, device-neutral input, camera visibility, asynchronous assets, audio
scenes and animation binding. These remain clean C contracts with explicit
ownership before a graphics or device backend is selected.

## External dependency governance

An external library is accepted only with a pinned revision, licence record,
security review, platform statement, update policy and isolation adapter. Public
Framework headers never expose third-party implementation types.

## Next implementation work

The next engine updates should add:

1. game component storage, deterministic system scheduling, save-state and
   transport adapters;
2. camera, lighting, material, mesh, spatial index and graphics-backend plans;
3. richer statistical pipelines, axis label formatting, interaction hit-testing and additional chart-rendering adapters;
4. tested PDF and open document-package adapters;
5. animation curves, tracks for vectors/colours and frame composition;
6. community relationships, feeds, privacy, reporting and federation policy;
7. production database drivers, result streaming, cancellation and connection
   health;
8. Framework UI view models for inspecting and controlling each engine.

Each addition should remain small, tested and useful on its own. A file should
not exist merely to increase a file count.
