#ifndef _cFollowCamera_HG_
#define _cFollowCamera_HG_

#include <glm/glm.hpp>


class cFollowCamera
{
public:
	cFollowCamera();

	cFollowCamera( glm::vec3 eye, glm::vec3 target );
	cFollowCamera( glm::vec3 eye, glm::vec3 target, glm::vec3 up );
	~cFollowCamera();

	glm::vec3 eye;
	glm::vec3 up;
	glm::vec3 target;

	glm::vec3 velocity;
	glm::vec3 acceleration;

	float maxSpeed;
	float fullSpeedDistanceFromTarget;

	void Step( double deltaTime );

	glm::vec3 followTargetLocation;
};

#endif
