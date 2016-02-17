#pragma comment(lib,"assimp.lib")
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "GL/glu.h" 





class Camera{
    static aiVector3D zero,yaxis,zaxis;
public:
    aiVector3D position;
    aiVector3D forward;
	aiVector3D cameraTarget;
    aiVector3D up;
	
 
    float forwardSpeed;
    float rotationSpeed;
	float pitch = 0;
	float yaw = 0;
	const float PI = 3.14159265;
	
     
    Camera():forwardSpeed(0.5f),rotationSpeed(0.1f){}
 
    void Init(aiVector3D& p=zero, aiVector3D& f=zaxis, aiVector3D& u=yaxis){
        position=p;
        forward=f;
        up=u;
    }
 
	void Update(sf::Event e){//respond to keyboard events
		if ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::D)){
                MoveLeftRight(-1);
            }

			if ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::A)){
                MoveLeftRight(1);
            }
 
            if ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::W)){
                MoveForwardBack(1);
            }
            if ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::S)){
                MoveForwardBack(-1);
            }

			if ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::Q)){
                MoveUpDown(1);
            }
            if ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::E)){
                MoveUpDown(-1);
            }
 
            if ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::Right)){
                TurnRightLeft(-1);
            }
            if ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::Left)){
                TurnRightLeft(1);
            }
            if ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::Up)){
               TurnUpDown(1);
            }
            if ((e.type == sf::Event::KeyPressed) && (e.key.code == sf::Keyboard::Down)){
                TurnUpDown(-1);
 
            }
	
	}
    void MoveLeftRight(int dir){ //Dir=+1=>Right, dir=-1=> Left
		position.x += dir * cos(yaw);
		position.z += dir * -sin(yaw);
    }

	void MoveUpDown(int dir){ //Dir=+1=>Right, dir=-1=> Left
		position.y += dir;
    }
 
    void MoveForwardBack(int dir){ //Dir=+1=>Forward, dir=-1=> Back
		position.x += dir * sin(yaw);
		position.z += dir * cos(yaw);
		position.y += dir * tan(pitch);
    }
 
    void TurnRightLeft(int dir){ //Dir=+1=>Right, dir=-1=> Left
		yaw += dir * (PI / 180.0f);
		forward.x = sin(yaw);
		forward.z = cos(yaw);
    }
         
    void TurnUpDown(int dir){ //Dir=+1=>Up, dir=-1=> Down
		pitch += dir * (PI / 180);
		forward.y = tan(pitch);
    }
 
    void ViewingTransform(){
        gluLookAt(	position.x,position.y,position.z,// camera position
			position.x + forward.x, position.y + forward.y, position.z + forward.z, //look at this point //TODO: BUG here!! what is it ??
					0,1,0); //camera up
    }
 
};

//create some default vectors
aiVector3D Camera::zero(0.0f);
aiVector3D Camera::yaxis(0.0f,1.0f,0.0f);
aiVector3D Camera::zaxis(0.0f,0.0f,1.0f);
