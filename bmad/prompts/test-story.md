# Prompt Template: Test Story

Use this template when asking an AI agent to test a story.

---

## Request Format

```
Please test story {STORY_ID} following the review-story workflow.

**Story Information:**
- ID: {STORY_ID}
- Title: {STORY_TITLE}
- Epic: {EPIC_NAME}
- Developer: {DEVELOPER_NAME}
- Status: Ready for QA

**Testing Context:**
- Story file: docs/stories/{EPIC_ID}/{STORY_ID}.md
- Acceptance criteria: [Listed in story file]
- Technical notes: [From story file]

**Testing Requirements:**

### 1. Functional Testing
Test each acceptance criterion:
{LIST_ACCEPTANCE_CRITERIA}

For each criterion:
- Test happy path
- Test with invalid inputs
- Test edge cases
- Test boundary conditions
- Verify error handling

### 2. UI/UX Testing
- Verify UI is intuitive and responsive
- Test keyboard shortcuts
- Test context menus
- Check visual feedback (selection, hover, etc.)
- Test with different window sizes
- Look for visual glitches

### 3. Performance Testing
- Measure operation response times
- Verify 60fps for interactive operations (<16ms per frame)
- Test with realistic data volumes
- Check memory usage
- Look for memory leaks
- {SPECIFIC_PERFORMANCE_REQUIREMENTS}

### 4. Undo/Redo Testing
- Test undo after each operation
- Test redo after undo
- Verify undo stack works correctly
- Test undo/redo with multiple operations

### 5. Cross-Platform Testing
- Test on Linux (if available)
- Test on Windows (if available)
- Verify keyboard shortcuts work on both
- Check for platform-specific issues

### 6. Edge Cases
{LIST_EDGE_CASES}

**Expected Output:**

Please provide:
1. **Test Summary**: Pass/Fail decision with rationale
2. **Test Results**: Results for each acceptance criterion
3. **Bug Reports**: Detailed reports for any issues found
   - Bug title
   - Severity (Critical/High/Medium/Low)
   - Steps to reproduce
   - Expected vs actual result
   - Platform (Linux/Windows/Both)
4. **Performance Results**: Timing measurements
5. **Recommendations**: Any suggestions for improvement

**Testing Tools:**
- Manual testing
- Performance profiling if needed
- Memory profiler if needed
```

---

## Variables to Replace

- `{STORY_ID}`: The story identifier
- `{STORY_TITLE}`: The story title
- `{EPIC_NAME}`: The epic name
- `{EPIC_ID}`: The epic ID
- `{DEVELOPER_NAME}`: Developer who implemented
- `{LIST_ACCEPTANCE_CRITERIA}`: Bulleted list from story file
- `{SPECIFIC_PERFORMANCE_REQUIREMENTS}`: Any specific perf requirements
- `{LIST_EDGE_CASES}`: Specific edge cases to test

---

## Bug Report Template

When reporting bugs, use this format:

```markdown
## Bug: {Short Description}

**Story ID:** {STORY_ID}
**Severity:** {Critical|High|Medium|Low}
**Platform:** {Linux|Windows|Both}

**Steps to Reproduce:**
1. Step one
2. Step two
3. Step three

**Expected Result:**
What should happen

**Actual Result:**
What actually happens

**Additional Information:**
- Screenshots (if applicable)
- Error messages
- Console output
- Performance data
```

---

## Example Usage

```
Please test story story-001-03 following the review-story workflow.

**Story Information:**
- ID: story-001-03
- Title: Select objects with click and marquee selection
- Epic: Core Drawing and Editing Tools
- Developer: dev-agent
- Status: Ready for QA

**Testing Context:**
- Story file: docs/stories/epic-001/story-001-03.md
- Acceptance criteria: [Listed in story file]

**Testing Requirements:**

### 1. Functional Testing
Test each acceptance criterion:
- User can click on any object to select it
- Selected objects show visual feedback
- Shift+Click adds objects to selection
- Ctrl+Click toggles object selection
- Marquee selection by dragging rectangle
- Ctrl+A selects all objects
- Escape deselects all objects

For each criterion:
- Test happy path
- Test with invalid inputs
- Test edge cases
- Test boundary conditions
- Verify error handling

### 2. UI/UX Testing
[As above]

### 3. Performance Testing
- Selection should respond in < 16ms
- Marquee selection should be smooth at 60fps
- Test with 100+ objects
[etc.]

### 4. Undo/Redo Testing
- Test undo after selection
- Verify undo restores previous selection state

### 5. Cross-Platform Testing
[As above]

### 6. Edge Cases
- Selection when no objects exist
- Selection of locked objects
- Selection across layers
- Selection of overlapping objects

**Expected Output:**
[As above]
```

---

## Pass/Fail Criteria

### Pass
- All acceptance criteria met
- No critical or high-severity bugs
- Performance requirements met
- Works on both platforms
- Undo/redo works correctly

### Fail
- Any acceptance criterion not met
- Critical or high-severity bugs found
- Performance requirements not met
- Feature doesn't work on one platform

### Needs Work
- Minor bugs found (medium/low severity)
- Performance close but not quite meeting targets
- Some edge cases not handled
