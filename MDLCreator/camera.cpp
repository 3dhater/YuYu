#include "yy.h"

#include "math\math.h"

#include "camera.h"

void camera_onUpdate(yyCamera* camera)
{
	math::makePerspectiveRHMatrix(
		camera->m_projectionMatrix,
		camera->m_fov,
		camera->m_aspect,
		camera->m_near,
		camera->m_far);

	auto V = math::mul(-camera->m_objectBase.m_localPosition, camera->m_rotationMatrix);
	camera->m_viewMatrix = camera->m_rotationMatrix;
	camera->m_viewMatrix[3] = V;
	camera->m_viewMatrix[3].w = 1.f;
}

Camera::Camera()
{
	m_camera = 0;
	m_moveSpeed = 5.f;
}

Camera::~Camera()
{
	if (m_camera) yyDestroy(m_camera);
}

void Camera::init()
{
	m_camera = yyCreate<yyCamera>();
	m_camera->m_cameraType = yyCameraType::Custom;
	m_camera->m_updateCallback = camera_onUpdate;	
	m_camera->m_fov = math::degToRad(80.f);
	m_camera->m_near = 0.1f;
	m_camera->m_far = 3000.f;

	reset();
}

void Camera::reset()
{
	m_camera->m_objectBase.m_localPosition.set(10.f, 10.f, 0.f, 0.f);
	m_camera->m_rotationMatrix.setRotation(Quat(math::degToRad(-45.f), math::degToRad(90.f), math::degToRad(0.f)));
}

void Camera::rotate(const v2f& mouseDelta, f32 dt)
{
	f32 speed = 5.f;
	Mat4 RX;
	Mat4 RY;
	bool update = false;
	if (mouseDelta.x != 0.f)
	{
		update = true;
		RY.setRotation(Quat(v4f(0.f, math::degToRad(mouseDelta.x) * dt * speed, 0.f, 0.f)));
	}
	if (mouseDelta.y != 0.f)
	{
		update = true;
		RX.setRotation(Quat(v4f(math::degToRad(mouseDelta.y) * dt * speed, 0.f, 0.f, 0.f)));
	}

	if (update)
		m_camera->m_rotationMatrix = RX * m_camera->m_rotationMatrix * RY;
}

void Camera::moveLeft(f32 dt, bool isShift)
{
	auto speed = m_moveSpeed; if (isShift) speed *= 10.f;
	_moveCamera(v4f(-speed * dt, 0.f, 0.f, 1.f));
}
void Camera::moveRight(f32 dt, bool isShift)
{
	auto speed = m_moveSpeed; if (isShift) speed *= 10.f;
	_moveCamera(v4f(speed * dt, 0.f, 0.f, 1.f));
}
void Camera::moveUp(f32 dt, bool isShift)
{
	auto speed = m_moveSpeed; if (isShift) speed *= 10.f;
	_moveCamera(v4f(0.f, speed * dt, 0.f, 1.f));
}
void Camera::moveDown(f32 dt, bool isShift)
{
	auto speed = m_moveSpeed; if (isShift) speed *= 10.f;
	_moveCamera(v4f(0.f, -speed * dt, 0.f, 1.f));
}
void Camera::moveBackward(f32 dt, bool isShift)
{
	auto speed = m_moveSpeed; if (isShift) speed *= 10.f;
	_moveCamera(v4f(0.f, 0.f, speed * dt, 1.f));
}
void Camera::moveForward(f32 dt, bool isShift)
{
	auto speed = m_moveSpeed; if (isShift) speed *= 10.f;
	_moveCamera(v4f(0.f, 0.f, -speed * dt, 1.f));
}

void Camera::_moveCamera(v4f& vel)
{
	auto RotInv = m_camera->m_rotationMatrix;
	RotInv.invert();
	vel = math::mul(vel, RotInv);
	m_camera->m_objectBase.m_localPosition += vel;
}

void Camera::update()
{
	m_camera->m_objectBase.UpdateBase();
	m_camera->Update();
}
