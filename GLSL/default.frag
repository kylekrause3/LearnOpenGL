#version 330 core
out vec4 FragColor;
  
in vec3 vertexColor; // the input variable from the vertex shader (same name and same type)  
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    //80% of first input texture, 20% of second
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.5); 
    //FragColor = texture(texture2, TexCoord) - texture(texture1, TexCoord);
    FragColor += vec4(0, 0, 0, 1 - FragColor.z);

    if(vertexColor != vec3(0, 0, 0)){
        FragColor *= vec4(vertexColor, 1);
    }
    else {
        FragColor *= vec4(1, 1, 1, 1);
    }
}