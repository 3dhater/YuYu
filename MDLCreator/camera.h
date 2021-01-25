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


	void rotate(const v2f& mouseDelta, f32 dt);
	void moveLeft(f32 dt);
	void moveRight(f32 dt);
	void moveUp(f32 dt);
	void moveDown(f32 dt);
	void moveBackward(f32 dt);
	void moveForward(f32 dt);
	void _moveCamera(v4f& vel);

	yyCamera* m_camera;
	Mat4 m_rotationMatrix;
	f32 m_moveSpeed;
};

#endif