# Prompt Template: Sprint Status Request

Use this template when requesting sprint status information.

---

## Request Format

```
Please provide the current sprint status for PatternCAD.

**Status Type:** {STATUS_TYPE}

**Context:**
- Sprint status file: docs/sprint-status.yaml
- Workflow status: docs/bmm-workflow-status.yaml
- Project: PatternCAD

**Required Information:**

{STATUS_REQUIREMENTS}

**Format:**
Please format the response as a clear, organized report with:
- Summary at the top
- Detailed metrics
- Risks and blockers highlighted
- Actionable recommendations if applicable
```

---

## Status Types

### Quick Status
Just the essentials:
```
**Required Information:**
- Current sprint number and goal
- Stories completed / in-progress / not-started
- Story points completed / total
- Any critical blockers
```

### Detailed Status
Full sprint analysis:
```
**Required Information:**
- Sprint metadata (number, goal, dates)
- Story counts by status (completed/in-progress/not-started)
- Story points by status
- Completion percentage
- Velocity (points per week)
- Epic-level progress
- Stories by epic with status
- Blocked stories with details
- Risks and concerns
- Projected completion date
- Recommendations
```

### Epic Status
Focus on epic progress:
```
**Required Information:**
- List all epics
- For each epic:
  - Status (not-started/in-progress/completed)
  - Stories completed / total
  - Story points completed / total
  - Completion percentage
  - Any blocked stories
- Epic dependencies
- Recommended next epic to start
```

### Story Status
Details on specific stories:
```
**Required Information:**
- Story ID: {STORY_ID}
- Current status
- Assigned to
- Acceptance criteria completion
- Dependencies
- Blockers (if any)
- Estimated vs actual effort
- Related stories
```

### Risk Assessment
Focus on risks and blockers:
```
**Required Information:**
- All blocked stories
- Stories behind schedule
- Dependency issues
- Team capacity concerns
- Technical risks
- Mitigation suggestions
```

---

## Example Usages

### Quick Status
```
Please provide the current sprint status for PatternCAD.

**Status Type:** Quick Status

**Context:**
- Sprint status file: docs/sprint-status.yaml
- Project: PatternCAD

**Required Information:**
- Current sprint number and goal
- Stories completed / in-progress / not-started
- Story points completed / total
- Any critical blockers

**Format:**
Brief summary suitable for daily standup.
```

### Detailed Status
```
Please provide the current sprint status for PatternCAD.

**Status Type:** Detailed Status

**Context:**
- Sprint status file: docs/sprint-status.yaml
- Workflow status: docs/bmm-workflow-status.yaml

**Required Information:**
- Sprint metadata (number, goal, dates)
- Story counts and points by status
- Completion percentage and velocity
- Epic-level progress
- Blocked stories with details
- Risks and concerns
- Projected completion date
- Recommendations

**Format:**
Comprehensive report suitable for sprint review.
```

### Epic Status
```
Please provide epic-level status for PatternCAD.

**Status Type:** Epic Status

**Context:**
- Sprint status file: docs/sprint-status.yaml

**Required Information:**
- List all epics with completion status
- Stories and points for each epic
- Epic dependencies
- Recommended next epic to start

**Format:**
Epic-focused view for planning purposes.
```

### Specific Story
```
Please provide status for story {STORY_ID}.

**Status Type:** Story Status

**Context:**
- Sprint status file: docs/sprint-status.yaml
- Story file: docs/stories/{EPIC_ID}/{STORY_ID}.md

**Required Information:**
- Story-001-03 current status
- Assigned developer
- Acceptance criteria status
- Any blockers
- Estimated vs actual effort

**Format:**
Detailed story-level information.
```

---

## Expected Report Format

### Quick Status Example
```markdown
# Sprint Status - Quick View

**Sprint 1**: Foundation Sprint
**Progress**: 2 completed, 1 in-progress, 4 not-started
**Points**: 10 / 37 completed (27%)
**Blockers**: None
```

### Detailed Status Example
```markdown
# Sprint Status Report

## Summary
- **Sprint**: 1 - Foundation Sprint
- **Duration**: Week 1-3 (Jan 27 - Feb 17)
- **Progress**: 2 completed, 1 in-progress, 4 not-started
- **Story Points**: 10 / 37 completed (27%)
- **Velocity**: 5 points/week (current)
- **Projected Completion**: Week 3 (on track)

## Epic Progress
### Epic-001: Core Drawing (27% complete)
- Completed: 2 stories, 10 points
- In Progress: 1 story, 5 points
- Not Started: 4 stories, 22 points

## In-Progress Stories
- story-001-03: Select objects (dev-agent, started 2 days ago)

## Blocked Stories
None

## Risks
- None at this time

## Recommendations
- Continue current trajectory
- Start story-001-04 once 001-03 is complete
```

---

## Variables to Replace

- `{STATUS_TYPE}`: Type of status report needed
- `{STATUS_REQUIREMENTS}`: Specific information requirements
- `{STORY_ID}`: Specific story ID (for story status)
- `{EPIC_ID}`: Epic ID (for epic-specific queries)
