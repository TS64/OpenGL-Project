varying vec2 vTexCoord;
varying vec4 N;
varying vec3 v;

void main(void)
{
	v = vec3(gl_ModelViewMatrix * gl_Vertex);
	N.xyz = normalize(gl_NormalMatrix * gl_Normal);
	N.w = gl_Vertex.y;

	//normal.xyz = normalize(gl_NormalMatrix * gl_Normal);
    //normal.w = gl_Vertex.y;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord0;
	gl_TexCoord[2] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}