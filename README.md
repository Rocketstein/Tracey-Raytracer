# Tracey Raytracer

<img width="700" height="700" alt="Image" src="https://github.com/user-attachments/assets/c36ff2b4-08f3-4011-ba82-da766f3511e0" />

Tracey is a basic raytracer with accelerated rendering using openMP parallelism and BVH-based object container. A GUI control with a user-oriented material/object palette is provided for free rendering. To use Tracey, simply execute the compiled binary, or use the powershell command under main.cpp to build the project.

Danger Zone: There is no limit on AA-Factor, recursion depth, or image size. My recommendation is not to input values over 30k, 200, and 10k-10k, respectively.

## Camera Controls:
- FOV
- Enable / disable Depths-of-Field
- Camera Location
- Camera view direction

## Image Controls:
- Image size
- Anti-Aliasing Factor
- Recursion Depth

## Object Types
- Spheres
- Quadrilaterals
- Box

## Materials
- Metal
- Glass
- Diffuse
- Texture
