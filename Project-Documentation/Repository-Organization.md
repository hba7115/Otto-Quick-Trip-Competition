# Otto Competition Repository Organization

## Repository Strategy
Using separate repository per competition for focused development and multi-year tracking.

## Repository Structure
This repository follows a year-based organization pattern for long-term development:

### Year-Based Organization
- `2025/`, `2026/`, etc. - Separate folders for each competition year
- Allows easy comparison between years
- Preserves competition history and evolution
- Tracks improvements and lessons learned

### Standard Folders
- **Competition-Entry/**: Final code submitted for competition
- **Development/**: All development versions (Rev-A through Rev-Z)
- **Testing/**: Validation programs and test results
- **Documentation/**: Strategy, results, lessons learned  
- **Hardware/**: Hardware configurations, wiring, calibrations
- **Photos/**: Visual documentation of build and competition

### Shared Code
- Common libraries used across multiple programs
- MPU6050 integration code
- Basic Otto control functions
- Competition utilities

## Version Control Strategy
- **Main branch**: Stable, tested code
- **Development workflow**: Copy stable versions from Arduino IDE
- **Tags**: Mark competition milestones (v2025-final, v2025-practice-3)
- **Commits**: Document each significant improvement or fix

## Storage Strategy: Hybrid Approach
- **GitHub**: Primary code storage, version control, documentation
- **Local Development**: Active Arduino IDE workspace (`C:\Users\Jeep\Documents\Arduino\`)
- **Google Drive**: Photos, videos, large files, complete project backups

## Development Workflow
1. **Develop code locally** in Arduino IDE working directory
2. **Test and validate** changes thoroughly
3. **Copy stable versions** to appropriate repository folder
4. **Update documentation** with changes and test results
5. **Commit and sync** to GitHub with descriptive messages
6. **Upload photos/videos** to Google Drive
7. **Link Google Drive content** in GitHub documentation

## Cross-Repository Code Sharing
- Shared code is copied between competition repositories
- Each competition can evolve its own version as needed
- Maintains independence while allowing code reuse
- Common libraries stored in Shared-Code folder

## File Naming Conventions
- **Code versions**: `Otto_Competition_Rev_X`
- **Test programs**: `Otto_[Purpose]_Test`
- **Documentation**: `[Topic]-[Year].md`
- **No timestamps**: Clean, descriptive names only

## Documentation Standards
- **Markdown format** for all documentation
- **Clear headings** and structure
- **Links to related files** and external resources
- **Regular updates** with development progress
- **Competition results** and lessons learned

## Backup Strategy
- **Primary**: GitHub repository (cloud + local)
- **Secondary**: Google Drive complete project backup
- **Tertiary**: Local development folder backup
- **Frequency**: After major milestones and before competitions
