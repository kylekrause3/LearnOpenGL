#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
  
out vec3 vertexColor; // specify a color output to the fragment shader
out vec2 TexCoord;

// model matrix translates local object vertices into world space coordinates
uniform mat4 model; 
// view matrix translates world space to camera view space
uniform mat4 view;
// projection matrix translates camera view space to clip space
uniform mat4 projection;



void main()
{
    // note that we read matrix multiplication from right to left
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    //gl_Position = transform * vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    vertexColor = aColor; // set ourColor to the input color we got from the vertex data
    TexCoord = aTexCoord;
}