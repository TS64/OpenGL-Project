#include "stdafx.h"
#include "glew\include\GL\glew.h"
#include "freeglut\include\GL\freeglut.h"
#include "cwc\include\glsl.h"
#include "SFML/OpenGL.hpp"
#include "Terrain.h"
#include <cmath>
#include <iostream>
#include <fstream>


#define GL_GLEXT_PROTOTYPES
#define BITMAP_ID 0x4D42
GLuint LoadShader(const char * vertex_file_path, const char * fragment_file_path)
{
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string line = "";
		while (std::getline(VertexShaderStream, line))
			VertexShaderCode += "\n" + line;
		VertexShaderStream.close();
	}
	else{
		printf("Impossible to open");
		getchar();
		return 0;
	}

	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()){
		std::string line = "";
		while (std::getline(FragmentShaderStream, line))
			FragmentShaderCode += "\n" + line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);
	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);
	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);
	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0){
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

Terrain::Terrain(void)
{
	gridWidth=100;
	gridDepth=100;

	terrWidth=50; //size of terrain in world units
	terrDepth=50;
	vertices=NULL;
	colors=NULL;	

	grassL = 0.05;
	grassH = 0.75;
	seaHeight = 0.1;
	snowHeight = 0.6;
	
	//num squares in grid will be width*height, two triangles per square
	//3 verts per triangle
	 numVerts=gridDepth*gridWidth*2*3;

	 loadTexture();

	 GLenum err = glewInit();
	 if (err!= GLEW_OK)
	 {
		 fprintf(stderr, "Error:&s\n", glewGetErrorString(err));
	 }
	 programID = LoadShader("vertex.glsl", "fragment.glsl");

	 sf::Texture bitmap;
	 bitmap.loadFromFile("TerrainHeight.bmp");
	 //imageData = (bitmap, &bitmapInfoHeader);
	 imageData = LoadBitmapFile("heightmap.bmp", &bitmapInfoHeader);
	 //imageData = LoadBMP("TerrainAus.bmp", tex);
	 //imageData 

	 
	 for (int z = 0; z < 100; z++)
	 {
		 for (int x = 0; x < 100; x++)
		 {
			 //terrainHeights[x][z][0] = float(x)*20.0f;
			 terrainHeights[x][z] = imageData[(z * 100 + x) * 3];
			 //terrainHeights[x][z][2] = -float(z)*20.0f;
		 }
	 }
}

unsigned char * Terrain::LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	FILE *filePtr;							      // the file pointer
	BITMAPFILEHEADER	bitmapFileHeader;		// bitmap file header
	unsigned char		*bitmapImage;			// bitmap image data
	int					imageIdx = 0;		   // image index counter
	unsigned char		tempRGB;				   // swap variable

	// open filename in "read binary" mode
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// read the bitmap file header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	// verify that this is a bitmap by checking for the universal bitmap id
	if (bitmapFileHeader.bfType != BITMAP_ID)
	{
		fclose(filePtr);
		return NULL;
	}

	// read the bitmap information header
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	int s = (bitmapInfoHeader->biHeight * bitmapInfoHeader->biWidth) * 3;

	// move file pointer to beginning of bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// allocate enough memory for the bitmap image data
	bitmapImage = (unsigned char *)malloc(s);

	// verify memory allocation
	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}
	//bitmapInfoHeader->biSizeImage = 3126;
	// read in the bitmap image data
	
	fread(bitmapImage, 1, s, filePtr);

	// make sure bitmap image data was read
	if (bitmapImage == NULL)
	{
		fclose(filePtr);
		return NULL;
	}

	// swap the R and B values to get RGB since the bitmap color format is in BGR
	for (imageIdx = 0; imageIdx < s; imageIdx += 3)
	{
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}

	// close the file and return the bitmap image data
	fclose(filePtr);
	return bitmapImage;
}

void Terrain::loadTexture() {
	sf::Image grassImage;
	if (!grassImage.loadFromFile("grass.png"))
		std::cout << "Error!" << std::endl;

	glGenTextures(1, &grassTex);

	glBindTexture(GL_TEXTURE_2D, grassTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		grassImage.getSize().x, grassImage.getSize().y, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, grassImage.getPixelsPtr());

	sf::Image seaImage;
	if (!seaImage.loadFromFile("sea.png"))
		std::cout << "Error!" << std::endl;

	glGenTextures(1, &seaTex);

	glBindTexture(GL_TEXTURE_2D, seaTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		seaImage.getSize().x, seaImage.getSize().y, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, seaImage.getPixelsPtr());

	sf::Image rockImage;
	if (!rockImage.loadFromFile("rock.png"))
		std::cout << "Error!" << std::endl;

	glGenTextures(1, &rockTex);

	glBindTexture(GL_TEXTURE_2D, rockTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
		rockImage.getSize().x, rockImage.getSize().y, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, rockImage.getPixelsPtr());
}

bool Terrain::LoadHeightMapFromImage()
{
	return true;
}

Terrain::~Terrain(void)
{
	delete [] vertices;
	delete [] colors;
}

//interpolate between two values
float lerp(float start, float end, float t){
	return start+(end-start)*t;
}

float Terrain::Normalx(vector &p1, vector &p2, vector &p3)
{
	vector V1 = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
	vector V2 = { p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2] };
	return (V1[1] * V2[2]) - (V1[2] * V2[1]);
}
float Terrain::Normaly(vector &p1, vector &p2, vector &p3)
{
	vector V1 = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
	vector V2 = { p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2] };
	return (V2[2] * V1[0]) - (V2[0] * V1[2]);
}
float Terrain::Normalz(vector &p1, vector &p2, vector &p3)
{
	vector V1 = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
	vector V2 = { p3[0] - p1[0], p3[1] - p1[1], p3[2] - p1[2] };
	return (V1[0] * V2[1]) - (V1[1] * V2[0]);
}

void Terrain::setPoint(vector v,float x, float y, float z){

		v[0]=x;
		v[1]=y;
		v[2]=z;
}

//helper function to calculate height of terrain at a given point in space
//you will need to modify this significantly to pull height from a map
float  Terrain::getHeight(float x, float y){
	
	//for the sample we will calculate height based on distance form origin
	float dist=sqrt(x*x+y*y);

	//center will be the highest point
	dist=30-dist;
	//put a nice curve in it
	dist*=dist;
	dist*=dist;
	//whoah, way to high, make it smaller
	dist/=50000;

	//return dist;
	int v = 1;
	return (terrainHeights[(int)x][(int)y]) / 10.0;

	
}

float loadHeightMap(float xPos, float yPos) {
	return 0;
}

void Terrain::Init(){
	
	delete [] vertices;//just in case we've called init before
	vertices=new vector[numVerts];
	delete [] colors;
	colors=new vector[numVerts];
	heights = new vector[3];


	//interpolate along the edges to generate interior points
	for(int i=0;i<gridWidth-1;i++){ //iterate left to right
		for(int j=0;j<gridDepth-1;j++){//iterate front to back
			int sqNum=(j+i*gridDepth);
			int vertexNum=sqNum*3*2; //6 vertices per square (2 tris)
			float front=lerp(-terrDepth/2,terrDepth/2,(float)j/gridDepth);
			float back =lerp(-terrDepth/2,terrDepth/2,(float)(j+1)/gridDepth);
			float left=lerp(-terrWidth/2,terrWidth/2,(float)i/gridDepth);
			float right=lerp(-terrDepth/2,terrDepth/2,(float)(i+1)/gridDepth);
			
			/*
			back   +-----+	looking from above, the grid is made up of regular squares
			       |tri1/|	'left & 'right' are the x cooded of the edges of the square
				   |   / |	'back' & 'front' are the y coords of the square
				   |  /  |	each square is made of two trianlges (1 & 2)
				   | /   |	
				   |/tri2|
			front  +-----+
			     left   right
				 */
			//tri1
			setPoint(colors[vertexNum],(rand()%255)/255.0,(rand()%255)/255.0,(rand()%255)/255.0);
			//setPoint(vertices[vertexNum++],left,getHeight(left,front),front);
			setPoint(vertices[vertexNum++], left, getHeight(i, j), front);

			setPoint(colors[vertexNum],(rand()%255)/255.0,(rand()%255)/255.0,(rand()%255)/255.0);
			//setPoint(vertices[vertexNum++],right,getHeight(right,front),front);
			setPoint(vertices[vertexNum++], right, getHeight(i+1, j), front);

			setPoint(colors[vertexNum],(rand()%255)/255.0,(rand()%255)/255.0,(rand()%255)/255.0);
			//setPoint(vertices[vertexNum++],right,getHeight(right,back),back);
			setPoint(vertices[vertexNum++], right, getHeight(i+1, j+1), back);

			//tri2
			setPoint(colors[vertexNum], (rand() % 255) / 255.0, (rand() % 255) / 255.0, (rand() % 255) / 255.0);
			//setPoint(vertices[vertexNum++], left, getHeight(left, front), front);
			setPoint(vertices[vertexNum++], left, getHeight(i, j), front);

			setPoint(colors[vertexNum], (rand() % 255) / 255.0, (rand() % 255) / 255.0, (rand() % 255) / 255.0);
			//setPoint(vertices[vertexNum++], right, getHeight(right, back), back);
			setPoint(vertices[vertexNum++], right, getHeight(i+1, j+1), back);

			setPoint(colors[vertexNum], (rand() % 255) / 255.0, (rand() % 255) / 255.0, (rand() % 255) / 255.0);
			//setPoint(vertices[vertexNum++], left, getHeight(left, back), back);
			setPoint(vertices[vertexNum++], left, getHeight(i, j+1), back);

			//declare a degenerate triangle
			//TODO: fix this to draw the correct triangle
			setPoint(colors[vertexNum],0,0,0);
			setPoint(vertices[vertexNum++],0,0,0);
			setPoint(colors[vertexNum],0,0,0);
			setPoint(vertices[vertexNum++],0,0,0);
			setPoint(colors[vertexNum],0,0,0);
			setPoint(vertices[vertexNum++],0,0,0);




		}
	}
}


void Terrain::Draw(){
	float mcolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mcolor); // Diffuse lighting

	//GL_AMBIENT_AND_DIFFUSE
	/*GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat materialShininess[] = { 128.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);*/
	GLuint VertexArrayID;
	
	GLuint vbo;
	

	glBegin(GL_TRIANGLES);
	GLfloat height;
	GLfloat blend;	
	for (int i = 0, n = 0, t = 1, p = 0; i < numVerts; i++, n++, t++){
		height = vertices[i][1] / 16.2;
		if (height >= 0.6 && height <= 0.75)
		{
			GLfloat x = height - 0.6f;
			blend = x / 0.15f;
		}
		if (height >= 0.05 && height <= 0.1)
		{
			GLfloat x = height - 0.05;
			blend = x / 0.05f;
		}
		if (n == 3)
		{
			n = 0;
		}
		if (n == 0)
		{
			glNormal3d(Normalx(vertices[i], vertices[i + 1], vertices[i + 2]),
				Normaly(vertices[i], vertices[i + 1], vertices[i + 2]),
				Normalz(vertices[i], vertices[i + 1], vertices[i + 2]));
		}

		if (t == 1 && n == 0 && height > grassL && height < grassH) // Grass 1
		{
			glTexCoord2d(0, 0);
			p = 1;
		}
		if (t == 2 && p == 1) // Grass 2
		{
			glTexCoord2d(0.25, 0);
		}
		if (t == 3 && p == 1) // Grass 3
		{
			glTexCoord2d(0.25, 1);
			t = 0;
			p = 0;
		}

		if (t == 1 && n == 0 && height < seaHeight) // Sea 1
		{
			glTexCoord2d(0.25, 0);
			p = 2;
		}
		if (t == 2 && p == 2) // Sea 2
		{
			glTexCoord2d(0.48, 0);
		}
		if (t == 3 && p == 2) // Sea 3
		{
			glTexCoord2d(0.48, 1);
			t = 0;
			p = 0;
		}

		if (t == 1 && n == 0 && height > 0.75) // Rock 1
		{
			glTexCoord2d(0.5, 0);
			p = 3;
		}
		if (t == 2 && p == 3) // Rock 2
		{
			glTexCoord2d(0.75, 0);
		}
		if (t == 3 && p == 3) // Rock 2
		{
			glTexCoord2d(0.75, 1);
			p = 0;
		}
		if (t == 3)
		{
			t = 0;
			p = 0;
		}
		glVertex3fv(vertices[i]);
	}
	glEnd();
	
	glUseProgram(programID);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	GLint texLoc;
	texLoc = glGetUniformLocation(programID, "grassTexture");
	glUniform1i(texLoc, 0);

	texLoc = glGetUniformLocation(programID, "seaTexture");
	glUniform1i(texLoc, 1);

	texLoc = glGetUniformLocation(programID, "rockTexture");
	glUniform1i(texLoc, 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, grassTex);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, seaTex);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, rockTex);

	glVertexPointer(3, GL_FLOAT, 0, vertices);
	glTexCoordPointer(2, GL_FLOAT, 0, vertices);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glDrawArrays(GL_TRIANGLES, 0, numVerts);

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}