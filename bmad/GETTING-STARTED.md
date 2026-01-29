# Getting Started with BMAD Method - PatternCAD

Welcome to the BMAD Method implementation for PatternCAD! This guide will help you get started quickly.

---

## What is BMAD Method?

BMAD Method v6 is an AI-assisted software development framework that uses specialized agents and workflows to manage the entire development lifecycle from planning through implementation.

For PatternCAD, we have:
- **4 specialized agents** (Architect, Developer, QA, Scrum Master)
- **3 workflow definitions** (dev-story, review-story, update-sprint)
- **4 prompt templates** (for common tasks)
- **62 user stories** organized into 8 epics
- **Complete project documentation** (PRD, Architecture, Stories)

---

## Quick Navigation

| Document | Purpose |
|----------|---------|
| **README.md** | Main documentation and overview |
| **INDEX.md** | Complete index and quick reference |
| **EXAMPLES.md** | Practical usage examples |
| **STRUCTURE.txt** | Visual structure overview |
| **GETTING-STARTED.md** | This document - your first steps |

---

## Your First 5 Minutes

### 1. Understand the Project State

Current state:
- **Project**: PatternCAD (Desktop CAD app for pattern design)
- **Phase**: Phase 0 - Foundation Sprint
- **Total Stories**: 62 stories, 382 story points
- **Duration**: Estimated 30-40 weeks (7-9 months)
- **Technology**: C++ with Qt framework, cross-platform (Linux + Windows)

### 2. Review Key Documents

Read these in order:

1. **Product Vision**
   - File: `/mnt/Data/Air MKG/Commun/Air MKG/Code/PatternCAD/docs/product-brief.md`
   - What: High-level product goals and vision

2. **Requirements**
   - File: `/mnt/Data/Air MKG/Commun/Air MKG/Code/PatternCAD/docs/prd-PatternCAD-2026-01-27.md`
   - What: Detailed product requirements

3. **Architecture**
   - File: `/mnt/Data/Air MKG/Commun/Air MKG/Code/PatternCAD/docs/architecture-PatternCAD-2026-01-27.md`
   - What: System architecture and technical decisions

4. **Sprint Status**
   - File: `/mnt/Data/Air MKG/Commun/Air MKG/Code/PatternCAD/docs/sprint-status.yaml`
   - What: Current sprint progress and all stories

### 3. Understand the Agents

| Agent | When to Use | Key Expertise |
|-------|------------|---------------|
| **architect** | Architecture decisions, design reviews | System design, patterns, trade-offs |
| **dev** | Implementing stories, writing code | C++, Qt, implementation |
| **qa** | Testing stories, finding bugs | Testing, quality validation |
| **scrum-master** | Sprint planning, status updates | Progress tracking, facilitation |

Each agent has a YAML file in `bmad/agents/` with full role definitions and system prompts.

---

## Your First Task: Check Sprint Status

Let's start by checking where we are in the project.

### Using the scrum-master Agent

**Prompt** (copy and use with an AI):
```
I'm using the BMAD Method for PatternCAD. Please act as the scrum-master agent.

Context files:
- bmad/agents/scrum-master.yaml (your role definition)
- docs/sprint-status.yaml (current sprint status)

Task: Provide a quick status report

Please tell me:
1. Current sprint number and goal
2. How many stories are completed/in-progress/not-started
3. Which epic we should focus on next
4. Recommended first story to implement

Format: Brief, suitable for getting started.
```

This will give you a clear picture of the current state.

---

## Your Second Task: Implement Your First Story

### Step 1: Choose a Story

For your first story, we recommend:
- **Story ID**: story-008-01
- **Title**: Create main window with menu bar and status bar
- **Epic**: User Interface and Cross-Platform Foundation
- **Complexity**: Moderate (5 points)
- **Why first**: Foundation piece needed for all other UI work

### Step 2: Read the Story

File: `/mnt/Data/Air MKG/Commun/Air MKG/Code/PatternCAD/docs/stories/epic-008/story-008-01.md`

Read the entire story file, paying attention to:
- Acceptance criteria (what must be implemented)
- Technical notes (implementation hints)
- Dependencies (what this story needs)

### Step 3: Review Architecture

File: `/mnt/Data/Air MKG/Commun/Air MKG/Code/PatternCAD/docs/architecture-PatternCAD-2026-01-27.md`

Look for:
- "User Interface" section
- Qt best practices
- Architecture patterns to follow

### Step 4: Request Implementation

**Use the prompt template**: `bmad/prompts/implement-story.md`

**Prompt** (copy and customize):
```
I need you to implement story story-008-01 from the PatternCAD project.

Context:
- Role: Act as the 'dev' agent (see bmad/agents/dev.yaml)
- Architecture: docs/architecture-PatternCAD-2026-01-27.md
- Story file: docs/stories/epic-008/story-008-01.md
- Sprint status: docs/sprint-status.yaml

Story Details:
- ID: story-008-01
- Title: Create main window with menu bar and status bar
- Epic: User Interface and Cross-Platform Foundation
- Priority: must-have

Requirements:
Follow the dev-story workflow (bmad/workflows/dev-story.yaml):
1. Read and understand the story requirements
2. Update sprint status to in-progress
3. Plan the implementation
4. Implement the code following Qt best practices
5. Write unit tests
6. Verify all acceptance criteria
7. Commit code with clear messages

Expected Deliverables:
- Implemented C++ code (MainWindow class)
- Qt UI files if needed
- Unit tests
- Code that meets all acceptance criteria
- Updated sprint-status.yaml

Guidelines:
- Follow the architecture document strictly
- Use Qt best practices (QMainWindow, signals/slots)
- Write clean, documented code
- Handle errors gracefully
- Target cross-platform (Linux + Windows)
```

The dev agent will then guide you through the implementation step by step.

---

## Understanding the Workflow

### The dev-story Workflow

When you implement a story, you follow this workflow (`bmad/workflows/dev-story.yaml`):

```
1. Read & Understand Story
   ↓
2. Update Status → in-progress (scrum-master)
   ↓
3. Plan Implementation (dev)
   ↓
4. Implement Code (dev)
   ↓
5. Write Tests (dev)
   ↓
6. Verify Acceptance Criteria (dev)
   ↓
7. Commit Code (dev)
   ↓
8. Request QA Review (dev)
   ↓
9. QA Testing (qa)
   ↓
10. Fix Bugs if needed (dev) ←─┐
   ↓                            │
11. Update Status → completed (scrum-master)
   ↓
12. Update Story File
```

If bugs are found in step 9, you loop back to step 10 to fix them.

---

## Common Commands

### Check Sprint Status
```
Agent: scrum-master
Template: bmad/prompts/sprint-status.md
Type: Quick Status or Detailed Status
```

### Implement a Story
```
Agent: dev
Template: bmad/prompts/implement-story.md
Workflow: bmad/workflows/dev-story.yaml
Input: story_id
```

### Test a Story
```
Agent: qa
Template: bmad/prompts/test-story.md
Workflow: bmad/workflows/review-story.yaml
Input: story_id
```

### Review Code
```
Agent: architect
Template: bmad/prompts/review-code.md
Input: story_id, review_type
```

### Update Sprint
```
Agent: scrum-master
Workflow: bmad/workflows/update-sprint.yaml
Types: status_change, sprint_planning, sprint_review, status_report
```

---

## File Structure to Know

```
bmad/
├── agents/           → Agent role definitions
├── workflows/        → Workflow process definitions
├── prompts/          → Reusable prompt templates
├── README.md         → Main documentation
├── INDEX.md          → Quick reference
├── EXAMPLES.md       → Practical examples
└── GETTING-STARTED.md → This file

docs/
├── product-brief.md           → Product vision
├── prd-PatternCAD-*.md       → Requirements
├── architecture-PatternCAD-*.md → Architecture
├── sprint-status.yaml         → Sprint tracking (⚠️ update often!)
├── bmm-workflow-status.yaml   → Phase tracking
└── stories/                   → All user stories
    ├── epic-001/ through epic-008/
```

---

## Tips for Success

### For Your First Sprint

1. **Start Small**: Begin with Phase 0 stories (foundation)
2. **Read Everything**: Story file + acceptance criteria + technical notes + architecture
3. **Follow Patterns**: Use the architecture document as your guide
4. **Update Frequently**: Keep sprint-status.yaml current
5. **Ask Questions**: Use the architect agent for guidance
6. **Test Early**: Don't wait until the end to test
7. **Commit Often**: Small, frequent commits with clear messages

### Working with Agents

**Architect Agent**:
- Use for: Technical decisions, design patterns, architecture questions
- Ask: "How should I design X?" or "What pattern should I use for Y?"

**Developer Agent**:
- Use for: Implementation, code writing, debugging
- Ask: "Implement story X" or "Help me debug this issue"

**QA Agent**:
- Use for: Testing, bug finding, quality validation
- Ask: "Test story X" or "Verify acceptance criteria"

**Scrum Master Agent**:
- Use for: Sprint planning, status updates, progress tracking
- Ask: "What's our sprint status?" or "Plan next sprint"

### Key Success Metrics

Track these:
- ✅ Stories completed per sprint
- ✅ Story points completed (velocity)
- ✅ Bugs found vs fixed
- ✅ Time per story (cycle time)
- ✅ QA pass rate

---

## Recommended Learning Path

### Day 1: Orientation
- [ ] Read this guide (GETTING-STARTED.md)
- [ ] Read README.md
- [ ] Skim INDEX.md
- [ ] Review sprint-status.yaml
- [ ] Check current sprint status with scrum-master

### Day 2-3: First Story
- [ ] Pick story-008-01
- [ ] Read story file completely
- [ ] Review relevant architecture sections
- [ ] Implement using dev agent and dev-story workflow
- [ ] Test with qa agent
- [ ] Update sprint status

### Week 1: Foundation
- [ ] Complete Phase 0 stories (4 stories)
- [ ] Get comfortable with agents and workflows
- [ ] Learn to update sprint-status.yaml
- [ ] Establish development rhythm

### Week 2+: Production
- [ ] Move to Phase 1 (Core Drawing)
- [ ] Follow recommended phase sequence
- [ ] Track velocity and adjust estimates
- [ ] Maintain quality with QA reviews

---

## Common Pitfalls to Avoid

1. **Not Reading Architecture**: Always check architecture document first
2. **Skipping Tests**: Write tests as you code, not after
3. **Not Updating Status**: Keep sprint-status.yaml current
4. **Ignoring Dependencies**: Check story dependencies before starting
5. **Not Using Workflows**: Follow defined workflows for consistency
6. **Rushing QA**: Don't skip QA review to save time
7. **Not Asking Questions**: Use architect agent when unsure

---

## Getting Help

### If You're Stuck

1. **Check Documentation**
   - README.md for overview
   - INDEX.md for quick reference
   - EXAMPLES.md for practical examples
   - Architecture document for technical guidance

2. **Use the Right Agent**
   - Technical question → architect
   - Implementation help → dev
   - Testing question → qa
   - Process question → scrum-master

3. **Review Story File**
   - Re-read acceptance criteria
   - Check technical notes
   - Review dependencies

4. **Look at Examples**
   - EXAMPLES.md has 7 detailed examples
   - Shows common scenarios and solutions

---

## Next Steps

After reading this guide:

1. **Check Sprint Status**
   - Use scrum-master agent
   - Understand current state

2. **Read Architecture**
   - File: docs/architecture-PatternCAD-2026-01-27.md
   - Understand system design

3. **Pick First Story**
   - Recommended: story-008-01
   - Read story file completely

4. **Start Implementation**
   - Use dev agent
   - Follow dev-story workflow
   - Update sprint status

5. **Test and Complete**
   - Use qa agent
   - Get QA sign-off
   - Mark story completed

---

## Quick Reference Card

**Check Status**
```
scrum-master + sprint-status.md → Quick/Detailed status
```

**Implement Story**
```
dev + implement-story.md + dev-story.yaml → Implemented code
```

**Test Story**
```
qa + test-story.md + review-story.yaml → QA sign-off
```

**Review Code**
```
architect + review-code.md → Architecture feedback
```

**Update Sprint**
```
scrum-master + update-sprint.yaml → Updated sprint-status.yaml
```

---

## You're Ready!

You now have everything you need to start developing PatternCAD using the BMAD Method.

**Your first action**: Check sprint status with scrum-master agent.

**Your first story**: Implement story-008-01 with dev agent.

**Remember**: Read the story file, follow the architecture, test thoroughly, and update sprint status regularly.

Good luck! 🚀

---

**Need more detail?** See README.md, INDEX.md, or EXAMPLES.md.

**Last Updated**: 2026-01-28
