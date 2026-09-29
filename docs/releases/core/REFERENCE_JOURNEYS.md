# Umicom Notes Core acceptance journeys

Status: **acceptance specifications, not executed GUI tests in this checkpoint**. These journeys qualify Framework, not Studio. Use an existing qualified compiler/editor until Studio's own campaign becomes active.

| ID | User action | Observable acceptance result | Lead batch |
|---|---|---|---|
| J01 | Install the SDK and compile the reference app from another directory | Public installed headers and libraries suffice; no private source dependency | FW-17 |
| J02 | Open two windows and two distinct UTF-8 documents | Actions affect only the selected document; identities remain distinct | FW-10, FW-12 |
| J03 | Edit, undo, redo, save, cancel Save As, then reopen | Cancel changes no destination; successful save/reopen preserves exact supported text | FW-13 |
| J04 | Start an index; request cancellation; close while it finishes | UI remains responsive; no callbacks access a destroyed owner | FW-05 |
| J05 | End the process while a practice document is dirty | Reopen distinguishes saved data from recoverable draft; never claims unsaved work was saved | FW-13 |
| J06 | Change saved data from a second valid writer | Conflict is visible; stale writer cannot overwrite accepted state silently | FW-07 |
| J07 | Save a split layout and restart | Component/context association restores without swapping documents | FW-12 |
| J08 | Make a file or required provider unavailable | Error is actionable, correctly scoped and does not masquerade as a successful empty result | FW-06, FW-16 |
| J09 | Install to a Unicode path and launch by icon with unrelated CWD | Runtime, icons, schemas and dependencies resolve without developer environment | FW-18 |
| J10 | Upgrade, repair and remove in a separate test installation | Ownership rules preserve user documents and identify incomplete maintenance | FW-18 |
| J11 | Use keyboard and declared accessibility/DPI profiles | All advertised actions remain reachable and labelled; measured budgets pass | FW-19 |
| J12 | A beginner follows the release guide on a clean supported host | The documented commands, controls and expected results match the installed product | FW-20 |

Every result must identify native host, configuration, candidate/package, test inputs and log/record. No fixture, metadata flag or the new memory-only lesson can stand in for these observations.
