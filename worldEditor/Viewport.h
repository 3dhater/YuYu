#ifndef __VIEWPORT_H__
#define __VIEWPORT_H__

#include "math\ray.h"


struct CursorRay
{
	yyRay m_center;
	yyRay m_N;
	yyRay m_S;
	yyRay m_E;
	yyRay m_W;
};

enum class ViewportCameraType
{
	Perspective,
	Front,
	Back,
	Left,
	Right,
	Top,
	Bottom
};

struct ViewportCamera
{
	ViewportCamera()
	{
		m_window = nullptr;
		m_camera = nullptr;
		m_type = ViewportCameraType::Perspective;
		m_fov = 1.f;
		m_moveSpeedDefault = 10.f;
		m_moveSpeed = m_moveSpeedDefault;
	}

	ViewportCameraType m_type;
	yyCamera* m_camera;
	f32 m_fov;
	yyWindow* m_window;

	f32 m_moveSpeed;
	f32 m_moveSpeedDefault;

	void init(yyWindow* window, ViewportCameraType);
	void reset();
	void destroy();
	void update();
	void rotate(const v2f& mouseDelta, f32 dt);

	void _moveCamera(v4f& vel);
	void moveForward(f32 deltaTime);
	void moveBackward(f32 deltaTime);
	void moveLeft(f32 deltaTime);
	void moveRight(f32 deltaTime);
	void moveUp(f32 deltaTime);
	void moveDown(f32 deltaTime);
};

class Viewport
{
public:
	Viewport();
	~Viewport();

	ViewportCamera* m_activeCamera;
	ViewportCamera* m_perspectiveCamera;

	void init(yyWindow*);
};

#endif