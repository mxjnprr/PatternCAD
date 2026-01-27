# Contributing to PatternCAD

Thank you for considering contributing to PatternCAD! This document provides guidelines for contributing to the project.

## Development Process

PatternCAD follows the **BMAD Method** for structured software development:

1. **Product Brief** → Requirements gathering
2. **PRD** → Detailed requirements documentation
3. **Architecture** → System design and technical decisions
4. **Sprint Planning** → Breaking work into stories and epics
5. **Implementation** → Actual development work

## Getting Started

### 1. Find a Story

Check the [Sprint Status](docs/sprint-status.yaml) for available stories:

```bash
cat docs/sprint-status.yaml
```

Look for stories with `status: "not-started"` and no assignment.

### 2. Read the Story File

Each story has a detailed markdown file in `docs/stories/`:

```bash
cat docs/stories/epic-XXX/story-XXX-XX.md
```

The story file includes:
- Description and context
- Acceptance criteria
- Technical notes and implementation guidance
- Dependencies
- Definition of done

### 3. Check Dependencies

Before starting a story, ensure all dependencies are completed:
- Review the "Dependencies" section in the story file
- Check sprint-status.yaml to verify dependency status

### 4. Create a Feature Branch

```bash
git checkout -b story-XXX-XX-short-description
```

Branch naming convention: `story-{epic}-{number}-{brief-description}`

Examples:
- `story-001-01-basic-shapes`
- `story-008-02-canvas-pan-zoom`

## Development Guidelines

### Code Style

- **C++ Standard**: C++17
- **Naming Conventions**:
  - Classes: PascalCase (`GeometryObject`)
  - Methods: camelCase (`boundingRect()`)
  - Member variables: m_ prefix (`m_position`)
  - Static members: s_ prefix (`s_instance`)
- **Formatting**: Use consistent indentation (4 spaces, no tabs)
- **Comments**: Document public APIs with Doxygen-style comments

### Qt Conventions

- Inherit from `QObject` for signal/slot support (add `Q_OBJECT` macro)
- Use Qt types where appropriate (`QString`, `QPointF`, etc.)
- Connect signals/slots using new-style syntax (function pointers)
- Emit signals for state changes

### File Organization

```cpp
// MyClass.h
#ifndef PATTERNCAD_MYCLASS_H
#define PATTERNCAD_MYCLASS_H

namespace PatternCAD {

class MyClass {
    // Class definition
};

} // namespace PatternCAD

#endif // PATTERNCAD_MYCLASS_H
```

### Testing

- Write unit tests for all new functionality
- Place tests in `tests/` directory
- Run tests before committing:

```bash
cd build
ctest
```

### Commits

- Write clear, descriptive commit messages
- Reference story numbers in commits:

```
story-001-01: Implement basic Line geometry class

- Add Line class with start and end points
- Implement length() and angle() methods
- Add unit tests for Line calculations
```

## Pull Request Process

### 1. Complete the Story

Ensure all acceptance criteria are met:
- [ ] All acceptance criteria checked off
- [ ] Unit tests written and passing
- [ ] Integration tests passing
- [ ] Documentation updated
- [ ] Code reviewed (self-review at minimum)

### 2. Update Story Status

Update `docs/sprint-status.yaml`:

```yaml
- id: "story-XXX-XX"
  status: "completed"  # was: "not-started"
  assigned_to: "Your Name"
  actual: "X points"  # actual effort
```

### 3. Create Pull Request

```bash
git push origin story-XXX-XX-description
```

Then create PR on GitHub/GitLab with:
- **Title**: `Story XXX-XX: [Story Name]`
- **Description**:
  - Link to story file
  - Summary of changes
  - Testing performed
  - Screenshots (if UI changes)

### 4. Code Review

- Address all review comments
- Make requested changes
- Re-request review when ready

### 5. Merge

Once approved, squash and merge to main branch.

## Architecture Compliance

Follow the architecture decisions documented in [Architecture Document](docs/architecture-PatternCAD-2026-01-27.md):

- **MVC Pattern**: Separate model, view, and controller concerns
- **Command Pattern**: All user actions as undoable commands
- **Observer Pattern**: Use Qt signals/slots for event notification
- **Factory Pattern**: For creating tools and file format handlers

## Questions?

- Check the [Product Brief](docs/product-brief.md) for project vision
- Check the [PRD](docs/prd-PatternCAD-2026-01-27.md) for requirements
- Check the [Architecture](docs/architecture-PatternCAD-2026-01-27.md) for design decisions
- Open an issue for clarification

## Code of Conduct

- Be respectful and professional
- Focus on constructive feedback
- Welcome newcomers and help them learn
- Celebrate successes and learn from failures

Thank you for contributing to PatternCAD!
