# Project state — Delay, Multi-FX, and Synth Harness

Last updated: 2026-07-28
Status: continuity scaffold; detailed lane state must be populated and maintained by the next substantive runs.

## Purpose

This file is the durable continuity record for work spanning:

- delay pedal;
- multi-FX pedal;
- shared pedal platform;
- Synth Harness for users, developers, repairers, and small manufacturers;
- firmware/DSP, hardware, mechanics, verification, manufacturing, service, and productization.

It is not a replacement for requirements, decision logs, test evidence, BOMs, schematics, source manifests, or reports. It links their current state so a new chat or agent can recover the thread without reconstructing it from conversation history.

## Mandatory persistence rule

After every run in which any of the following occurs, this file or the nearest lane-specific `state.md` **must be updated before the run is considered complete**:

- a design decision is accepted, rejected, reversed, deferred, or moved to HOLD;
- firmware, DSP, hardware, mechanical, UI, test, BOM, cost, roadmap, or product-scope artifacts are created or modified;
- a risk, dependency, assumption, blocker, acceptance criterion, or next action materially changes;
- repository state, source revision, build status, measurement status, or validation evidence changes.

A report or chat answer alone is not durable project state.

If the active working folder is writable, update its lane-local `state.md`. If not, update this GitHub file. Never claim persistence unless the file was actually written and the resulting path/commit is reported.

A run with no material state change may leave the file unchanged, but the run output must state: `State update: not required — no decision or artifact state changed.`

## Update order

1. Inspect this file and any more-specific lane `state.md` before work.
2. Inspect the latest authoritative artifacts and repository SHA/dirty state.
3. Perform the work and independent critique.
4. Update affected decision/artifact/test records.
5. Update `state.md` last, so it reflects what actually exists.
6. Report the exact state file path and commit/hash or local artifact path.

## Required state entry

Each substantive run must add or refresh an entry containing:

```yaml
run:
  date: YYYY-MM-DD
  lane: shared | delay | multi-fx | synth-harness | verification | productization
  objective:
  repository_or_workspace:
  base_sha_or_snapshot:
  state_before:
  changes:
    decisions:
    artifacts_created_or_modified:
    tests_or_measurements:
  evidence:
    - path/url/commit/log
  risks_or_blockers:
  acceptance_status: PASS | PASS_WITH_GAPS | BLOCKED
  next_single_action:
  owner:
```

## Current lane index

| Lane | Current authority | Current state file | Notes |
|---|---|---|---|
| Shared platform | Repository files plus latest united engineering report and active instructions | `state.md` until a more-specific file exists | Common interfaces, reusable DSP/HAL/UI/test assets, platform decisions |
| Delay pedal | Latest delay architecture report, delay handoff context, delay AGENTS instructions | create `work_products/delay/state.md` or equivalent when that lane is materialized | Delay-centric; compact validation path plus full-featured pedal |
| Multi-FX pedal | Latest multi-FX report and multi-FX AGENTS instructions | create `work_products/multi_fx/state.md` or equivalent when that lane is materialized | Separate companion product; fixed-slot graph, not arbitrary everything-pedal |
| Synth Harness | Active project instructions and latest harness suitability work | create `work_products/synth_harness/state.md` or equivalent when that lane is materialized | Users, developers, repairers, small manufacturers; broader than production test |
| Verification/productization | Test plans, evidence logs, BOM/cost/manufacturing artifacts | create lane-local state when artifacts exist | Hardware tests, firmware profiling, fixtures, service, DFM, commercialization |

## Current cross-lane invariants

- Delay and multi-FX remain separate products.
- Shared infrastructure is preferred only when it does not degrade product identity, audio performance, ergonomics, serviceability, or validation clarity.
- Existing Daisy hardware is an alpha/reference baseline, not automatically a production platform.
- Real-time, hardware, mechanical, cost, and commercial claims require evidence at the appropriate gate.
- Facts, inferences, estimates, proposals, and decisions must remain distinguishable.
- Hardware testing by the user is a dependency for hardware acceptance, but it must not stop independent firmware, DSP, architecture, tooling, documentation, and test-harness work that can proceed beforehand.

## Decision log summary

No design decision is newly frozen by creation of this scaffold. The next substantive lane run must populate the current decisions from the latest authoritative artifacts rather than relying on this initial summary.

## Run history

### 2026-07-28 — shared continuity scaffold

- **Objective:** establish mandatory durable state persistence across lane-specific chats and repository work.
- **Change:** created root `state.md` and defined the completion gate for future runs.
- **Acceptance:** PASS.
- **Next single action:** each new lane chat reads this file first and creates or updates its nearest lane-specific `state.md` after its first substantive work package.
