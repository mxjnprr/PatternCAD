# BMAD Method v6 - PatternCAD Project

This directory contains the BMAD Method implementation for the PatternCAD project.

## Directory Structure

```
bmad/
├── config.yaml              # Project configuration
├── agents/                  # Agent definitions
│   ├── architect.yaml       # System Architect agent
│   ├── dev.yaml            # Developer agent
│   ├── qa.yaml             # Quality Assurance agent
│   └── scrum-master.yaml   # Scrum Master agent
├── workflows/              # Workflow definitions
│   ├── dev-story.yaml      # Story implementation workflow
│   ├── review-story.yaml   # Story review and QA workflow
│   └── update-sprint.yaml  # Sprint status update workflow
├── prompts/                # Reusable prompt templates
│   ├── implement-story.md  # Template for story implementation requests
│   ├── review-code.md      # Template for code review requests
│   ├── test-story.md       # Template for story testing requests
│   └── sprint-status.md    # Template for sprint status requests
└── agent-overrides/        # Agent customizations (optional)
```

## Quick Start

### 1. Check Project Status
To see current sprint status:
```
Use sprint-status.md prompt template
Request: "Quick Status" or "Detailed Status"
```

### 2. Implement a Story
To implement a user story:
```
1. Use implement-story.md prompt template
2. Reference the story ID (e.g., story-001-01)
3. Agent will follow dev-story.yaml workflow
4. Story status will be updated in docs/sprint-status.yaml
```

### 3. Review Code
To review implemented code:
```
1. Use review-code.md prompt template
2. Specify review type (architecture, code quality, functionality, or all)
3. Agent will provide detailed feedback
```

### 4. Test a Story
To test a completed story:
```
1. Use test-story.md prompt template
2. Reference the story ID
3. Agent will follow review-story.yaml workflow
4. QA will sign off or report bugs
```

## Agents

### Architect
**Role:** System Architect
**Expertise:** Architecture design, technical decisions, design patterns
**Context:** docs/architecture-PatternCAD-2026-01-27.md
**Use for:** Architecture decisions, design reviews, technical guidance

### Dev
**Role:** Developer
**Expertise:** C++, Qt, implementation, testing
**Context:** Architecture, stories, sprint status
**Use for:** Implementing stories, writing code, creating tests

### QA
**Role:** Quality Assurance
**Expertise:** Testing, bug reporting, quality validation
**Context:** Stories, acceptance criteria, sprint status
**Use for:** Testing stories, finding bugs, validating quality

### Scrum Master
**Role:** Scrum Master
**Expertise:** Sprint management, progress tracking, facilitation
**Context:** Sprint status, workflow status, stories
**Use for:** Sprint planning, status updates, tracking progress

## Workflows

### dev-story
**Purpose:** Implement a user story from start to completion
**Agents:** dev, qa, scrum-master
**Steps:**
1. Read and understand story
2. Update status to in-progress
3. Plan implementation
4. Implement code
5. Write tests
6. Verify acceptance criteria
7. Request QA review
8. QA testing
9. Fix bugs (if needed)
10. Update status to completed

### review-story
**Purpose:** Review and quality-check a completed story
**Agents:** qa, dev, scrum-master, architect (optional)
**Steps:**
1. Review requirements
2. Create test plan
3. Functional testing
4. UI/UX testing
5. Performance testing
6. Cross-platform testing
7. Architecture review (optional)
8. Compile results
9. Sign off or reject

### update-sprint
**Purpose:** Update sprint status and track progress
**Agents:** scrum-master
**Steps:** Varies by update type:
- **status_change:** Update story status
- **sprint_planning:** Plan next sprint
- **sprint_review:** Review completed sprint
- **status_report:** Generate status report

## Key Files

### Configuration
- **bmad/config.yaml**: Project-level configuration
- **docs/bmm-workflow-status.yaml**: High-level workflow phase tracking
- **docs/sprint-status.yaml**: Detailed sprint and story tracking

### Documentation
- **docs/product-brief.md**: Product vision and goals
- **docs/prd-PatternCAD-2026-01-27.md**: Product requirements
- **docs/architecture-PatternCAD-2026-01-27.md**: System architecture
- **docs/stories/**: Individual story files organized by epic

## Sprint Tracking

The project uses **docs/sprint-status.yaml** as the single source of truth for sprint progress.

### Story Status Values
- **not-started**: Story hasn't been started
- **in-progress**: Developer is actively working
- **completed**: All acceptance criteria met, QA signed off

### Epic Status
Calculated automatically based on story completion:
- **not-started**: No stories started
- **in-progress**: Some stories started or completed
- **completed**: All stories completed

## Development Process

### Recommended Workflow

1. **Sprint Planning** (Scrum Master)
   - Review sprint-status.yaml
   - Select stories for sprint
   - Assign stories to developers
   - Update sprint metadata

2. **Story Implementation** (Developer)
   - Pick story from sprint
   - Update status to in-progress
   - Follow dev-story workflow
   - Implement, test, commit
   - Request QA review

3. **Story Review** (QA)
   - Follow review-story workflow
   - Test all acceptance criteria
   - Report bugs if found
   - Sign off when complete

4. **Status Updates** (Scrum Master)
   - Keep sprint-status.yaml current
   - Track progress daily
   - Report sprint health
   - Identify and resolve blockers

5. **Sprint Review** (Scrum Master)
   - Calculate metrics
   - Analyze performance
   - Create sprint report
   - Plan next sprint

## Story Implementation Phases

Follow this sequence for best results (from sprint-status.yaml):

**Phase 0: Foundation** (2-3 weeks)
- Basic window, canvas, undo system, build system

**Phase 1: Core Drawing** (3-4 weeks)
- All drawing tools, selection, basic editing

**Phase 2: Transformations** (2-3 weeks)
- Rotate, mirror, scale, align, distribute

**Phase 3: Parametric System** (4-5 weeks)
- Parameters, expressions, constraints, solver (most complex)

**Phase 4: Pattern Features** (4-5 weeks)
- Seam allowance, notches, grading

**Phase 5: Layout & Isolation** (3-4 weeks)
- Layout sheets, nesting, isolation mode

**Phase 6: Measurement** (2-3 weeks)
- Measurement tools, unit conversion

**Phase 7: File I/O** (5-6 weeks)
- Native format, DXF, SVG, PDF, auto-save

**Phase 8: Polish** (3-4 weeks)
- Bug fixes, optimization, documentation

**Total:** 30-40 weeks (7-9 months)

## Best Practices

### For Developers
- Always read the story file and architecture first
- Follow the Command pattern for undoable operations
- Target 60fps for interactive operations (<16ms)
- Write unit tests for all new code
- Update story status as you progress
- Commit frequently with clear messages

### For QA
- Test all acceptance criteria systematically
- Try to break the feature (adversarial testing)
- Test on both Linux and Windows
- Verify performance requirements
- Report bugs clearly with reproduction steps
- Only sign off when all criteria are met

### For Scrum Master
- Keep sprint-status.yaml always up-to-date
- Commit status changes immediately
- Monitor for blockers proactively
- Report sprint health regularly
- Follow recommended phase sequence
- Facilitate team communication

### For Architect
- Document all architectural decisions
- Reference architecture document in reviews
- Consider long-term implications
- Maintain consistency across codebase
- Ensure performance targets are achievable
- Balance simplicity with functionality

## Metrics

Track these metrics in sprint-status.yaml:

- **Total Stories:** 62 stories across 8 epics
- **Total Story Points:** 382 points estimated
- **Velocity:** Calculate as points completed per week
- **Completion %:** Stories completed / total stories
- **Epic Progress:** Track per-epic completion

## Support

For questions or issues with the BMAD Method implementation:

1. Check the relevant agent definition (bmad/agents/)
2. Review the workflow definition (bmad/workflows/)
3. Consult the architecture document
4. Check the sprint-status.yaml for current state

## Version

- **BMAD Method Version:** 6
- **Project:** PatternCAD
- **Project Type:** desktop-app
- **Project Level:** 3 (12-40 stories)

---

Last updated: 2026-01-28
