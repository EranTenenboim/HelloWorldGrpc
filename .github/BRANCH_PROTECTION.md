# Branch Protection Configuration

This document describes the recommended GitHub branch protection settings for this repository.

## Main Branch Protection

To enable branch protection for the `main` branch, go to:
**Settings → Branches → Add rule**

### Required Settings:

1. **Branch name pattern**: `main`
2. **Protect matching branches**: ✅ Checked

### Protection Rules:

#### ✅ Require a pull request before merging
- **Require approvals**: 1
- **Dismiss stale PR approvals when new commits are pushed**: ✅
- **Require review from code owners**: ✅ (if CODEOWNERS file exists)

#### ✅ Require status checks to pass before merging
- **Require branches to be up to date before merging**: ✅
- **Status checks required**:
  - `Setup and Validate`
  - `Build`
  - `Test Suite`
  - `Security & Quality`
  - `Documentation Check`
  - `Final Validation`

#### ✅ Require conversation resolution before merging
- **Require conversation resolution before merging**: ✅

#### ✅ Require signed commits
- **Require signed commits**: ✅ (recommended for security)

#### ✅ Require linear history
- **Require linear history**: ✅ (prevents merge commits)

#### ✅ Include administrators
- **Include administrators**: ✅ (applies rules to admins too)

## Development Workflow

### For Contributors:

1. **Create feature branch**:
   ```bash
   git checkout -b feature/your-feature-name
   ```

2. **Make changes and test locally**:
   ```bash
   ./setup.sh
   bazel build //...
   ./run_tests.sh
   ```

3. **Commit and push**:
   ```bash
   git add .
   git commit -m "feat: add your feature"
   git push origin feature/your-feature-name
   ```

4. **Create Pull Request**:
   - Go to GitHub repository
   - Click "New Pull Request"
   - Select your feature branch → main
   - Fill in PR description
   - Wait for CI checks to pass
   - Request review from maintainers

### For Maintainers:

1. **Review PR**: Check code quality, tests, and documentation
2. **Approve PR**: Once satisfied with the changes
3. **Merge**: Use "Squash and merge" or "Rebase and merge" (not "Merge commit")

## CI/CD Pipeline

The GitHub Actions workflow (`.github/workflows/ci.yml`) runs automatically on:

- **Push to main/develop**: Full pipeline
- **Pull Request to main**: Full pipeline
- **Manual trigger**: Via GitHub UI

### Pipeline Stages:

1. **Setup and Validate**: Checks prerequisites and validates environment
2. **Build**: Compiles all targets with Bazel
3. **Test Suite**: Runs unit tests, integration tests, and coverage
4. **Test Matrix**: Tests on multiple OS (Ubuntu 20.04/22.04, macOS 12/13)
5. **Security & Quality**: Security scans and code quality checks
6. **Performance Tests**: Performance benchmarks (main branch only)
7. **Documentation Check**: Validates README and scripts
8. **Final Validation**: Ensures all checks passed

### Required Checks for PRs:

- ✅ All builds must pass
- ✅ All tests must pass
- ✅ Security checks must pass
- ✅ Documentation must be valid
- ✅ Code coverage must not decrease
- ✅ No merge conflicts

## Troubleshooting CI Failures

### Common Issues:

1. **Build Failures**:
   - Check Bazel version compatibility
   - Verify all dependencies are available
   - Review BUILD file syntax

2. **Test Failures**:
   - Run tests locally: `./run_tests.sh`
   - Check test output for specific failures
   - Verify test environment setup

3. **Security Check Failures**:
   - Remove any hardcoded secrets
   - Use secure URLs (HTTPS) in BUILD files
   - Review dependency versions

4. **Documentation Failures**:
   - Check README syntax
   - Verify all links work
   - Ensure setup scripts are executable

### Getting Help:

- Check the Actions tab for detailed logs
- Review the specific failing job
- Run the same commands locally to reproduce
- Contact maintainers if issues persist

## Best Practices

### For Contributors:

- ✅ Keep PRs small and focused
- ✅ Write descriptive commit messages
- ✅ Add tests for new features
- ✅ Update documentation as needed
- ✅ Run `./setup.sh` and `./run_tests.sh` before pushing

### For Maintainers:

- ✅ Review PRs thoroughly
- ✅ Ensure CI passes before merging
- ✅ Keep main branch stable
- ✅ Use semantic versioning for releases
- ✅ Monitor CI performance and costs

## Emergency Procedures

### Bypassing Protection (Emergency Only):

If you need to bypass branch protection in an emergency:

1. **Temporarily disable protection**:
   - Go to Settings → Branches
   - Edit the main branch rule
   - Uncheck "Include administrators"
   - Make emergency fix
   - Re-enable protection

2. **Use force push** (last resort):
   ```bash
   git push --force-with-lease origin main
   ```

**⚠️ Warning**: Only use in genuine emergencies. Always re-enable protection immediately.
