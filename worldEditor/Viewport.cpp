#include "yy.h"
#include "math\math.h"
#include "math\mat.h"
#include "math\quat.h"

//#include "KrGui.h"

#include "yy_window.h"

#include "scene\common.h"
#include "scene\camera.h"

#include "Viewport.h"

Viewport::Viewport()
{
	m_activeCamera = nullptr;
	m_perspectiveCamera = nullptr;
}

Viewport::~Viewport()
{
	if(m_perspectiveCamera)
	{
		yyDestroy(m_perspectiveCamera);
	}
}

void Viewport::init(yyWindow* window)
{
	m_perspectiveCamera = yyCreate<ViewportCamera>();
	m_perspectiveCamera->init(window, ViewportCameraType::Perspective);


	m_activeCamera = m_perspectiveCamera;
}


void ViewportCamera::init(yyWindow* window, ViewportCameraType cameraType)
{
	m_window = window;
	m_type = cameraType;
	
	switch (cameraType)
	{
	case ViewportCameraType::Perspective:
		m_fov = math::degToRad(110.f);
		break;
	case ViewportCameraType::Front:
	case ViewportCameraType::Back:
	case ViewportCameraType::Left:
	case ViewportCameraType::Right:
	case ViewportCameraType::Top:
	case ViewportCameraType::Bottom:
		m_fov = math::degToRad(0.1f);
		break;
	default:
		break;
	}

	
	reset();
}

void camera_onUpdate(yyCamera* camera)
{
	math::makePerspectiveRHMatrix(
		camera->m_projectionMatrix,
		camera->m_fov,
		camera->m_aspect,
		camera->m_near,
		camera->m_far);

	auto V = math::mul(-camera->m_objectBase.m_localPosition, camera->m_rotationMatrix);
	camera->m_viewMatrix = camera->m_rotationMatrix;// *camera->m_objectBase.m_globalMatrix;
	camera->m_viewMatrix[3] = V;
	camera->m_viewMatrix[3].w = 1.f;
}
void ViewportCamera::reset()
{
	destroy();
	
	m_camera = yyCreate<yyCamera>();
	m_camera->m_aspect = m_window->m_clientSize.x / m_window->m_clientSize.y;
	m_camera->m_cameraType = yyCameraType::Custom;
	m_camera->m_far = 2000.f;
	m_camera->m_near = 0.3f;
	m_camera->m_fov = m_fov;
	m_camera->m_updateCallback = camera_onUpdate;
//	m_camera->m_objectBase.SetRotation(v4f(math::degToRad(-45.f), 0.f, 0.f, 1.f));
	m_camera->m_objectBase.m_localPosition.set(2.f, 10.f, 0.f, 0.f);
	update();
}
void ViewportCamera::destroy()
{
	if (m_camera)
	{
		yyDestroy(m_camera);
		m_camera = nullptr;
	}
}
void ViewportCamera::update()
{
	m_camera->m_objectBase.UpdateBase();
	m_camera->Update();
}
void ViewportCamera::_moveCamera(v4f& vel)
{
	auto RotInv = m_camera->m_rotationMatrix;
	RotInv.invert();
	vel = math::mul(vel, RotInv);
	m_camera->m_objectBase.m_localPosition += vel;
}
void ViewportCamera::moveLeft(f32 dt)
{
	_moveCamera(v4f(-m_moveSpeed * dt, 0.f, 0.f, 1.f));
}
void ViewportCamera::moveRight(f32 dt)
{
	_moveCamera(v4f(m_moveSpeed * dt, 0.f, 0.f, 1.f));
}
void ViewportCamera::moveUp(f32 dt)
{
	_moveCamera(v4f(0.f, m_moveSpeed * dt, 0.f, 1.f));
}
void ViewportCamera::moveDown(f32 dt)
{
	_moveCamera(v4f(0.f, -m_moveSpeed * dt, 0.f, 1.f));
}
void ViewportCamera::moveBackward(f32 dt)
{
	_moveCamera(v4f(0.f, 0.f, m_moveSpeed * dt, 1.f));
}
void ViewportCamera::moveForward(f32 dt)
{
	_moveCamera(v4f(0.f, 0.f, -m_moveSpeed * dt, 1.f));
}
void ViewportCamera::rotate(const v2f& mouseDelta, f32 dt)
{
	f32 speed = 4.4f;
	Mat4 RX;
	Mat4 RY;
	bool update = false;
	if (mouseDelta.x != 0.f)
	{
		update = true;
		RY.setRotation(Quat(v4f(0.f, math::degToRad(-mouseDelta.x) * dt * speed, 0.f, 0.f)));
	}
	if (mouseDelta.y != 0.f)
	{
		update = true;
		RX.setRotation(Quat(v4f(math::degToRad(-mouseDelta.y) * dt * speed, 0.f, 0.f, 0.f)));
	}

	if (update)
		m_camera->m_rotationMatrix = RX * m_camera->m_rotationMatrix * RY;
}