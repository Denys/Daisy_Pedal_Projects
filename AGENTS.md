# Pedal and Synth Harness Workspace Rules

This file applies to the entire repository unless a nested `AGENTS.md` narrows implementation details. A nested file may not weaken evidence, lane isolation, state persistence, decision history, verification, licensing, or write-safety rules.

## Mandatory continuity gate

Before any substantive engineering or research response, read:

1. this `AGENTS.md`;
2. root `state.md`;
3. root `decision_log.md`;
4. the nearest lane-specific `state.md`, if present;
5. the active lane instructions and current artifacts.

Do not infer content from filenames or old chat summaries. If a required continuity file is missing or unreadable, do not make current-state claims. Report the blocker and the smallest recovery action.

Within this repository, accepted records in `decision_log.md` govern prior decisions; `state.md` is the current continuity index. Neither proves implementation, build, test, measurement, sourcing, or current external facts. Verify those against the named primary artifact or source.

## Active decision resolution

Follow `Supersedes` links before treating an ADR as current. For a given scope, the active decision is the latest applicable, non-superseded record in that chain. An immutable older heading such as `Accepted` does not make the old decision active. If the chain is missing, contradictory, or ambiguous, mark the matter `HOLD` and recover the evidence before substantive work.

## Product lanes

Keep these lanes distinct:

- **Delay pedal:** delay-centric product, with compact and full pedal variants.
- **Multi-FX pedal:** separate focused product using a bounded fixed-slot graph; not a second delay workstation or arbitrary everything-pedal.
- **Synth Harness:** reusable ecosystem for users, developers, repairers, and small manufacturers. It includes diagnostics, bring-up, validation, firmware update, preset/editor infrastructure, production test, repair, service, and developer tooling. It is not a third pedal.
- **Shared platform:** only interfaces and modules that do not degrade product identity, audio performance, ergonomics, serviceability, or validation clarity.

Every substantive run declares one primary lane. A shared-platform change must record separate impact on Delay, Multi-FX, and Synth Harness.

## Evidence and status

Use explicit evidence labels:

- `VERIFIED`: observed in the current source, datasheet, CAD, measurement, or executed test.
- `DERIVED`: calculated from verified inputs with method and assumptions.
- `PROPOSED`: recommended but not implemented or accepted.
- `ASSUMED`: working assumption required to proceed.
- `UNVERIFIED`: plausible, stale, inaccessible, or unsupported.
- `HOLD`: blocked pending discriminating evidence.
- `REJECTED`: excluded for the stated scope and evidence.

Never promote a proposal because it appeared in a prior report. Generated diagrams, README statements, filenames, and chat summaries are navigation or evidence leads, not proof.

## Language boundary

Use English for internal analysis, prompts, repository files, state, decision records, reports, specifications, diagrams, tables, and generated artifacts. Use Italian only for visible conversation with Denys. Preserve exact identifiers, code, quotations, filenames, and source-language titles.

All visible blocker, failure, completion, and rollover prose must also be Italian. Exact identifiers, filenames, paths, code, quotations, and machine values may remain unchanged.

## State persistence

Root `state.md` answers "where are we now?" It is append-preserving history plus a current lane index. Lane-local state may add detail but may not replace the root index.

After any material change, append root `state.md` and also append the affected lane state when one is maintained. Write referenced artifacts first, the decision log next, lane state next, and root state last. Preserve all earlier history.

Material changes include:

- decisions or alternatives;
- architecture or controlled interfaces;
- firmware, DSP, hardware, mechanics, UI, BOM, cost, test, roadmap, or product scope;
- assumptions, risks, blockers, acceptance criteria, evidence, validation, or next action;
- repository state or generated artifacts.

Each root entry records the date/run ID, primary lane/objective, base revision, prior state, active ADR IDs and decision statuses, artifacts, inspected evidence, test status (`PASS`, `FAIL`, `NOT_RUN`, or `COULD_NOT_RUN`), assumptions, risks, blockers, next single action, and HW/FW/mechanical/documentation impact.

Re-fetch every written path and report its commit/hash. If no material state changed, create no ceremonial entry and state that explicitly.

## Decision persistence

`decision_log.md` is append-only and ADR-style. It answers "why was this decision made?"

- Never delete, renumber, or silently rewrite a decision record.
- New accepted, rejected, deferred, or superseding decisions receive a new record.
- Reversing a decision requires `Supersedes`, old decision, new decision, reason, evidence, impact, and required validation.
- Corrections to factual metadata are appended as correction notes; they do not erase the original record.

## Completion gate

A substantive run is complete only when:

- authoritative continuity files were read;
- the requested artifact or analysis exists;
- evidence and validation status are explicit;
- decisions, next action, blockers, and generated artifacts are recorded;
- required state and decision updates were re-read and verified;
- Delay, Multi-FX, and Synth Harness were not conflated;
- no proposed or historical claim was presented as verified.

If required persistence did not succeed, provide the complete patch/content and end with:

`STATO: INCOMPLETO — persistenza dello stato mancante`

## Context rollover

If the host warns about context limits, the thread is compacted or loses material context, lane recall becomes contradictory, or current evidence can no longer be distinguished from historical output:

1. stop new engineering work;
2. use the `re-fresh` skill in standard mode;
3. rebuild the handoff from this file, root `state.md`, `decision_log.md`, lane state, current repository refs, and reachable sources;
4. resolve supersession chains and cite active ADR IDs;
5. label claims `Observed`, `Decision`, `Pending`, or `Unverified`;
6. include the next action, completion test, exact source identifiers/revisions, and obsolete or superseded paths to avoid;
7. create a unique immutable handoff named `work_products/handoffs/re-fresh-pedal-synth-harness-YYYY-MM-DDTHHMMSS-Europe-Zurich-<run-id>.md`; never overwrite an earlier handoff.

Never claim that a new chat was created unless the runtime confirms it. Otherwise instruct Denys in Italian to open a new chat in the same Project and paste the generated re-fresh prompt.

## Repository writes

Read-only inspection is allowed when relevant. External writes require the active user request or an explicit scheduled-task authorization. Preserve unrelated work, fetch before update, serialize writes to one path, avoid destructive/history-rewriting actions, and verify every claimed write.
