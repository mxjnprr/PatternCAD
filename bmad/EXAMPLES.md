# BMAD Method - Usage Examples

Practical examples for using the BMAD Method with PatternCAD.

---

## Example 1: Implementing Your First Story

### Scenario
You want to implement story-008-01 (Create main window).

### Step-by-Step

**1. Check Sprint Status**
```
Agent: scrum-master
Prompt template: sprint-status.md

Request:
"Please provide the current sprint status for PatternCAD.
Status Type: Quick Status
Show: sprint number, goal, and available stories to work on."
```

**2. Request Story Implementation**
```
Agent: dev
Prompt template: implement-story.md

Request:
"I need you to implement story story-008-01 from the PatternCAD project.

Story Details:
- ID: story-008-01
- Title: Create main window with menu bar and status bar
- Epic: User Interface and Cross-Platform Foundation
- Priority: must-have

Context:
- Architecture document: docs/architecture-PatternCAD-2026-01-27.md
- Story file: docs/stories/epic-008/story-008-01.md
- Sprint status: docs/sprint-status.yaml

Requirements:
Please follow the dev-story workflow:
1. Read and understand the story requirements
2. Review relevant architecture sections
3. Plan the implementation
4. Implement the code following Qt best practices
5. Create Command class for undo/redo if needed
6. Write unit tests
7. Verify all acceptance criteria
8. Update story status to in-progress

Expected Deliverables:
- Implemented C++ code
- Unit tests
- Code that meets all acceptance criteria
- Updated story file with status

Guidelines:
- Follow the architecture document strictly
- Use Qt best practices
- Write clean, documented code
- Handle errors gracefully"
```

**3. Monitor Progress**
The dev agent will:
- Update sprint-status.yaml to "in-progress"
- Implement the MainWindow class
- Create necessary UI components
- Write tests
- Commit code with clear messages

**4. Request QA Review**
```
Agent: qa
Prompt template: test-story.md

Request:
"Please test story story-008-01 following the review-story workflow.

Story Information:
- ID: story-008-01
- Title: Create main window with menu bar and status bar
- Epic: User Interface and Cross-Platform Foundation
- Developer: dev-agent
- Status: Ready for QA

Testing Requirements:
[Include full testing checklist from template]"
```

**5. Complete Story**
After QA sign-off:
```
Agent: scrum-master
Workflow: update-sprint.yaml

Request:
"Update story story-008-01 to completed status.
- Status: completed
- Actual effort: 5 points
- QA signed off: yes"
```

---

## Example 2: Sprint Planning

### Scenario
You want to plan Sprint 1 (Foundation Sprint).

### Process

**1. Review Current State**
```
Agent: scrum-master
Prompt:
"Review docs/sprint-status.yaml and docs/bmm-workflow-status.yaml.
Provide sprint 0 summary and recommend stories for sprint 1."
```

**2. Select Stories**
Based on Phase 0 recommendation:
- story-008-01: Main window
- story-008-02: Canvas with pan/zoom
- story-008-08: Undo/redo system
- story-008-11: Cross-platform build

**3. Update Sprint**
```
Agent: scrum-master
Workflow: update-sprint.yaml (sprint_planning)

Request:
"Plan Sprint 1 for PatternCAD:

Sprint Details:
- Sprint number: 1
- Sprint goal: Foundation Sprint - Build basic application infrastructure
- Duration: 3 weeks
- Start date: 2026-01-28

Selected Stories:
- story-008-01 (5 points) - assign to dev1
- story-008-02 (5 points) - assign to dev1
- story-008-08 (8 points) - assign to dev2
- story-008-11 (13 points) - assign to dev2

Total: 4 stories, 31 points

Please update sprint-status.yaml with this plan."
```

---

## Example 3: Code Review

### Scenario
You want architect to review the implementation of story-001-03.

### Process

**1. Request Architecture Review**
```
Agent: architect
Prompt template: review-code.md

Request:
"Please review the code for story story-001-03.

Story Information:
- ID: story-001-03
- Title: Select objects with click and marquee selection
- Epic: Core Drawing and Editing Tools
- Status: in-progress

Review Context:
- Architecture document: docs/architecture-PatternCAD-2026-01-27.md
- Story file: docs/stories/epic-001/story-001-03.md

Review Type: architecture

Please focus on:
1. Architecture compliance (Command pattern, separation of concerns)
2. Qt best practices (signals/slots, memory management)
3. Performance considerations (60fps target)
4. Code organization and structure

Provide:
1. List of issues by severity
2. Architectural concerns
3. Recommendations"
```

**2. Review Response Example**
The architect will provide:
- ✅ Good: Command pattern correctly used for SelectObjectsCommand
- ✅ Good: Proper use of signals/slots for selection changes
- ⚠️ Medium: Selection algorithm could be optimized for many objects
- ⚠️ Low: Consider extracting selection logic to separate SelectionManager class

**3. Address Issues**
```
Agent: dev
Request:
"Based on architect review feedback for story-001-03:
1. Optimize selection algorithm for performance
2. Consider creating SelectionManager class
Please implement these improvements."
```

---

## Example 4: Bug Found During QA

### Scenario
QA finds a bug while testing story-001-04.

### Process

**1. QA Reports Bug**
```
Agent: qa
Output from test-story.md workflow:

Bug Report:
## Bug: Spacebar grab-and-move doesn't work on locked layers

**Story ID:** story-001-04
**Severity:** High
**Platform:** Both (Linux and Windows)

**Steps to Reproduce:**
1. Create a pattern piece on layer 1
2. Lock layer 1
3. Hover over the piece
4. Press spacebar
5. Try to move the piece

**Expected Result:**
Locked pieces should not be movable

**Actual Result:**
Piece moves even though layer is locked

**Additional Information:**
The lock check is missing in GrabMoveInteraction class
```

**2. Developer Fixes Bug**
```
Agent: dev
Workflow: dev-story.yaml (step 10 - Fix Bugs)

Request:
"Fix bug reported in story-001-04:
Bug: Spacebar grab-and-move doesn't work on locked layers

Required fix:
- Add layer lock check in GrabMoveInteraction
- Prevent grab-and-move on locked layers
- Show feedback to user (cursor change or message)
- Test the fix

Please implement the fix and re-test."
```

**3. Re-test**
```
Agent: qa
Request:
"Re-test story-001-04 focusing on the layer lock bug fix.
Verify that locked layers cannot be moved with spacebar grab."
```

**4. Sign Off**
After successful re-test:
```
Agent: qa
Output:
"✅ Bug fix verified. Layer lock now properly prevents grab-and-move.
✅ All acceptance criteria met.
QA Sign-off: Approved"
```

---

## Example 5: Sprint Status Report

### Scenario
You want a detailed sprint status for sprint review meeting.

### Process

**Request Detailed Status**
```
Agent: scrum-master
Prompt template: sprint-status.md

Request:
"Please provide the current sprint status for PatternCAD.

Status Type: Detailed Status

Required Information:
- Sprint metadata (number, goal, dates)
- Story counts and points by status
- Completion percentage and velocity
- Epic-level progress
- Blocked stories with details
- Risks and concerns
- Projected completion date
- Recommendations

Format: Comprehensive report suitable for sprint review."
```

**Example Response**
```markdown
# Sprint 1 Status Report - PatternCAD

## Summary
- **Sprint**: 1 - Foundation Sprint
- **Duration**: Week 1-3 (Jan 28 - Feb 18, 2026)
- **Progress**: 3 completed, 1 in-progress, 0 not-started
- **Story Points**: 26 / 31 completed (84%)
- **Velocity**: 13 points/week (average)
- **Projected Completion**: Week 3 (on track)

## Epic Progress

### Epic-008: User Interface and Cross-Platform Foundation
- **Status**: in-progress (33% complete)
- **Completed**: 3 stories, 18 points
  - story-008-01: Main window (5 points)
  - story-008-02: Canvas with pan/zoom (5 points)
  - story-008-08: Undo/redo system (8 points)
- **In Progress**: 1 story, 13 points
  - story-008-11: Cross-platform build (13 points, 80% complete)
- **Remaining**: 8 stories, 50 points

## In-Progress Stories

### story-008-11: Cross-platform build and packaging
- **Assigned to**: dev2
- **Started**: 5 days ago
- **Progress**: 80% complete
- **Status**: CMakeLists.txt done, Linux AppImage working, Windows NSIS installer in progress
- **Blockers**: None
- **ETA**: 2 days

## Completed Stories

1. ✅ story-008-01: Main window (completed 2 days ago, 5 points)
2. ✅ story-008-02: Canvas (completed 4 days ago, 5 points)
3. ✅ story-008-08: Undo/redo (completed 6 days ago, 8 points)

## Metrics

- **Stories Completed**: 3 / 4 (75%)
- **Points Completed**: 18 / 31 (58%)
- **Average Story Cycle Time**: 3 days
- **Velocity**: 13 points/week
- **Quality**: 0 bugs reported, all QA sign-offs passed

## Risks and Concerns

✅ **No Major Risks**

Minor observations:
- story-008-11 is taking longer than estimated (13 points estimated, ~16 actual)
- Windows packaging is slightly more complex than anticipated
- Consider adding 20% buffer for cross-platform stories

## Projected Completion

- **Sprint 1 End Date**: Feb 18, 2026
- **Current Pace**: On track to complete all 4 stories
- **Confidence**: High (95%)

## Recommendations

1. ✅ Continue current trajectory - sprint is healthy
2. Consider starting epic-001 stories in sprint 2
3. story-001-01 through story-001-03 recommended for next sprint
4. Estimated sprint 2 capacity: 35-40 points (based on current velocity)

## Next Steps

1. Complete story-008-11 (2 days remaining)
2. Sprint review meeting on Feb 18
3. Sprint 2 planning on Feb 19
4. Begin epic-001 (Core Drawing) in sprint 2
```

---

## Example 6: Handling a Blocker

### Scenario
A story is blocked by a technical issue.

### Process

**1. Developer Reports Blocker**
```
Agent: dev
Context: Working on story-002-08 (Implement constraint solver)

Report:
"Story story-002-08 is blocked. Issue:
- Constraint solver requires Eigen library for linear algebra
- Eigen not yet added to build system
- Cannot proceed without it

Blocker: Missing dependency (Eigen library)
Impact: Cannot implement Newton-Raphson solver
Resolution needed: Add Eigen to CMakeLists.txt and vcpkg"
```

**2. Scrum Master Documents Blocker**
```
Agent: scrum-master
Workflow: update-sprint.yaml

Request:
"Update sprint-status.yaml for story-002-08:
- Status: in-progress (blocked)
- Add to technical_notes: 'BLOCKED: Requires Eigen library to be added to build system'
- Blocker severity: High
- Action needed: Add Eigen dependency"
```

**3. Architect Provides Solution**
```
Agent: architect

Request:
"Story-002-08 is blocked on missing Eigen library.
Please provide guidance on:
1. How to add Eigen to the build system
2. Which Eigen modules we need
3. Any Qt compatibility considerations
4. Build configuration changes needed"

Response:
"To add Eigen:
1. Add to vcpkg.json: 'eigen3'
2. Update CMakeLists.txt: find_package(Eigen3 REQUIRED)
3. Link: target_link_libraries(patterncad PRIVATE Eigen3::Eigen)
4. Include: #include <Eigen/Dense>
5. Use Eigen 3.4+ (header-only, Qt compatible)
6. Test on both Linux and Windows"
```

**4. Developer Resolves Blocker**
```
Agent: dev

Request:
"Implement the Eigen integration as specified by architect:
1. Update vcpkg.json
2. Update CMakeLists.txt
3. Test build on Linux and Windows
4. Document in architecture if needed
5. Unblock story-002-08"
```

**5. Resume Story**
```
Agent: scrum-master

Request:
"Update sprint-status.yaml for story-002-08:
- Remove blocker note
- Status: in-progress (unblocked)
- Resume implementation"
```

---

## Example 7: Architecture Decision

### Scenario
Need to make a decision about how to implement seam allowance offset algorithm.

### Process

**1. Developer Asks Question**
```
Agent: dev
Context: Planning story-004-01 (Add seam allowance automatically)

Question to architect:
"For story-004-01, I need to implement offset algorithm for seam allowance.

Two options:
1. Use Clipper library (dedicated offset algorithm, fast, proven)
2. Use Boost.Geometry (already using Boost, more features, but heavier)

Considerations:
- Performance: need to offset complex curves in <200ms
- Dependencies: minimize external dependencies
- Maintainability: which is easier to maintain?
- Features needed: miter/round/bevel corner handling

What do you recommend?"
```

**2. Architect Evaluates Options**
```
Agent: architect
Prompt template: (ad-hoc architectural decision)

Response:
"Architecture Decision: Seam Allowance Offset Algorithm

## Recommendation: Use Clipper Library

### Rationale:

**Performance** ✅
- Clipper is specifically designed for polygon offsetting
- Benchmarks show <50ms for typical pattern pieces
- Well-optimized for this use case

**Dependencies** ✅
- Single-purpose library, small footprint (~100KB)
- Header-only option available
- No conflicts with Qt

**Features** ✅
- Supports all corner types (miter, round, bevel)
- Handles self-intersections correctly
- Robust for complex polygons

**Maintainability** ✅
- Well-documented, stable API
- Active maintenance
- Used in many CAD applications

**Integration**:
- Add to vcpkg.json: 'clipper'
- Include: #include <clipper/clipper.hpp>
- Create OffsetAlgorithm wrapper class
- Keep interface generic (can swap later if needed)

### Decision:
Use Clipper library for seam allowance offsetting.
Document this decision in architecture.md under 'Pattern Features' section.
```

**3. Update Architecture**
```
Agent: architect

Action:
"Update docs/architecture-PatternCAD-2026-01-27.md:

Add to 'Pattern Features / Seam Allowance' section:

**Offset Algorithm**: Clipper Library
- Rationale: Fast, robust, dedicated offset algorithm
- Performance: <50ms for typical patterns
- Features: Miter, round, bevel corners
- Integration: Wrapped in OffsetAlgorithm class
- Decided: 2026-01-28

This keeps implementation details flexible while documenting the decision."
```

**4. Developer Implements**
```
Agent: dev

Proceed with implementation:
"Implement story-004-01 using Clipper library as decided:
1. Add Clipper to build system
2. Create OffsetAlgorithm wrapper class
3. Implement miter, round, bevel corner options
4. Integrate with SeamAllowance class
5. Test performance (<200ms requirement)
6. Follow architecture pattern"
```

---

## Tips for Success

### For Developers
- Always read story file completely before starting
- Check dependencies in sprint-status.yaml
- Follow architecture document patterns
- Ask architect for guidance on complex decisions
- Update sprint status regularly
- Commit frequently with clear messages

### For QA
- Create comprehensive test plans
- Test systematically through all acceptance criteria
- Try to break the feature (think adversarially)
- Document bugs clearly with reproduction steps
- Test on both platforms before sign-off

### For Scrum Master
- Keep sprint-status.yaml always current
- Monitor for blockers proactively
- Report status regularly
- Facilitate communication between agents
- Follow recommended phase sequence

### For Architect
- Document all major decisions
- Consider long-term implications
- Balance simplicity with functionality
- Keep architecture document updated
- Review critical or complex implementations

---

## Common Patterns

### Pattern 1: Story Implementation Loop
```
1. dev: Implement story
2. dev: Self-test acceptance criteria
3. dev: Request QA review
4. qa: Test story
5. If bugs: Return to step 1
6. If pass: scrum-master marks complete
```

### Pattern 2: Sprint Cycle
```
1. scrum-master: Sprint planning (select stories)
2. dev + qa: Implement and test stories
3. scrum-master: Track progress daily
4. scrum-master: Sprint review (metrics, report)
5. Repeat for next sprint
```

### Pattern 3: Blocker Resolution
```
1. dev: Encounter blocker
2. dev: Report blocker
3. scrum-master: Document in sprint-status.yaml
4. architect: Provide solution (if technical)
5. dev: Implement solution
6. scrum-master: Remove blocker
7. dev: Resume story
```

---

**Last Updated:** 2026-01-28
