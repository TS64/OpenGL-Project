//////////////////////////////////////////////////////////// 
// Headers 
//////////////////////////////////////////////////////////// 
#include "stdafx.h" 
#ifdef _DEBUG 
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#endif 
#pragma comment(lib,"opengl32.lib") 
#pragma comment(lib,"glu32.lib") 
 
#include "SFML/Graphics.hpp" 
#include "SFML/Graphics/Shader.hpp"
#include "SFML/OpenGL.hpp" 
#include <iostream> 
  
 
#include "Terrain.h"
#include "Camera.h"


int main() 
{ 
    // Create the main window 
	sf::ContextSettings Settings;
	Settings.depthBits = 24;
	Settings.stencilBits = 8;
	Settings.antialiasingLevel = 2;
    int width=600,height=600;
	sf::RenderWindow App(sf::VideoMode(width, height, 32), "SFML OpenGL", sf::Style::Close, Settings);
    // Create a clock for measuring time elapsed     
    sf::Clock Clock; 
	
	bool wireframeToggle = false;

	aiVector3D position(0,10,-30);
	Camera camera;
    camera.Init(position); //create a camera

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ZERO);
      
    //prepare OpenGL surface for HSR 
    glClearDepth(1.f); 
    glClearColor(0.3f, 0.3f, 0.6f, 0.f); //background colour
    glEnable(GL_DEPTH_TEST); 
    glDepthMask(GL_TRUE); 
   
    //// Setup a perspective projection & Camera position 
    glMatrixMode(GL_PROJECTION); 
    glLoadIdentity(); 
	//set up a 3D Perspective View volume
	gluPerspective(90.f, (float)width / height, 1.f, 300.0f);//fov, aspect, zNear, zFar 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	sf::Texture grass;
	sf::Texture sea;
	sf::Texture rock;
	GLuint text;
	std::string image = "allTex.png";
	grass.loadFromFile(image);
	if (!grass.loadFromFile(image))
	{
		std::cout << "Could not load " << image;
		char c;
		std::cin >> c;
		return false;
	}
	image = "sea.png";
	sea.loadFromFile(image);
	if (!sea.loadFromFile(image))
	{
		std::cout << "Could not load " << image;
		char c;
		std::cin >> c;
		return false;
	}
	image = "rock.png";
	rock.loadFromFile(image);
	if (!rock.loadFromFile(image))
	{
		std::cout << "Could not load " << image;
		char c;
		std::cin >> c;
		return false;
	}
	sf::Sprite grassS;
	grassS.setTexture(grass);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

     


	//load & bind the shader
	sf::Shader shader;

	//all the lighting & texture blending code should  be put in 'fragment.glsl'
	if(!shader.loadFromFile("vertex.glsl","fragment.glsl")){
		char c;
		std::cin >> c;
        exit(1);
    }

	/*sf::Texture::bind(&grass);
	shader.setParameter("grassTexture", sf::Shader::CurrentTexture);
	sf::Texture::bind(&sea);
	shader.setParameter("seaTexture", sf::Shader::CurrentTexture);
	sf::Texture::bind(&rock);
	shader.setParameter("rockTexture", sf::Shader::CurrentTexture);*/
	

	sf::Shader::bind(&shader);

	sf::RenderStates states;
	states.shader = &shader;

	//Create our Terrain
	Terrain terrain;
	terrain.Init();

    // Start game loop 
    while (App.isOpen()) 
    { 
        // Process events 
        sf::Event Event; 
        while (App.pollEvent(Event)) 
        { 
            // Close window : exit 
            if (Event.type == sf::Event::Closed) 
                App.close(); 
   
            // Escape key : exit 
            if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Escape)) 
                App.close(); 

			if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::A))
			{
				
			}

			if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::O))
			{
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				//set up a 2D Orthographic View volume
				glOrtho(-20.0, 20.0, -20.0, 20.0, -100.0, 300.0);
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
			}
            
			if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::P))
			{
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				//set up a 3D Perspective View volume
				gluPerspective(90.f, (float)width / height, 1.f, 300.0f);//fov, aspect, zNear, zFar 
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
			}

			if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::I))
			{
				if (wireframeToggle == false)
				{
					wireframeToggle = true;
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}
				else if (wireframeToggle)
				{
					wireframeToggle = false;
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			}
			//update the camera
			camera.Update(Event);
            
    
        } 
           
        //Prepare for drawing 
        // Clear color and depth buffer 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
		//sf::Texture::bind(&grass);
		//glBindTexture(GL_TEXTURE_2D, grass);

        // Apply some transformations 
        //initialise the worldview matrix
		glMatrixMode(GL_MODELVIEW); 
        glLoadIdentity(); 

		glColor3d(1, 1, 1);

		//get the viewing transform from the camera
		GLfloat lightPosition[] = { 5.0f, 2.0f, 0.0f, 0.0f };
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		camera.ViewingTransform();

		
		/*GLfloat specular[] = { 0.75, 0.75, 0.75, 0.75 };
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, specular);*/
		

		


		//make the world spin
		//TODO:probably should remove this in final
		//static float ang=0.0;
		//ang+=0.01f;
		//glRotatef(ang*2,0,1,0);//spin about y-axis
		

		
		//draw the world
		terrain.Draw();
		
		
		   
        // Finally, display rendered frame on screen 
        App.display(); 
    } 
   
    return EXIT_SUCCESS; 
}
