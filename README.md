
# OpenGL-Graphics-Engine
Graphics Engine made from the group up entirely from openGL, GLFW, C++, GLSL, and various other libraries. Enables user to to easily load models and textures for rendering and viewing.

# Features/Details
* lighting is based of the phong shading model (diffuse, ambient, specular)
* Implemented Directional Lights, Point Lights, and Spot-light.
* Able to easily create Lights, Objects, and Sounds via helper classes.
* Textures can be added singular or grouped to a particular model.
* fragmentShaders and VertexShaders are implemented for generic models and lighting.
* Camera class enables user to modify the generic camera (allows both 3rd persons and 1st person view). Camera is movable
* implemented sound modules with openAL.
* Models are created and imported from blender and other 3D modeling softwares.
* Anti-aliasing functionality.
* Sky-Box functionality.
* Depth and Stencil test functionality.
* Face-culling functionaility.
* Fast instancing functionality to greatly increase rendering speed.
* easy Frame buffer to Texture functionality.

# Future development
* Implement a quaternion based camera to ensure the removal of gimbal lock. 
* Implement GUI functionality to easily added models via drag and drop.
* Implement save state functionality of scene.
* Easy access engine to add and remove from the scene.
* Bloom/ emission effects.
* ray tracing implementation to render scenes.

# How I got Started
A very helpful site that gets you started on learning OpenGL: [LearnOpenGL](https://learnopengl.com/).

# Tools used in the Project:
* OpenGL
* OpenCV
* GLSL
* C++
* GLFW
* Assimp
* ImGui
* OpenAL

# Demonstration
