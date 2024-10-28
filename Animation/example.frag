// Shader-ul de fragment / Fragment shader 
#version 330 core

in vec4 ex_Color;
in vec2 tex_Coord;

out vec4 out_Color;

uniform sampler2D myTexture;
uniform bool hasTexture;

void main(void)
{
    if (hasTexture) {
        out_Color = texture(myTexture, tex_Coord);
    }
    else {
        out_Color = ex_Color;
    }
}