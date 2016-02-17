#pragma once


#include "SFML/Graphics.hpp"

class Terrain
{
	//size of the mesh forming the terrain
	int gridWidth,gridDepth;
	int numVerts;
	//size of the terrain in world_coords
	float terrWidth,terrDepth;

	typedef  GLfloat vector[3];
	//array of vertices for the grid(this will be a triangle list)
	//I know, very inefficient, but let's run before we walk
	//vector *vertices;
	vector *colors;
	vector *heights;

	GLfloat grassL;
	GLfloat grassH;
	GLfloat seaHeight;
	GLfloat snowHeight;

	float getHeight(float x, float y);
	void setPoint(vector, float, float,float);
	void loadTexture();
	GLuint grassTex, seaTex, rockTex;
	GLuint programID;
	sf::Texture grass, rock, sea;

	GLuint vertexShaderObject, fragmentShaderObject;

	bool LoadHeightMapFromImage();

	float loadTerrain(float xPos, float yPos);

	unsigned char* imageData;
	BITMAPINFOHEADER bitmapInfoHeader;

	unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader);

	void UpdateTerrainShaderParameters();
	
public:
	Terrain(void);
	~Terrain(void);

	void Init();
	void Draw();
	float Terrain::Normalx(vector &p1, vector &p2, vector &p3);
	float Terrain::Normaly(vector &p1, vector &p2, vector &p3);
	float Terrain::Normalz(vector &p1, vector &p2, vector &p3);
	vector *vertices;

	float terrainHeights[100][100];
};

