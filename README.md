# Interactive OpenGL Application: A Recreation of "The Lord of the Rings" Scenes

## Overview

This project is an interactive application developed using **OpenGL**, inspired by the universe of *The Lord of the Rings*. It features two distinct scenes: **The Shire** and **Mordor**, with a focus on photorealistic rendering, dynamic lighting, animations, and user interaction.

The application uses a first-person perspective, allowing the user to navigate through the scenes using keyboard and mouse inputs. Key visual elements include advanced lighting models, shadow mapping, and special effects like ash fall and moving lava.

---

## Features

### Scenes and Objects
1. **The Shire**:
   - Four Hobbit houses, including Bilbo/Frodo’s iconic home with detailed surroundings (fence, bench, stone stairs, and bird perched on a gate post).
   - A flowing river crossed by a bridge, where a cart moves, simulating Gandalf's arrival.
   - A field with crops, animals like ducks and deer, and various trees forming dense forests.
   - Special elements: Frodo’s ring near the river, highlighted by a circling eagle, and the sword Sting in Bilbo’s yard.

2. **Mordor**:
   - Surrounded by a volcanic mountain range and dominated by the Eye of Sauron.
   - Features include a lava lake with moving textures and Smaug the dragon overseeing the area.
   - Ash fall and fog effects create a foreboding atmosphere.

---

### Functionalities
- **Day/Night Cycle**:
  - Switch between day and night views of the Shire, with realistic lighting adjustments for moonlight and house interiors.
- **Interactive Navigation**:
  - Move freely with keyboard inputs (`W`, `A`, `S`, `D`) and control the view with the mouse.
  - Use "God-mode" (`G`) for unrestricted movement.
- **Dynamic Scene Transitions**:
  - Approach the ring in the Shire to transition to Mordor, and vice versa.
- **Camera and View Modes**:
  - Toggle between wireframe, point, and filled render modes (`F`).
  - Collision detection restricts movement too close to houses.

---

## Technical Details

### Rendering and Lighting
- **Blinn-Phong Lighting Model**:
  - Ambient, diffuse, and specular components for realistic illumination.
- **Shadow Mapping**:
  - Generates realistic shadows using frame buffers and depth textures.
  - Attempts were made to implement cascaded shadow mapping for improved quality.

### Effects and Animations
1. **Ash Fall**:
   - Overlays textures with transparent fragments for a dynamic effect.
   - Movement synchronized with simulation time.
2. **Lava Movement**:
   - Uses sinusoidal functions to animate points in the lava lake.
   - Normal vectors calculated for realistic reflections.
3. **Fog**:
   - Exponential equation for visibility reduction and depth perception.

### Camera Navigation
- Orthogonal coordinate system for precise movement and rotation.
- Custom matrices handle view transformations.
- Smooth transitions for "God-mode" and perspective shifts.
