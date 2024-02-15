# LearnOpenGL

File structure should be:\
./LearnOpenGL/ (this repo)\
./include/\
./lib/\
./3dModels/\
As outlined in the project settings\
(in VS: *Project->Properties->VC++ Directories* and *Project->Properties->Linker->Input*)

OpenGL 3.3\
https://learnopengl.com/

## Dependencies
Notes are provided for manual building with CMake and Visual Studio.

https://github.com/glfw
- compile with CMake, build with Visual Studio
- put *glfw3.lib* in the ./lib/ foler
- put *glfw3.h* and *glfw3native.h* in the ./include/GLFW/ folder (if it doesn't exist then make it)

https://github.com/Dav1dde/glad/tree/master
- If the [webservice](https://glad.dav1d.de/) is unavailable, Compile with CMake, build with Visual Studio
- If the webservice is availaible, "make sure the language is set to C++, and in the API section select an OpenGL version of at least 3.3 (which is what we'll be using; higher versions are fine as well). Also make sure the profile is set to Core and that the Generate a loader option is ticked... and click Generate to produce the resulting library files."
- put the glad/ and KHR/ include folders in the ./include/ folder
- put *glad.c* into the ./LearnOpenGL/src/ folder if it doesn't already exist.
- Make sure you choose glad 1 not glad 2

https://freetype.org/ (freetype2)
- compile with CMake, build with Visual Studio
- put the [freetype]/include/freetype/ folder in the project ./include/ folder
- put *ft2build.h* in the ./include/ folder
-   "Due to how FreeType is developed (at least at the time of this writing), you cannot put their header files in a new directory; they should be located at the root of your include directories."
- put *freetype.lib* in the ./lib/ folder

https://github.com/assimp/assimp/blob/master/Build.md
- put the raw includes/assimp/ (the folder with all of the header .hpp files) folder in in the ./includes/ folder
- compile with CMake, build with Visual Studio
- put *assimp-vc143-mtd.lib* in the ./lib/ folder
- put *assimp-vc143-mtd.dll* in the ./LearnOpenGL/ folder
- put copy the assimp/ includes folder into the ./include/ directory (should be ./include/assimp/)

https://glm.g-truc.net/0.9.9/
- **do not build**
- put glm/glm/ (the folder with all of the header .hpp files) into ./include/ 

## 3d Models
https://learnopengl.com/data/models/backpack.zip
- put contents in ./3dModels/backpack/

https://github.com/superg3m/LearningOpenGL/blob/master/Resources/Neon.fbx
- put in ./3dModels/Neon/ (create folder)
