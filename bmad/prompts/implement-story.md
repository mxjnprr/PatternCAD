# Prompt Template: Implement Story

Use this template when asking an AI agent to implement a user story.

---

## Request Format

```
I need you to implement story {STORY_ID} from the PatternCAD project.

**Story Details:**
- ID: {STORY_ID}
- Title: {STORY_TITLE}
- Epic: {EPIC_NAME}
- Priority: {PRIORITY}

**Context:**
- Architecture document: docs/architecture-PatternCAD-2026-01-27.md
- Story file: docs/stories/{EPIC_ID}/{STORY_ID}.md
- Sprint status: docs/sprint-status.yaml

**Requirements:**
Please follow the dev-story workflow:
1. Read and understand the story requirements
2. Review relevant architecture sections
3. Plan the implementation
4. Implement the code following Qt best practices
5. Create Command class for undo/redo
6. Write unit tests
7. Verify all acceptance criteria
8. Update story status to in-progress

**Expected Deliverables:**
- Implemented C++ code
- Command class for undo/redo
- Unit tests
- Code that meets all acceptance criteria
- Updated story file with status

**Guidelines:**
- Follow the architecture document strictly
- Use Command pattern for all undoable operations
- Target 60fps for interactive operations
- Write clean, documented code
- Handle errors gracefully
- Test on both Linux and Windows if possible
```

---

## Variables to Replace

- `{STORY_ID}`: The story identifier (e.g., story-001-01)
- `{STORY_TITLE}`: The story title
- `{EPIC_NAME}`: The epic this story belongs to
- `{EPIC_ID}`: The epic ID (e.g., epic-001)
- `{PRIORITY}`: Story priority level

---

## Example Usage

```
I need you to implement story story-001-01 from the PatternCAD project.

**Story Details:**
- ID: story-001-01
- Title: Draw basic geometric shapes
- Epic: Core Drawing and Editing Tools
- Priority: must-have

**Context:**
- Architecture document: docs/architecture-PatternCAD-2026-01-27.md
- Story file: docs/stories/epic-001/story-001-01.md
- Sprint status: docs/sprint-status.yaml

**Requirements:**
Please follow the dev-story workflow:
1. Read and understand the story requirements
2. Review relevant architecture sections
3. Plan the implementation
4. Implement the code following Qt best practices
5. Create Command class for undo/redo
6. Write unit tests
7. Verify all acceptance criteria
8. Update story status to in-progress

**Expected Deliverables:**
- Implemented C++ code
- Command class for undo/redo
- Unit tests
- Code that meets all acceptance criteria
- Updated story file with status

**Guidelines:**
- Follow the architecture document strictly
- Use Command pattern for all undoable operations
- Target 60fps for interactive operations
- Write clean, documented code
- Handle errors gracefully
- Test on both Linux and Windows if possible
```

---

## Tips

- Always reference the specific story file
- Point to relevant architecture sections
- Be clear about acceptance criteria
- Emphasize following the architecture
- Request status updates as work progresses
