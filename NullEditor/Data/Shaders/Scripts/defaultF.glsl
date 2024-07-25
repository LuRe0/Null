#version 330 core
out vec4 FragColor;     // Output color

in vec2 TexCoord;
in vec4 color;

uniform sampler2D texture0;
uniform vec4 tintColor;

void main()
{
	// Sample the texture color and multiply by the vertex color
	vec4 resultColor = texture(texture0, TexCoord);

	if(resultColor.a == 0)
	{
		discard;
	}
	
	if(tintColor.a != 0)
	{
		resultColor *= tintColor;
	}
	
	FragColor = resultColor;
	
}