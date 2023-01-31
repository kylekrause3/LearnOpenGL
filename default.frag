#version 330 core
out vec4 FragColor;
  
uniform vec4 ourColor; // the input variable from the vertex shader (same name and same type)  

void main()
{
    FragColor = ourColor;
}