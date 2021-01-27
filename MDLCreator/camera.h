#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "scene/common.h"
#include "scene/camera.h"

class Camera
{
public:
	Camera();
	~Camera();

	void init();
	void update();
	void reset();


	void rotate(const v2f& mouseDelta, f32 dt);
	
	void moveLeft(f32 dt, bool isShift );
	void moveRight(f32 dt, bool isShift);
	void moveUp(f32 dt, bool isShift);
	void moveDown(f32 dt, bool isShift);
	void moveBackward(f32 dt, bool isShift);
	void moveForward(f32 dt, bool isShift);

	void _moveCamera(v4f& vel);

	yyCamera* m_camera;
	f32 m_moveSpeed;
};

#endif