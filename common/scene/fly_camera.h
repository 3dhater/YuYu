#ifndef _YUYU_FLY_CAMERA_H_
#define _YUYU_FLY_CAMERA_H_

#include "math\math.h"
#include "scene\common.h"
#include "scene\camera.h"

YY_FORCE_INLINE
void yyFlyCamera_onUpdate(yyCamera* camera)
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

class yyFlyCamera : public yyCamera
{
	void _moveCamera(v4f& vel){
		auto RotInv = m_rotationMatrix;
		RotInv.invert();
		vel = math::mul(vel, RotInv);
		m_objectBase.m_localPosition += vel;
	}
public:

	yyFlyCamera(){
		m_moveSpeed = 5.f;

		m_cameraType = yyCameraType::Custom;
		m_updateCallback = yyFlyCamera_onUpdate;
		m_fov = math::degToRad(80.f);
		m_near = 0.1f;
		m_far = 3000.f;
	}

	~yyFlyCamera(){
	}

	void Update(){
		m_objectBase.UpdateBase();
		yyCamera::Update();
	}

	void SetPosition(f32 x, f32 y, f32 z){
		m_objectBase.m_localPosition.set(x, y, z, 1.f);
	}

	void SetRotation(f32 x, f32 y, f32 z){
		Mat4 RX;
		Mat4 RY;
		//Mat4 RZ;
		RY.setRotation(Quat(v4f(0.f, math::degToRad(y), 0.f, 0.f)));
		RX.setRotation(Quat(v4f(math::degToRad(x), 0.f, 0.f, 0.f)));
		//RZ.setRotation(Quat(v4f(0.f, 0.f, math::degToRad(z), 0.f)));
		m_rotationMatrix = RX * m_rotationMatrix * RY;
	}

	void Rotate(const v2f& mouseDelta, f32 dt){
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
			m_rotationMatrix = RX * m_rotationMatrix * RY;
	}

	void MoveLeft(f32 dt){
		auto speed = m_moveSpeed;
		_moveCamera(v4f(-speed * dt, 0.f, 0.f, 1.f));
	}

	void MoveRight(f32 dt){
		auto speed = m_moveSpeed; 
		_moveCamera(v4f(speed * dt, 0.f, 0.f, 1.f));
	}

	void MoveUp(f32 dt){
		auto speed = m_moveSpeed; 
		_moveCamera(v4f(0.f, speed * dt, 0.f, 1.f));
	}

	void MoveDown(f32 dt){
		auto speed = m_moveSpeed;
		_moveCamera(v4f(0.f, -speed * dt, 0.f, 1.f));
	}

	void MoveBackward(f32 dt){
		auto speed = m_moveSpeed;
		_moveCamera(v4f(0.f, 0.f, speed * dt, 1.f));
	}

	void MoveForward(f32 dt){
		auto speed = m_moveSpeed;
		_moveCamera(v4f(0.f, 0.f, -speed * dt, 1.f));
	}

	f32 m_moveSpeed;
};

#endif