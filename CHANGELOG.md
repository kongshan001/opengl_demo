# Changelog

All notable changes to this project will be documented in this file.

## [Unreleased]

### Added - 2026-02-22

#### Phong Lighting System (需求1)
- **New Classes**:
  - `PhongLight` - Base class for Phong lighting
  - `DirectionalLightPhong` - Directional light implementation
  - `PointLightPhong` - Point light with distance attenuation
  - `PhongLightManager` - Manages up to 8 lights
  - `PhongMaterial` - Material properties (ambient, diffuse, specular, shininess)

- **New Files**:
  - `include/light/Light.h` - Light class definitions
  - `include/light/LightManager.h` - Light manager header
  - `src/light/Light.cpp` - Light implementation
  - `src/light/LightManager.cpp` - Manager implementation
  - `resources/shaders/phong_lighting.vs` - Vertex shader
  - `resources/shaders/phong_lighting.fs` - Fragment shader with Blinn-Phong
  - `tests/test_phong_light.cpp` - 44 unit tests

- **Features**:
  - Phong lighting model (ambient + diffuse + specular)
  - Blinn-Phong specular highlights
  - Distance attenuation for point lights
  - Support for up to 8 lights simultaneously
  - GPU-friendly shader data structure with proper padding
  - Enable/disable lights at runtime
  - Get shader data array for uniform upload

- **Tests**: 44 unit tests covering:
  - DirectionalLightPhong (9 tests)
  - PointLightPhong (9 tests)
  - PhongLightManager (21 tests)
  - PhongMaterial (2 tests)
  - LightTypePhong enum (1 test)
  - Integration tests (2 tests)

### Changed
- Updated `cmake/Dependencies.cmake` to include `src/light/*.cpp`

---

## Multi-Agent Workflow

This project uses a multi-agent development workflow:

| Role | Responsibility |
|------|----------------|
| 🎯 Planner | Define requirements and acceptance criteria |
| 💻 Developer | Implement features using coding agents |
| 🔍 Reviewer | Code review and quality check |
| 🧪 QA | Write unit tests, ensure 100% coverage |
| 📝 Documenter | Update documentation and commit |

### Workflow
```
Planner → Developer → Reviewer → QA → Documenter → Git Commit → Push
```
