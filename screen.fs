#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;
const float offset = 1.0/1024;  

//void main()
//{
//	FragColor = texture(screenTexture, TexCoords);
//}

//����
//void main()
//{
//	FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
//}

void main()
{
	vec3 col = vec3(texture(screenTexture, TexCoords));
	// if(col.x!=1.0f&&col.y!=1.0f&&col.z!=1.0f){
	// 	FragColor = vec4(vec3(1.0 - col), 1.0);
	// }else{
	// 	FragColor = texture(screenTexture, TexCoords);
	// }
	FragColor = vec4(vec3(1.0 - col), 1.0);
}