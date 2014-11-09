attribute vec3 position;
attribute vec2 texCoord;
attribute vec3 normal;
attribute vec3 tangent;

varying vec2 texCoord0;
varying vec3 worldPos0;
varying mat3 axesInfoMatrix;

uniform mat4 T_model;
uniform mat4 T_MVP;

void main()
{
    gl_Position = T_MVP * vec4(position, 1.0);
    texCoord0 = texCoord; 
    worldPos0 = (T_model * vec4(position, 1.0)).xyz;

    vec3 norm = normalize((T_model * vec4(normal, 0.0)).xyz);
    vec3 tang = normalize((T_model * vec4(tangent, 0.0)).xyz);
    //TODO: rework this line
    //tang = normalize((tang - dot(tang, norm)) * norm); //Gram-Schmidt re-orthoganalization
    vec3 biTangent = cross(tang, norm);

    axesInfoMatrix = mat3(tang, biTangent, norm);

}
