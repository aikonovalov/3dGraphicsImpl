# 3dGraphicsImpl

**Lightweight CPU‑based 3D Renderer in C++**

This repository contains a hand‑crafted 3D graphics pipeline built from scratch—no GPU APIs, no external engines. It’s an educational project demonstrating each stage of transforming a mesh of triangles into a final 2D image, powered by Qt for windowing and display.

---

## Overview

- **Custom Math Library**  
  Implemented my own `Vector3`, `Matrix4x4`, `Plane`, and related types to handle all linear‑algebra operations.
- **OBJ Loader**  
  Simple parser for Wavefront `.obj` files, including vertices, normals, and texture coordinates.
- **Pipeline Stages**  
  1. **Model → World → View** transforms  
  2. **Backface culling** based on triangle winding  
  3. **Perspective division** and viewport mapping  
  4. **Depth buffering** and scanline rasterization  
  5. **Various light** with point and directional lights  
- **Shadow Cubemaps**  
  Six render passes around a point light source to generate omnidirectional shadow maps.

---

## Quick Start

1. **Prerequisites**  
   - C++20 compiler (GCC, Clang, MSVC)  
   - Qt 6 (Widgets)  
   - CMake 3.15+

2. **Clone & Build**

   ```bash
   git clone --branch dev https://github.com/aikonovalov/3dGraphicsImpl.git
   cd 3dGraphicsImpl
   mkdir build && cd build
   cmake ..
   cmake --build .
