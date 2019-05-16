#include "globalOpenGLStuff.h"

#include "globalStuff.h"

#include <iostream>

#include "globalMaze.h"

#include "cAnimationState.h"

// This has all the keyboard, mouse, and controller stuff

extern sLight* pTheOneLight;	//  = NULL;

bool g_MouseIsInsideWindow = false;



void cursor_enter_callback(GLFWwindow* window, int entered)
{
	if ( entered )
	{
		::g_MouseIsInsideWindow = true;
		std::cout << "Mouse moved indide window" << std::endl;
	}
	else
	{
		::g_MouseIsInsideWindow = false;
		std::cout << "Mouse moved outside window" << std::endl;
	}
	return;
}//cursor_enter_callback(...


void key_callback( GLFWwindow* window, 
						  int key, 
						  int scancode, 
						  int action, 
						  int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
        glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		mazeProcessingThreads = !mazeProcessingThreads;
	}


	// Ctrl down? 
	if ( mods == GLFW_MOD_CONTROL )
	{
		if ( key == GLFW_KEY_L && action == GLFW_PRESS )
		{
			// Toggle the spheres on and off
			::g_bDrawDebugLightSpheres = ! ::g_bDrawDebugLightSpheres;
		}
	}


	if ( key == GLFW_KEY_B && action == GLFW_PRESS )
	{
		if ( ::g_map_pParticleEmitters.find("Smoke01") != ::g_map_pParticleEmitters.end() )
		{
			::g_map_pParticleEmitters["Smoke01"]->Reset(true);
		}
		if ( ::g_map_pParticleEmitters.find("Smoke02") != ::g_map_pParticleEmitters.end() )
		{
			::g_map_pParticleEmitters["Smoke02"]->Reset(true);
		}
		
		if ( ::g_map_pParticleEmitters.find("PlasmaExplosion") != ::g_map_pParticleEmitters.end() )
		{
			::g_map_pParticleEmitters["PlasmaExplosion"]->Reset(100, true);
			// Since this is an explosion, turn off the regeneration
			::g_map_pParticleEmitters["PlasmaExplosion"]->ParticleGenerationOff();
		}
	}


	// HACK:
	// Change the animation from the keystrokes
	//7890

	cMeshSceneObject* pSM = findObjectByFriendlyName("Player1");

	if ( key == GLFW_KEY_7 && action == GLFW_PRESS )
	{
		pSM->currentAnimation =  "Idle";
		//pSM->currentAnimation =  "assets/modelsFBX/RPG-Character_Unarmed-Idle(FBX2013).fbx";
	

	}
	if ( key == GLFW_KEY_8 && action == GLFW_PRESS )
	{
		pSM->currentAnimation = "Unarmed-Attack-R3";
//		pSM->currentAnimation = "Unarmed-Attack-Kick-L1";
		//pSM->currentAnimation = "assets/modelsFBX/RPG-Character_Unarmed-Attack-Kick-L1(FBX2013).FBX";

		// Add an animation state
		cAnimationState* pKick = new cAnimationState();
		pKick->defaultAnimation.name = "Unarmed-Attack-R3";
		//pKick->defaultAnimation.name = "Unarmed-Attack-Kick-L1";

	}
	if ( key == GLFW_KEY_9 && action == GLFW_PRESS )
	{
		pSM->currentAnimation = "Unarmed-Walk";
		//pSM->currentAnimation = "assets/modelsFBX/RPG-Character_Unarmed-Walk(FBX2013).FBX";
	}
	if ( key == GLFW_KEY_0 && action == GLFW_PRESS )
	{
		pSM->currentAnimation = "Unarmed-Fall";
		//pSM->currentAnimation = "assets/modelsFBX/RPG-Character_Unarmed-Fall(FBX2013).fbx";
	}

	//const float CAMERA_SPEED = 0.1f;
	//// WASD + q = "up", e = down		y axis = up and down
	////									x axis = left and right
	////									z axis = forward and backward
	//// 
	//if (key == GLFW_KEY_W && action == GLFW_PRESS)	// "forward"
	//{	
	//	g_CameraEye.z += CAMERA_SPEED;
	//}
	//if (key == GLFW_KEY_S && action == GLFW_PRESS)	// "backwards"
	//{	
	//	g_CameraEye.z -= CAMERA_SPEED;
	//}
	//if (key == GLFW_KEY_A && action == GLFW_PRESS)	// "left"
	//{	
	//	g_CameraEye.x -= CAMERA_SPEED;
	//}
	//if (key == GLFW_KEY_D && action == GLFW_PRESS)	// "right"
	//{	
	//	g_CameraEye.x += CAMERA_SPEED;
	//}
	//if (key == GLFW_KEY_Q && action == GLFW_PRESS)	// "up"
	//{	
	//	g_CameraEye.y += CAMERA_SPEED;
	//}
	//if (key == GLFW_KEY_E && action == GLFW_PRESS)	// "down"
	//{	
	//	g_CameraEye.y -= CAMERA_SPEED;
	//}

	return;
}//key_callback(...



bool IsShiftDown(GLFWwindow* window)
{
	if ( glfwGetKey( window, GLFW_KEY_LEFT_SHIFT ) )	{ return true; }
	if ( glfwGetKey( window, GLFW_KEY_RIGHT_SHIFT ) )	{ return true; }
	// both are up
	return false;
}

bool IsCtrlDown(GLFWwindow* window)
{
	if ( glfwGetKey( window, GLFW_KEY_LEFT_CONTROL ) )	{ return true; }
	if ( glfwGetKey( window, GLFW_KEY_RIGHT_CONTROL ) )	{ return true; }
	// both are up
	return false;
}

bool IsAltDown(GLFWwindow* window)
{
	if ( glfwGetKey( window, GLFW_KEY_LEFT_ALT ) )	{ return true; }
	if ( glfwGetKey( window, GLFW_KEY_RIGHT_ALT ) )	{ return true; }
	// both are up
	return false;
}

bool AreAllModifiersUp(GLFWwindow* window)
{
	if ( IsShiftDown(window) )	{ return false;	}
	if ( IsCtrlDown(window) )	{ return false;	} 
	if ( IsAltDown(window) )	{ return false; }
	// Yup, they are all UP
	return true;
}

void ProcessAsyncKeys(GLFWwindow* window)
{
	const float CAMERA_MOVE_SPEED_SLOW = 0.1f;
	const float CAMERA_MOVE_SPEED_FAST = 1.0f;

	const float CAMERA_TURN_SPEED = 0.1f;

	// WASD + q = "up", e = down		y axis = up and down
	//									x axis = left and right
	//									z axis = forward and backward
	// 

	//float cameraSpeed = CAMERA_MOVE_SPEED_SLOW;
	//if ( glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS  )
	//{
	//	cameraSpeed = CAMERA_MOVE_SPEED_FAST;
	//}
	
	float cameraMoveSpeed = ::g_pFlyCamera->movementSpeed;
	
	// If no keys are down, move the camera
	if ( AreAllModifiersUp(window) )
	{
		// Note: The "== GLFW_PRESS" isn't really needed as it's actually "1" 
		// (so the if() treats the "1" as true...)

		// physics Checkpoint #4
		if ( glfwGetKey( window, GLFW_KEY_P ) == GLFW_PRESS )
		{
			::g_ProjectileInitialVelocity += 0.1f;
		}
		if ( glfwGetKey( window, GLFW_KEY_O ) == GLFW_PRESS )
		{
			::g_ProjectileInitialVelocity -= 0.1f;
		}
	
		if ( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS )
		{
//			g_CameraEye.z += cameraSpeed;
			::g_pFlyCamera->MoveForward_Z( +cameraMoveSpeed ); 
		}
		if ( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )	// "backwards"
		{	
//			g_CameraEye.z -= cameraSpeed;
			::g_pFlyCamera->MoveForward_Z( -cameraMoveSpeed ); 
		}
		if ( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS )	// "left"
		{	
//			g_CameraEye.x -= cameraSpeed;
			::g_pFlyCamera->MoveLeftRight_X( -cameraMoveSpeed );
		}
		if ( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS )	// "right"
		{	
//			g_CameraEye.x += cameraSpeed;
			::g_pFlyCamera->MoveLeftRight_X( +cameraMoveSpeed );
		}
		if ( glfwGetKey( window, GLFW_KEY_Q ) == GLFW_PRESS )	// "up"
		{	
			::g_pFlyCamera->MoveUpDown_Y( -cameraMoveSpeed );
//			::g_pFlyCamera->Roll_CW_CCW( +cameraSpeed );
		}
		if ( glfwGetKey( window, GLFW_KEY_E ) == GLFW_PRESS )	// "down"
		{	
//			g_CameraEye.y -= cameraSpeed;
			::g_pFlyCamera->MoveUpDown_Y( +cameraMoveSpeed );
//			::g_pFlyCamera->Roll_CW_CCW( -cameraSpeed );
		}
	}//if(AreAllModifiersUp(window)

	// If shift is down, do the rotation camera stuff...
	// If no keys are down, move the camera
	if ( IsShiftDown(window) )
	{	
		//if ( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS )
		//{
		//	::g_pFlyCamera->Pitch_UpDown( +cameraSpeed ); 
		//}
		//if ( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )	// "backwards"
		//{	
		//	::g_pFlyCamera->Pitch_UpDown( -cameraSpeed ); 
		//}
		//if ( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS )	// "left"
		//{	
		//	::g_pFlyCamera->Yaw_LeftRight( +cameraSpeed );
		//}
		//if ( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS )	// "right"
		//{	
		//	::g_pFlyCamera->Yaw_LeftRight( -cameraSpeed );
		//}
		if ( glfwGetKey( window, GLFW_KEY_Q ) == GLFW_PRESS )	// "up"
		{	
			::g_pFlyCamera->Roll_CW_CCW( -CAMERA_TURN_SPEED );
//			::g_pFlyCamera->MoveUpDown_Y( +cameraSpeed );
		}
		if ( glfwGetKey( window, GLFW_KEY_E ) == GLFW_PRESS )	// "down"
		{	
			::g_pFlyCamera->Roll_CW_CCW( +CAMERA_TURN_SPEED );
//			::g_pFlyCamera->MoveUpDown_Y( -cameraSpeed );
		}
	}//IsShiftDown(window)

	const float MIN_LIGHT_BRIGHTNESS = 0.001f;

	// Control (ctrl) key down? Move light
	if ( IsCtrlDown(window) )
	{
		if ( glfwGetKey( window, GLFW_KEY_W ) )	{	pTheOneLight->position.z += cameraMoveSpeed;	}
		if ( glfwGetKey( window, GLFW_KEY_S ) )	{	pTheOneLight->position.z -= cameraMoveSpeed;	}
		if ( glfwGetKey( window, GLFW_KEY_A ) )	{	pTheOneLight->position.x -= cameraMoveSpeed;	}
		if ( glfwGetKey( window, GLFW_KEY_D ) ) {	pTheOneLight->position.x += cameraMoveSpeed;	}
		if ( glfwGetKey( window, GLFW_KEY_Q ) )	{	pTheOneLight->position.y += cameraMoveSpeed;	}
		if ( glfwGetKey( window, GLFW_KEY_E ) )	{	pTheOneLight->position.y -= cameraMoveSpeed;	}

		if ( glfwGetKey( window, GLFW_KEY_1 ) )
		{	// Const
			pTheOneLight->atten.x *= 0.99f;		// Decrease by 1 percent
		}
		if ( glfwGetKey( window, GLFW_KEY_2 ) )
		{	// Const
			pTheOneLight->atten.x *= 1.01f;		// Increase by 1 percent
		}
		if ( glfwGetKey( window, GLFW_KEY_3 ) )
		{	// Linear
			pTheOneLight->atten.y *= 0.99f;		// Decrease by 1 percent
		}
		if ( glfwGetKey( window, GLFW_KEY_4 ) )
		{	// Linear
			pTheOneLight->atten.y *= 1.01f;		// Increase by 1 percent
		}
		if ( glfwGetKey( window, GLFW_KEY_5 ) )
		{	// Quadratic
			pTheOneLight->atten.z *= 0.99f;		// Decrease by 0.1 percent
		}
		if ( glfwGetKey( window, GLFW_KEY_6 ) )
		{	// Quadratic
			pTheOneLight->atten.z *= 1.01f;		// Increase by 0.1 percent
		}
	}//if(!IsShiftDown(window) )

//	std::cout << pTheOneLight->atten.x << ", " << pTheOneLight->atten.y << ", " << pTheOneLight->atten.z << std::endl;

	// Control (ctrl) key down? Move light
	if ( IsAltDown(window) )
	{
		cMeshSceneObject* pPlayer1 = findObjectByFriendlyName("Player1");
		float MoveSpeed = 0.2f;

		if ( glfwGetKey( window, GLFW_KEY_W ) )	
		{ 
			if (!maze->maze[pPlayer1->mazeX][pPlayer1->mazeZ + 1][0])
			{
				pPlayer1->position.z += MoveSpeed;
			}
			if (glm::abs(pPlayer1->position.z - pPlayer1->mazeZ * 2.001f) > 1.5f)
			{
				pPlayer1->mazeZ++;
			}
		}
		if ( glfwGetKey( window, GLFW_KEY_S ) )	
		{
			if (!maze->maze[pPlayer1->mazeX][pPlayer1->mazeZ - 1][0])
			{
				pPlayer1->position.z -= MoveSpeed;
			}
			if (glm::abs(pPlayer1->position.z - pPlayer1->mazeZ * 2.001f) > 1.5f)
			{
				pPlayer1->mazeZ--;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_A))
		{
			if (!maze->maze[pPlayer1->mazeX + 1][pPlayer1->mazeZ][0])
			{
				pPlayer1->position.x += MoveSpeed;
			}
			if (glm::abs(pPlayer1->position.x - pPlayer1->mazeX * 2.001f) > 1.5f)
			{
				pPlayer1->mazeX++;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_D))
		{
			if (!maze->maze[pPlayer1->mazeX - 1][pPlayer1->mazeZ][0])
			{
				pPlayer1->position.x -= MoveSpeed;
			}
			if (glm::abs(pPlayer1->position.x - pPlayer1->mazeX * 2.001f) > 1.5f)
			{
				pPlayer1->mazeX--;
			}
		}
		//if ( glfwGetKey( window, GLFW_KEY_Q ) )	{ pPlayer1->position.y += MoveSpeed; }
		//if ( glfwGetKey( window, GLFW_KEY_E ) )	{ pPlayer1->position.y -= MoveSpeed; }
		//if ( glfwGetKey( window, GLFW_KEY_PERIOD ) )
		//{	// Increase the "brightness"
		//	pRogerRabbit->scale *= 1.01f;		// Increase by 1 percent
		//}
		//if ( glfwGetKey( window, GLFW_KEY_COMMA ) )
		//{	// Decrease the "brightness"
		//	::g_lightBrightness *= 0.99f;		// Decrease by 1 percent
		//	if ( ::g_lightBrightness < MIN_LIGHT_BRIGHTNESS )
		//	{
		//		::g_lightBrightness = MIN_LIGHT_BRIGHTNESS;
		//	}
		//}

		/*if (glm::abs(pPlayer1->position.z - pPlayer1->mazeZ * 2.001f) > 1.5f)
		{
			pPlayer1->mazeZ++;
		}
		if (glm::abs(pPlayer1->position.x - pPlayer1->mazeX * 2.001f) > 1.5f)
		{
			pPlayer1->mazeX++;
		}*/
	}//if(!IsShiftDown(window) )	
	
	return;
}// ProcessAsyncKeys(..


// Mouse (cursor) callback
void cursor_position_callback( GLFWwindow* window, double xpos, double ypos )
{

	return;
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    
	return;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// A regular mouse wheel returns the y value
	::g_pFlyCamera->setMouseWheelDelta( yoffset );

//	std::cout << "Mouse wheel: " << yoffset << std::endl;

	return;
}

void ProcessAsyncMouse(GLFWwindow* window)
{
	double x, y;
	glfwGetCursorPos( window, &x, &y );

	::g_pFlyCamera->setMouseXY( x, y );

	const float MOUSE_SENSITIVITY = 0.1f;

//	std::cout << ::g_pFlyCamera->getMouseX() << ", " << ::g_pFlyCamera->getMouseY() << std::endl;

	// Mouse left (primary?) button pressed? 
	// AND the mouse is inside the window...
	if ( ( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS ) 
		 && ::g_MouseIsInsideWindow )
	{
		// Mouse button is down so turn the camera
		::g_pFlyCamera->Yaw_LeftRight( -::g_pFlyCamera->getDeltaMouseX() * MOUSE_SENSITIVITY );

		::g_pFlyCamera->Pitch_UpDown( ::g_pFlyCamera->getDeltaMouseY() * MOUSE_SENSITIVITY );

	}

	// Adjust the mouse speed
	if ( ::g_MouseIsInsideWindow )
	{
		const float MOUSE_WHEEL_SENSITIVITY = 0.1f;

		// Adjust the movement speed based on the wheel position
		::g_pFlyCamera->movementSpeed = (::g_pFlyCamera->getMouseWheel() * MOUSE_WHEEL_SENSITIVITY);
		if ( ::g_pFlyCamera->movementSpeed <= 0.0f )
		{
			::g_pFlyCamera->movementSpeed = 0.0f;
		}
	}

	return;
}//ProcessAsyncMouse(...





void joystick_callback(int joy, int event)
{
    if (event == GLFW_CONNECTED)
    {
		std::cout << "Joystick #" << joy << " was connected. Reseting...";
        // The joystick was connected
		::g_pJoysticks->InitOrReset();
		std::cout << "done." << std::endl;
    }
    else if (event == GLFW_DISCONNECTED)
    {
        // The joystick was disconnected
		std::cout << "Joystick #" << joy << " was disconnected. Reseting...";
        // The joystick was connected
		::g_pJoysticks->InitOrReset();
		std::cout << "done." << std::endl;
    }

	return;
}

void ProcessAsyncJoysticks(GLFWwindow* window, cJoystickManager* pJoysticks)
{
	// Any joysticks?
	if ( pJoysticks->getNumberOfJoysticksPresent() < 1 )
	{
		// Attempt to reset the joysticks
		pJoysticks->InitOrReset();

		// Still none? 
		if (pJoysticks->getNumberOfJoysticksPresent() < 1 )
		{
			return;
		}
	}//if ( pJoysticks->getNumberOfJoysticksPresent() < 1 )

	unsigned int JoystickID = ::g_pJoysticks->getFirstJoystickID();



	if ( JoystickID != cJoystickManager::JOYSTICK_INVALID_ID )
	{
		switch ( ::g_pJoysticks->GetJoystickType(JoystickID) )
		{
		case cJoystickManager::sJoyInfo::PS4_Controller:
			z_ProcessAsyncJoysticksPS4(window, pJoysticks);
			break;
	 
		case cJoystickManager::sJoyInfo::XboxOne_Controller:
			z_ProcessAsyncJoysticksXboxOne(window, pJoysticks);
			break;
		}
	}
	return;
}

void z_ProcessAsyncJoysticksXboxOne(GLFWwindow* window, cJoystickManager* pJoysticks)
{

	// Get the 1st joystick
	unsigned int joystickID = pJoysticks->getFirstJoystickID();

	pJoysticks->Update(joystickID);

	
	const float CAMERA_JOYSTICK_FORWARD_SPEED = 1.0f;
	const float CAMERA_JOYSTICK_TURN_SPEED = 0.25f;
	const float CAMERA_JOYSTICK_ROLL_SPEED = 0.25f;
	const float JOYSTICK_DEADZONE = 0.1f;


//	float cameraMoveSpeed = ::g_pFlyCamera->movementSpeed;
	float cameraMoveSpeed = CAMERA_JOYSTICK_FORWARD_SPEED;


	float axis_4 = pJoysticks->getJoystickAxis( joystickID, 4);
	float axis_5 = pJoysticks->getJoystickAxis( joystickID, 5);

	::g_pFlyCamera->MoveForward_Z( (-cameraMoveSpeed) * (axis_4 - axis_5) ); 

	
	float left_x = pJoysticks->getJoystickAxis( joystickID, 0 );
	float left_y = pJoysticks->getJoystickAxis( joystickID, 1 );
	float right_x = pJoysticks->getJoystickAxis( joystickID, 2 );
	float right_y = pJoysticks->getJoystickAxis( joystickID, 3 );

	::g_pFlyCamera->Yaw_LeftRight( (-cameraMoveSpeed) * (right_x * CAMERA_JOYSTICK_TURN_SPEED) );
	::g_pFlyCamera->Pitch_UpDown( (+cameraMoveSpeed) * (right_y * CAMERA_JOYSTICK_TURN_SPEED) );

	const unsigned int XBOX_BUTTON_A = 0;
	const unsigned int XBOX_BUTTON_B = 1;
	const unsigned int XBOX_BUTTON_X = 2;
	const unsigned int XBOX_BUTTON_Y = 3;

	const unsigned int XBOX_LEFT_BUMPER = 4;
	const unsigned int XBOX_RIGHT_BUMPER = 5;

	const unsigned int XBOX_BACK_BUTTON = 6;
	const unsigned int XBOX_START_BUTTON = 7;

	const unsigned int XBOX_LEFT_JOYSTICK_BUTTON = 8;
	const unsigned int XBOX_RIGHT_JOYSTICK_BUTTON = 9;

	const unsigned int XBOX_DPAD_UP = 10;
	const unsigned int XBOX_DPAD_RIGHT = 11;
	const unsigned int XBOX_DPAD_DOWN = 12;
	const unsigned int XBOX_DPAD_LEFT = 13;

	//if ( pJoysticks->isJoystickButtonDown( joystickID, XBOX_LEFT_BUMPER ) )
	//{
	//	::g_pFlyCamera->Roll_CW_CCW( -CAMERA_JOYSTICK_ROLL_SPEED );
	//}
	//if ( pJoysticks->isJoystickButtonDown( joystickID, XBOX_RIGHT_BUMPER ) )
	//{
	//	::g_pFlyCamera->Roll_CW_CCW( +CAMERA_JOYSTICK_ROLL_SPEED );
	//}
	::g_pFlyCamera->Roll_CW_CCW( left_x * CAMERA_JOYSTICK_ROLL_SPEED );

	if ( pJoysticks->isJoystickButtonDown( joystickID, XBOX_DPAD_UP ) )
	{
		::g_pFlyCamera->MoveUpDown_Y( +CAMERA_JOYSTICK_FORWARD_SPEED );
	}
	if ( pJoysticks->isJoystickButtonDown( joystickID, XBOX_DPAD_DOWN ) )
	{
		::g_pFlyCamera->MoveUpDown_Y( -CAMERA_JOYSTICK_FORWARD_SPEED );
	}
	if ( pJoysticks->isJoystickButtonDown( joystickID, XBOX_DPAD_LEFT ) )
	{
		::g_pFlyCamera->MoveLeftRight_X( -CAMERA_JOYSTICK_FORWARD_SPEED );
	}
	if ( pJoysticks->isJoystickButtonDown( joystickID, XBOX_DPAD_RIGHT ) )
	{
		::g_pFlyCamera->MoveLeftRight_X( +CAMERA_JOYSTICK_FORWARD_SPEED );
	}

	//if ( pJoysticks->isJoystickButtonDown( joystickID, 1 ) )
	//{
	//	::g_pFlyCamera->Roll_CW_CCW( CAMERA_JOYSTICK_ROLL_SPEED );
	//}

	//std::vector<float> vecAxes;
	//std::vector<unsigned char> vecButtons;
	//pJoysticks->GetJoystickState( joystickID, vecAxes, vecButtons );



	return;
}

void z_ProcessAsyncJoysticksPS4(GLFWwindow* window, cJoystickManager* pJoysticks)
{
	// Get the 1st joystick
	unsigned int joystickID = pJoysticks->getFirstJoystickID();

	pJoysticks->Update(joystickID);

	
	const float CAMERA_JOYSTICK_FORWARD_SPEED = 1.0f;
	const float CAMERA_JOYSTICK_TURN_SPEED = 0.25f;
	const float CAMERA_JOYSTICK_ROLL_SPEED = 0.25f;
	const float JOYSTICK_DEADZONE = 0.1f;


	//	float cameraMoveSpeed = ::g_pFlyCamera->movementSpeed;
	float cameraMoveSpeed = CAMERA_JOYSTICK_FORWARD_SPEED;

	const unsigned int PS4_LEFT_X_AXIS = 0;
	const unsigned int PS4_LEFT_Y_AXIS = 1;
	const unsigned int PS4_RIGHT_X_AXIS = 2;
	const unsigned int PS4_RIGHT_Y_AXIS = 5;
	const unsigned int PS4_LEFT_TRIGGER = 3;
	const unsigned int PS4_RIGHT_TRIGGER = 4;

	const unsigned int PS4_DPAD_DOWN = 16;
	const unsigned int PS4_DPAD_UP = 14;

	const unsigned int PS4_DPAD_LEFT = 17;
	const unsigned int PS4_DPAD_RIGHT = 15;

	//TODO:	
	const unsigned int PS4_LEFT_BUMPER = 0;		// ????
	const unsigned int PS4_RIGHT_BUMPER = 0;	// ????
	const unsigned int PS4_SHARE_BUTTON = 0;
	const unsigned int PS4_OPTIONS_BUTTON = 0;
	const unsigned int PS4_LEFT_JOY_BUTTON = 0;
	const unsigned int PS4_RIGHT_JOY_BUTTON = 0;
	const unsigned int PS4_TOUCHPAD_BUTTON = 0;




	float axis_left_x = pJoysticks->getJoystickAxis( joystickID, PS4_LEFT_X_AXIS);
	float axis_left_y = pJoysticks->getJoystickAxis( joystickID, PS4_LEFT_Y_AXIS);
	//float axis_4 = pJoysticks->getJoystickAxis( joystickID, 4);
	//float axis_5 = pJoysticks->getJoystickAxis( joystickID, 5);
	float axis_right_y = pJoysticks->getJoystickAxis( joystickID, PS4_RIGHT_Y_AXIS);
	float axis_right_x = pJoysticks->getJoystickAxis( joystickID, PS4_RIGHT_X_AXIS);

	cMeshSceneObject* pPlayer = findObjectByFriendlyName("PlayerBody");

	const float PLAYER_MOVEMENT_SCALING = 0.1f;

	switch (::g_CameraInUse)
	{
	case eCameraTypes::FOLLOW_CAMERA:
		pPlayer->position.x -= (axis_left_x * PLAYER_MOVEMENT_SCALING);
		pPlayer->position.z -= (axis_left_y * PLAYER_MOVEMENT_SCALING);
//		::g_pFollowCamera->followTargetLocation = pPlayer->position;

		break;
	case eCameraTypes::FLY_CAMERA:

		::g_pFlyCamera->MoveForward_Z( (-cameraMoveSpeed) * axis_right_y ); 
		::g_pFlyCamera->MoveLeftRight_X( (-cameraMoveSpeed) * axis_right_x ); 

		::g_pFlyCamera->Yaw_LeftRight( (-cameraMoveSpeed) * (axis_left_x * CAMERA_JOYSTICK_TURN_SPEED) );
		::g_pFlyCamera->Pitch_UpDown( (+cameraMoveSpeed) * (axis_left_y * CAMERA_JOYSTICK_TURN_SPEED) );


		if ( pJoysticks->isJoystickButtonDown( joystickID, PS4_DPAD_UP ) )
		{
			::g_pFlyCamera->MoveUpDown_Y( CAMERA_JOYSTICK_FORWARD_SPEED );
		}
		if ( pJoysticks->isJoystickButtonDown( joystickID, PS4_DPAD_DOWN ) )
		{
			::g_pFlyCamera->MoveUpDown_Y( -CAMERA_JOYSTICK_FORWARD_SPEED );
		}
		break;
	}//switch (::g_CameraInUse)

	return;
}//void ProcessAsyncJoysticks(...