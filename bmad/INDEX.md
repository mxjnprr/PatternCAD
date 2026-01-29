# BMAD Method - Complete Index

Quick reference guide for all BMAD Method components in PatternCAD.

---

## Agents

### architect.yaml
**Role:** System Architect
**Expertise:** Architecture design, design patterns, technical decisions
**Key Responsibilities:**
- Design system architecture
- Make architectural decisions
- Review architecture compliance
- Guide technical implementations
- Evaluate trade-offs

**When to Use:**
- Designing new subsystems
- Making major technical decisions
- Reviewing complex implementations
- Resolving architectural questions
- Planning system evolution

**Key Context Files:**
- docs/architecture-PatternCAD-2026-01-27.md
- docs/prd-PatternCAD-2026-01-27.md

---

### dev.yaml
**Role:** Developer
**Expertise:** C++ programming, Qt framework, implementation
**Key Responsibilities:**
- Implement user stories
- Write clean, maintainable code
- Follow architecture guidelines
- Write unit tests
- Update story status

**When to Use:**
- Implementing stories
- Writing code
- Creating tests
- Debugging issues
- Refactoring

**Key Context Files:**
- docs/architecture-PatternCAD-2026-01-27.md
- docs/sprint-status.yaml
- docs/stories/**/*.md

---

### qa.yaml
**Role:** Quality Assurance
**Expertise:** Testing, bug reporting, quality validation
**Key Responsibilities:**
- Test stories against acceptance criteria
- Find and report bugs
- Validate performance requirements
- Test cross-platform compatibility
- Sign off on completed stories

**When to Use:**
- Testing implemented stories
- Validating acceptance criteria
- Finding bugs
- Performance testing
- Cross-platform verification

**Key Context Files:**
- docs/sprint-status.yaml
- docs/stories/**/*.md
- docs/architecture-PatternCAD-2026-01-27.md

---

### scrum-master.yaml
**Role:** Scrum Master
**Expertise:** Sprint management, progress tracking, facilitation
**Key Responsibilities:**
- Plan and manage sprints
- Track story progress
- Update sprint-status.yaml
- Identify and remove blockers
- Report sprint health

**When to Use:**
- Sprint planning
- Status updates
- Progress tracking
- Risk management
- Team facilitation

**Key Context Files:**
- docs/sprint-status.yaml
- docs/bmm-workflow-status.yaml
- docs/stories/**/*.md

---

## Workflows

### dev-story.yaml
**Purpose:** Complete implementation of a user story
**Duration:** Variable (depends on story complexity)
**Primary Agent:** dev
**Supporting Agents:** qa, scrum-master

**Workflow Steps:**
1. Read and understand story
2. Update status to in-progress
3. Plan implementation
4. Implement code
5. Write unit tests
6. Verify acceptance criteria
7. Commit code
8. Request QA review
9. QA testing
10. Fix bugs (if needed)
11. Update status to completed
12. Update story file

**Triggers:**
- Story assigned to developer
- Sprint planning selects story
- User requests story implementation

**Outputs:**
- Implemented code
- Updated story file
- Unit tests
- Git commits

---

### review-story.yaml
**Purpose:** Quality assurance review of completed story
**Duration:** 0.5-2 days (depends on story complexity)
**Primary Agent:** qa
**Supporting Agents:** dev, scrum-master, architect (optional)

**Workflow Steps:**
1. Review story requirements
2. Create test plan
3. Functional testing
4. UI/UX testing
5. Performance testing
6. Cross-platform testing
7. Architecture review (optional)
8. Compile test results
9. Sign off or reject
10. Update story status

**Triggers:**
- Developer marks story ready for review
- Code is committed and pushed
- QA review requested

**Outputs:**
- Test results
- Bug reports (if issues found)
- QA sign-off or rejection
- Updated story status

---

### update-sprint.yaml
**Purpose:** Update and track sprint progress
**Duration:** Minutes to hours
**Primary Agent:** scrum-master

**Workflow Types:**

#### status_change
Update when story status changes
- Steps: Identify change → Update YAML → Update metrics → Commit

#### sprint_planning
Plan the next sprint
- Steps: Review current → Select stories → Update metadata → Assign stories → Commit

#### sprint_review
Review completed sprint
- Steps: Calculate metrics → Analyze performance → Update epics → Create report → Commit

#### status_report
Generate current status
- Steps: Gather data → Calculate metrics → Assess risks → Generate report

**Triggers:**
- Story status changes
- Sprint planning session
- Sprint review session
- Status report requested

**Outputs:**
- Updated sprint-status.yaml
- Progress reports
- Risk assessments
- Git commits

---

## Prompt Templates

### implement-story.md
**Purpose:** Request story implementation
**Variables:** story_id, story_title, epic_name, epic_id, priority
**Use Case:** When asking AI to implement a story
**Agent:** dev

**Template Sections:**
- Story details
- Context files
- Requirements
- Expected deliverables
- Implementation guidelines

---

### review-code.md
**Purpose:** Request code review
**Variables:** story_id, story_title, epic_name, review_type
**Use Case:** When asking AI to review code
**Agent:** architect or qa

**Review Types:**
- Architecture review
- Code quality review
- Functionality review
- Full review (all of above)

**Template Sections:**
- Story information
- Review context
- Review checklist
- Expected output

---

### test-story.md
**Purpose:** Request story testing
**Variables:** story_id, story_title, developer_name, acceptance_criteria
**Use Case:** When asking AI to test a story
**Agent:** qa

**Template Sections:**
- Story information
- Testing requirements
- Functional testing
- UI/UX testing
- Performance testing
- Undo/redo testing
- Cross-platform testing
- Edge cases

---

### sprint-status.md
**Purpose:** Request sprint status
**Variables:** status_type
**Use Case:** When asking AI for sprint information
**Agent:** scrum-master

**Status Types:**
- Quick status (daily standup)
- Detailed status (sprint review)
- Epic status (planning)
- Story status (specific story)
- Risk assessment (blockers)

---

## Quick Commands

### Check Sprint Status
```
Use: sprint-status.md template
Agent: scrum-master
Type: Quick Status or Detailed Status
```

### Implement a Story
```
Use: implement-story.md template
Agent: dev
Input: story_id (e.g., story-001-01)
Workflow: dev-story.yaml
```

### Review Code
```
Use: review-code.md template
Agent: architect or qa
Input: story_id, review_type
```

### Test a Story
```
Use: test-story.md template
Agent: qa
Input: story_id
Workflow: review-story.yaml
```

### Update Status
```
Workflow: update-sprint.yaml
Agent: scrum-master
Type: status_change, sprint_planning, sprint_review, or status_report
```

---

## File Locations

### Configuration
- `bmad/config.yaml` - Project configuration
- `docs/bmm-workflow-status.yaml` - High-level phase tracking
- `docs/sprint-status.yaml` - Sprint and story tracking

### Documentation
- `docs/product-brief.md` - Product vision
- `docs/prd-PatternCAD-2026-01-27.md` - Requirements
- `docs/architecture-PatternCAD-2026-01-27.md` - Architecture

### Stories
- `docs/stories/epic-001/` - Core Drawing and Editing
- `docs/stories/epic-002/` - Parametric Design
- `docs/stories/epic-003/` - Transformations
- `docs/stories/epic-004/` - Pattern Features
- `docs/stories/epic-005/` - Layout and Nesting
- `docs/stories/epic-006/` - Measurement
- `docs/stories/epic-007/` - File Operations
- `docs/stories/epic-008/` - User Interface

---

## Epic Summary

| Epic | Name | Stories | Points | Priority |
|------|------|---------|--------|----------|
| epic-001 | Core Drawing | 7 | 37 | must-have |
| epic-002 | Parametric System | 9 | 62 | must-have |
| epic-003 | Transformations | 5 | 21 | must-have |
| epic-004 | Pattern Features | 7 | 39 | must-have |
| epic-005 | Layout & Nesting | 6 | 38 | must-have |
| epic-006 | Measurement | 4 | 16 | must-have |
| epic-007 | File Operations | 11 | 101 | must-have |
| epic-008 | UI Foundation | 12 | 68 | must-have |
| **Total** | **All Epics** | **62** | **382** | |

---

## Implementation Phases

| Phase | Name | Duration | Epics | Goal |
|-------|------|----------|-------|------|
| 0 | Foundation | 2-3 weeks | epic-008 (partial) | Build system, window, canvas |
| 1 | Core Drawing | 3-4 weeks | epic-001 | All drawing tools |
| 2 | Transformations | 2-3 weeks | epic-003 | Rotate, mirror, scale |
| 3 | Parametric | 4-5 weeks | epic-002 | Parameters, solver |
| 4 | Pattern Features | 4-5 weeks | epic-004 | Seam, notches, grading |
| 5 | Layout | 3-4 weeks | epic-005 | Layout, nesting, isolation |
| 6 | Measurement | 2-3 weeks | epic-006 | Measurement tools |
| 7 | File I/O | 5-6 weeks | epic-007 | DXF, SVG, PDF, native |
| 8 | Polish | 3-4 weeks | - | Bug fixes, optimization |
| **Total** | | **30-40 weeks** | | **7-9 months** |

---

## Agent Collaboration Matrix

| Scenario | Primary Agent | Supporting Agents | Workflow |
|----------|---------------|-------------------|----------|
| Implement story | dev | scrum-master, qa | dev-story |
| Test story | qa | dev, scrum-master | review-story |
| Architecture decision | architect | dev, scrum-master | (ad-hoc) |
| Sprint planning | scrum-master | dev, qa, architect | update-sprint |
| Bug fix | dev | qa, scrum-master | dev-story (subset) |
| Status update | scrum-master | - | update-sprint |
| Code review | architect | dev | (ad-hoc) |
| Performance issue | architect | dev, qa | (ad-hoc) |

---

## Status Values Reference

### Story Status
- `not-started` - Story hasn't been started yet
- `in-progress` - Developer is actively working on it
- `completed` - All acceptance criteria met, QA signed off

### Epic Status
- `not-started` - No stories started in this epic
- `in-progress` - Some stories started or completed
- `completed` - All stories in epic completed

### Bug Severity
- `critical` - Crashes, data loss, core feature broken
- `high` - Feature doesn't work as specified, major issue
- `medium` - Partial functionality, workaround exists
- `low` - Minor visual issue, cosmetic problem

---

## Key Metrics

| Metric | Description | Source |
|--------|-------------|--------|
| Total Stories | 62 | sprint-status.yaml |
| Total Points | 382 | sprint-status.yaml |
| Stories Completed | Variable | sprint-status.yaml metrics |
| Points Completed | Variable | sprint-status.yaml metrics |
| Velocity | Points/week | Calculated |
| Completion % | (completed/total) × 100 | Calculated |
| Epic Progress | Per-epic completion | sprint-status.yaml |

---

## Common Tasks

### Starting a New Sprint
1. Agent: scrum-master
2. Workflow: update-sprint.yaml (sprint_planning)
3. Select stories based on dependencies and priority
4. Assign stories to developers
5. Set sprint goal and dates
6. Commit updated sprint-status.yaml

### Implementing Your First Story
1. Read this index and README.md
2. Review docs/architecture-PatternCAD-2026-01-27.md
3. Pick story from sprint (start with story-008-01)
4. Use implement-story.md prompt template
5. Follow dev-story.yaml workflow
6. Update sprint-status.yaml when done

### Checking Project Health
1. Agent: scrum-master
2. Use sprint-status.md prompt template
3. Request "Detailed Status"
4. Review metrics, blockers, risks
5. Take action on blockers

### Getting Unstuck
1. Check agent definitions for guidance
2. Review architecture document
3. Consult story acceptance criteria
4. Ask architect agent for technical guidance
5. Check sprint-status.yaml for dependencies

---

**Last Updated:** 2026-01-28
**BMAD Method Version:** 6
**Project:** PatternCAD
