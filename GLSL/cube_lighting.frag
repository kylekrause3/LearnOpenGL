#version 330 core
out vec4 FragColor;

uniform vec4 customColor;

void main()
{
    FragColor = vec4(1.0); // set all 4 vector values to 1.0

    if (customColor.w == 1.0f){
        FragColor = customColor;
    }
}