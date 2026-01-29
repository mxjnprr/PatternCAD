# Prompt Template: Review Code

Use this template when asking an AI agent to review code for a story.

---

## Request Format

```
Please review the code for story {STORY_ID}.

**Story Information:**
- ID: {STORY_ID}
- Title: {STORY_TITLE}
- Epic: {EPIC_NAME}
- Status: {CURRENT_STATUS}

**Review Context:**
- Architecture document: docs/architecture-PatternCAD-2026-01-27.md
- Story file: docs/stories/{EPIC_ID}/{STORY_ID}.md
- Acceptance criteria: [Listed in story file]

**Review Checklist:**

### Architecture Compliance
- [ ] Follows architecture patterns
- [ ] Command pattern used for undoable operations
- [ ] Proper separation of concerns (UI/business logic/data)
- [ ] Uses Qt best practices and idioms
- [ ] Proper use of signals/slots for communication

### Code Quality
- [ ] Clean, readable code
- [ ] Meaningful variable/function names
- [ ] Appropriate comments for complex logic
- [ ] Error handling is implemented
- [ ] No obvious bugs or issues
- [ ] Follows C++ best practices

### Functionality
- [ ] All acceptance criteria met
- [ ] Feature works as specified
- [ ] Edge cases handled
- [ ] Error messages are clear
- [ ] Undo/redo works correctly

### Performance
- [ ] Interactive operations < 16ms (60fps target)
- [ ] Efficient algorithms used
- [ ] No obvious performance bottlenecks
- [ ] Memory usage is reasonable

### Testing
- [ ] Unit tests exist
- [ ] Tests cover main functionality
- [ ] Tests cover edge cases
- [ ] All tests pass

**Review Type:**
{REVIEW_TYPE: architecture | code_quality | functionality | all}

**Please provide:**
1. List of issues found (by severity: critical, high, medium, low)
2. Architectural concerns
3. Code quality suggestions
4. Performance observations
5. Overall assessment (approve/reject/needs_work)
```

---

## Variables to Replace

- `{STORY_ID}`: The story identifier
- `{STORY_TITLE}`: The story title
- `{EPIC_NAME}`: The epic name
- `{EPIC_ID}`: The epic ID
- `{CURRENT_STATUS}`: Current story status
- `{REVIEW_TYPE}`: Type of review needed

---

## Review Types

### Architecture Review
Focus on:
- Architectural compliance
- Design pattern usage
- Module boundaries
- Performance characteristics

### Code Quality Review
Focus on:
- Code readability
- Best practices
- Error handling
- Documentation

### Functionality Review
Focus on:
- Acceptance criteria
- Feature completeness
- Edge cases
- User experience

### Full Review
All of the above

---

## Example Usage

```
Please review the code for story story-001-03.

**Story Information:**
- ID: story-001-03
- Title: Select objects with click and marquee selection
- Epic: Core Drawing and Editing Tools
- Status: in-progress

**Review Context:**
- Architecture document: docs/architecture-PatternCAD-2026-01-27.md
- Story file: docs/stories/epic-001/story-001-03.md
- Acceptance criteria: [Listed in story file]

**Review Checklist:**
[Full checklist as above]

**Review Type:** all

**Please provide:**
1. List of issues found (by severity)
2. Architectural concerns
3. Code quality suggestions
4. Performance observations
5. Overall assessment
```

---

## Severity Guidelines

**Critical:**
- Architecture violations
- Data loss or corruption
- Crashes or severe bugs
- Security issues

**High:**
- Feature doesn't work as specified
- Major performance issues
- Undo/redo broken
- Memory leaks

**Medium:**
- Code quality issues
- Minor functionality gaps
- Inefficient but working code
- Missing error handling

**Low:**
- Code style inconsistencies
- Minor optimizations possible
- Documentation gaps
- Minor UX improvements
