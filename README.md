# 🎧 NCS Visualizer with Perlin Noise (SFML + C++)

A real-time music visualizer inspired by NCS (NoCopyrightSounds) style, built in C++ using [SFML](https://www.sfml-dev.org/) and Perlin noise. It renders a pulsating sphere of glowing dots reacting to audio amplitude, creating a mesmerizing visual effect.

<p align="center">
  <img src="assets/NCS visualizer.gif" width="600">
</p>



---

## ✨ Features

- Real-time audio-reactive visuals
- Perlin noise for fluid animation
- Dot size, amplitude, and frequency adjustable at runtime
- Lightweight and smooth on most systems

---

## 🚀 Getting Started

### 🔧 Prerequisites

- C++17 compatible compiler (e.g. MSVC, g++, clang++)
- [SFML 2.6.x](https://www.sfml-dev.org/download.php)
- `PerlinNoise.hpp` (header-only library by Ken Perlin / S. Ivanov)

You can get `PerlinNoise.hpp` from:  
👉 https://github.com/Reputeless/PerlinNoise

### 🛠️ Build Instructions (CMake)

```bash
git clone https://github.com/your-username/ncs-visualizer.git
cd ncs-visualizer

mkdir build && cd build
cmake ..
cmake --build .
