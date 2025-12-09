Assignment 1 (Modern OpenGL, GLFW + GLAD)

This folder contains **three** small programs that match your assignment spec:

1. `red_triangle` — shows a single **red triangle** (Task 1).
2. `blue_square` — shows a single **blue square** (Task 1, built by changing ~3 lines vs triangle).
3. `task2_scene` — draws the composite picture (two shaded red circles, a color‑interpolated triangle, and nested black/white squares) (Task 2).

All code uses **OpenGL 3.3 Core**, **no deprecated functions**.

---

## How to build (Windows, recommended: MSYS2 + MinGW + make)

1) Install **MSYS2**. Open the **MSYS2 MinGW x64** shell.
2) Install toolchain and deps:
```
pacman -S --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-pkgconf mingw-w64-x86_64-glfw make
```
3) Get **GLAD** (OpenGL 3.3 Core, language C/C++) from https://glad.dav1d.de/ (Generator).
   - Copy `include/glad/**` and `include/KHR/**` into `include/` here.
   - Copy `src/glad.c` into `src/` here (replace the placeholder file).

4) Build:
```
make
```
5) Run:
```
./bin/red_triangle
./bin/blue_square
./bin/task2_scene
```

> If you prefer **VS Code**, open this folder in VS Code, select the MSYS2 MinGW terminal, and run the same `make` commands.

---

## Linux (Ubuntu/Debian)

```
sudo apt install build-essential pkg-config libglfw3-dev
# add GLAD headers to include/ and glad.c to src/ (same as above)
make
```

---

## Visual Studio (MSVC) alternative (if your instructor **does not** require a Makefile)

- Install **vcpkg**, then in a VS Developer PowerShell:
```
git clone https://github.com/microsoft/vcpkg
.cpkgootstrap-vcpkg.bat
.cpkgcpkg install glfw3:x64-windows
.cpkgcpkg integrate install
```
- Create an **Empty Console App** (x64). Add `red_triangle.cpp` (or others) and **your** `glad.c` to the project.
- Add linker dep: `opengl32.lib`.
- Ensure the project uses the vcpkg triplet x64; GLFW headers/libs will be auto‑wired.

---

## Folder structure to submit

Zip a folder named like `ab123_a1/` (your initials + number) containing:
- `source files` (`src/*.cpp`, `src/glad.c`), 
- `Makefile`, 
- this `README.md`.

The **default make target** builds everything. I should be able to `cd` into the folder and type `make` to compile, then run each program from `bin/`.

---

## Notes

- Window size is **500×500** pixels as required.
- Colors and geometry are set per‑vertex; interpolation happens in the fragment shader.
- Circles use a **GL_TRIANGLE_FAN** with the **first vertex in the center**; the red channel varies with angle to mimic shading.
- Ellipse is just the circle with Y scaled by **0.6**.
- Squares are filled via **GL_TRIANGLE_STRIP** with grayscale to get black/white nesting.

Good luck!

