#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1; 
    sampler2D texture_emission1;
}; 

in vec2 TexCoords;
in vec3 FragPos;

uniform Material material;

void main()
{ 
    vec3 result = texture(material.texture_diffuse1, TexCoords).rgb;
    vec3 emission = texture(material.texture_emission1, TexCoords).rgb;    
    result += emission;

    FragColor = vec4(result, 1.0f);
}
