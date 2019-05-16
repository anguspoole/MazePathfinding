#include "cFollowCamera.h"

#include <math.h>

#include "GeneralUtilities.h"

cFollowCamera::cFollowCamera()
{
	// Defaults for OpenGL 1.0
	this->eye = glm::vec3( 0.0f, 0.0f, -1.0f );
	this->up = glm::vec3( 0.0f, 1.0f, 0.0f );
	this->maxSpeed = 1.0f;
	this->fullSpeedDistanceFromTarget = 1.0f;
	return;
}

cFollowCamera::cFollowCamera( glm::vec3 eye_, glm::vec3 target_ )
{
	this->eye = eye_;
	this->target = target_;
	return;
}

cFollowCamera::cFollowCamera( glm::vec3 eye_, glm::vec3 target_, glm::vec3 up_ )
{
	this->eye = eye_;
	this->target = target_;
	this->up = up_;
	return;
}


cFollowCamera::~cFollowCamera()
{
	return;
}


void cFollowCamera::Step( double deltaTime )
{
	// Ideal location is 'followTargetLocation'

	glm::vec3 distanceToTarget = this->eye - this->followTargetLocation;

	// If we are GT max distance on any axis, then go full speed
	if ( fabs(distanceToTarget.x) > this->fullSpeedDistanceFromTarget )
	{	// Go at maximum speed
		this->velocity.x = this->maxSpeed * -getSign<float>(distanceToTarget.x);
	}
	else
	{	// Slow down when getting close
		this->velocity.x = smoothstep<float>( 0.0f, this->fullSpeedDistanceFromTarget, fabs(distanceToTarget.x) )
			               * this->maxSpeed 
			               * -getSign<float>(distanceToTarget.x); 
	}

	if ( fabs(distanceToTarget.x) > this->fullSpeedDistanceFromTarget )
	{	// Go at maximum speed
		this->velocity.y = this->maxSpeed * -getSign<float>(distanceToTarget.y);
	}
	else
	{	// Slow down when getting close
		this->velocity.y = smoothstep<float>( 0.0f, this->fullSpeedDistanceFromTarget, fabs(distanceToTarget.y) )
			               * this->maxSpeed 
			               * -getSign<float>(distanceToTarget.y); 
	}


	if ( fabs(distanceToTarget.z) > this->fullSpeedDistanceFromTarget )
	{	// Go at maximum speed
		this->velocity.z = this->maxSpeed * -getSign<float>(distanceToTarget.z);
	}
	else
	{	// Slow down when getting close
		this->velocity.z = smoothstep<float>( 0.0f, this->fullSpeedDistanceFromTarget, fabs(distanceToTarget.z) )
			               * this->maxSpeed 
			               * -getSign<float>(distanceToTarget.z); 
	}


	this->eye.x += this->velocity.x * static_cast<float>(deltaTime);
	this->eye.y += this->velocity.y * static_cast<float>(deltaTime);
	this->eye.z += this->velocity.z * static_cast<float>(deltaTime);

	return;
}
