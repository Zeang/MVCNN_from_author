#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model, view, projection;
uniform vec3 lightColor, objectColor, lightPos;

smooth out vec4 color;

void main()
{
    gl_Position = projection * view * vec4(position, 1.0f);

	vec3 FragPos = vec3(model * vec4(position, 1.0f));
	vec3 Normal = mat3(transpose(inverse(model))) * normal;
	
	float ambientStrength = 0.65f;
    vec3 ambient = ambientStrength * lightColor;
	
	vec3 lightDir = normalize(lightPos - FragPos);
	float diff = max(dot(Normal, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

    //vec3 result = (ambient + diffuse) * objectColor;
	vec3 result = ambient * 0.4f + diffuse * 0.6f;
	
	if (length(normal) == 0)
		color = vec4(objectColor, 1.0f);
	else
		color = vec4(result, 1.0f);
}