/*struct TerrainRegion
{
    float min;
    float max;
};*/

/*uniform TerrainRegion region1;
uniform TerrainRegion region2;
uniform TerrainRegion region3;
uniform TerrainRegion region4;*/

uniform sampler2D grassTexture;
uniform sampler2D seaTexture;
uniform sampler2D rockTexture;

varying vec4 N;
varying vec3 v;

vec4 GenerateTerrainColor()
{
	vec4 grassTex = texture2D(grassTexture, gl_TexCoord[0].st);
	vec4 seaTex = texture2D(seaTexture, gl_TexCoord[0].st);
	vec4 rockTex = texture2D(rockTexture, gl_TexCoord[0].st);

	float region1min = -0.5 * 16.2;
	float region1max = 0.1 * 16.2;

	float region2min = 0.05 * 16.2;
	float region2max = 0.75 * 16.2;
	
	float region3min = 0.5 * 16.2;
	float region3max = 1.4 * 16.2;
    vec4 terrainColor = vec4(0.0, 0.0, 0.0, 1.0);


	vec3 n = N.xyz;
	vec3 L = normalize(gl_LightSource[0].position.xyz - v);
	vec4 specular = gl_LightSource[0].specular * max(dot(N,L), 0.0) * 0.5f;  // pow function wouldn't compile, scaled by 0.5f instead

    float height = N.w;
    float regionMin = 0.0;
    float regionMax = 0.0;
    float regionRange = 0.0;
    float regionWeight = 0.0;
    
    // Terrain region 1.
    regionMin = region1min;
    regionMax = region1max;
    regionRange = regionMax - regionMin;
    regionWeight = (regionRange - abs(height - regionMax)) / regionRange;
    regionWeight = max(0.0, regionWeight);
    //terrainColor += regionWeight * texture2D(seaTexture, gl_TexCoord[0].st);
	terrainColor += regionWeight * seaTex + specular;

    // Terrain region 2.
    regionMin = region2min;
    regionMax = region2max;
    regionRange = regionMax - regionMin;
    regionWeight = (regionRange - abs(height - regionMax)) / regionRange;
    regionWeight = max(0.0, regionWeight);
    //terrainColor += regionWeight * texture2D(grassTexture, gl_TexCoord[0].st);
	terrainColor += regionWeight * grassTex;

    // Terrain region 3.
    regionMin = region3min;
    regionMax = region3max;
    regionRange = regionMax - regionMin;
    regionWeight = (regionRange - abs(height - regionMax)) / regionRange;
    regionWeight = max(0.0, regionWeight);
    //terrainColor += regionWeight * texture2D(rockTexture, gl_TexCoord[0].st);
	terrainColor += regionWeight * rockTex;

    return terrainColor;
}

void main (void)
{
	vec3 n = N.xyz;
	vec4 grassTex = texture2D(grassTexture, gl_TexCoord[0].st);
	vec4 seaTex = texture2D(seaTexture, gl_TexCoord[0].st);
	vec4 rockTex = texture2D(rockTexture, gl_TexCoord[0].st);
	//gl_FragColor = grassTex;

	vec3 L = normalize(gl_LightSource[0].position.xyz - v);
	vec4 Idiff = gl_FrontLightProduct[0].diffuse * max(dot(n,L), 0.5);
	
	Idiff = clamp(Idiff, 0.0, 1.0);

	gl_FragColor = Idiff * GenerateTerrainColor();
}