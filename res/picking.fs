#version 440

out vec4 gl_FragColor;

uniform int uuid;
 
void main()
{
    gl_FragColor = vec4(uuid / 255.0, 0, 0, 1.0);
}
