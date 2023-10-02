# LearnOpenGL

File structure should be:
./LearnOpenGL/ (this repo)\
./include/\
./lib/\
./3dModels/\
As outlined in the project settings (in VS: *Project->Properties->VC++ Directories* and *Project->Properties->Linker->Input*)\

OpenGL 3.3\
https://learnopengl.com/

## Dependencies
Notes are provided for manual building with CMake and Visual Studio.

https://github.com/glfw
- compile with CMake, build with Visual Studio
- put *glfw3.lib* in the ./lib/ foler
- put *glfw3.h* and *glfw3native.h* in the ./include/GLFW/ folder (if it doesn't exist then make it)

https://github.com/Dav1dde/glad
- If the webservice is unavailable, Compile with CMake, build with Visual Studio
- put the glad/ and KHR/ include folders in the ./include/ folder
- put *glad.c* into the ./LearnOpenGL/src/ folder if it doesn't already exist.

https://freetype.org/
- compile with CMake, build with Visual Studio
- put the [freetype]/include/freetype/ folder in the project ./include/ folder
- put *ft2build.h* in the ./include/ folder
-   "Due to how FreeType is developed (at least at the time of this writing), you cannot put their header files in a new directory; they should be located at the root of your include directories."
- put *freetype.lib* in the ./lib/ folder

https://assimp.org/
- put the raw includes/assimp/ (the folder with all of the header .hpp files) folder in in the ./includes/ folder
- compile with CMake, build with Visual Studio
- put *assimp-vc143-mtd.lib* in the ./lib/ folder
- put *assimp-vc143-mtd.dll* in the ./LearnOpenGL/ folder
- put *Config.h* from the build's include folder in the ./includes/assimp/ folder

https://glm.g-truc.net/0.9.9/
- **do not build**
- put glm/glm/ (the folder with all of the header .hpp files) into ./include/ 
