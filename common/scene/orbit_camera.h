#ifndef _YUYU_ORBIT_CAMERA_H_
#define _YUYU_ORBIT_CAMERA_H_

#include "math\math.h"
#include "scene\common.h"
#include "scene\camera.h"

class yyOrbitCamera : public yyCamera
{
public:

	yyOrbitCamera(){
		m_cameraType = yyCameraType::Custom;
		m_fov = math::degToRad(80.f);
		m_near = 0.1f;
		m_far = 3000.f;
	}

	~yyOrbitCamera(){
	}

	v3f m_positionInWorld;

	void Update(){
		m_objectBase.UpdateBase();
		
		math::makePerspectiveRHMatrix(
			m_projectionMatrix,
			m_fov,
			m_aspect,
			m_near,
			m_far);

		m_positionInWorld = v3f(0.f, m_objectBase.m_localPosition.w, 0.f);

		Mat4 MX(Quat(m_objectBase.m_rotation.x, 0.f, 0.f));
		Mat4 MY(Quat(0.f, m_objectBase.m_rotation.y, 0.f));
		//Mat4 MZ(Quat(0.f, 0.f, camera->m_objectBase.m_rotation.z));

		m_positionInWorld = math::mul(m_positionInWorld, (MY * MX));
		m_positionInWorld += v3f(
			m_objectBase.m_localPosition.x,
			m_objectBase.m_localPosition.y,
			m_objectBase.m_localPosition.z);

		Mat4 T;
		T.m_data[3] = m_positionInWorld;
		T.m_data[3].w = 1.f;

		Mat4 P(Quat(v4f(-m_objectBase.m_rotation.x + math::degToRad(90.f), 0.f, 0.f, 1.f)));
		Mat4 Y(Quat(v4f(0.f, -m_objectBase.m_rotation.y, 0.f, 1.f)));
		Mat4 R(Quat(v4f(0.f, 0.f, -m_objectBase.m_rotation.z, 1.f)));
		m_viewMatrix = (R*(P * Y)) * T;
		m_viewMatrixInvert = m_viewMatrix;
		m_viewMatrixInvert.invert();
		m_viewProjectionMatrix = m_projectionMatrix * m_viewMatrix;
	}

	void Reset() {
		m_near = 0.01f;
		m_far = 1000.f;
		m_fov = math::degToRad(90.f);
		m_aspect = 800.f / 600.f;

		m_objectBase.m_localPosition = v4f(0.f, 0.f, 0.f, 5.f);
		m_objectBase.m_rotation = v3f(math::degToRad(-45.f), 0.f, 0.f);
	}

	void SetPosition(f32 x, f32 y, f32 z){
		m_objectBase.m_localPosition.set(x, y, z, 1.f);
	}

	void PanMove(const v2f& mouseDelta, f32 deltaTime) {
		v4f vec(
			mouseDelta.x * deltaTime,
			0.f,
			-mouseDelta.y * deltaTime,
			0.f);
		Mat4 MX(Quat(m_objectBase.m_rotation.x, 0.f, 0.f));
		Mat4 MY(Quat(0.f, m_objectBase.m_rotation.y, 0.f));
		//Mat4 MZ(Quat(0.f, 0.f, m_objectBase.m_rotation.z));
		vec = math::mul(vec, MY * MX);
		m_objectBase.m_localPosition += vec;
	}

	void Rotate(const v2f& mouseDelta, f32 deltaTime) {
		m_objectBase.m_rotation.x += mouseDelta.y * deltaTime;
		m_objectBase.m_rotation.y += -mouseDelta.x * deltaTime;
	}

	void Zoom(f32 value) {
		m_objectBase.m_localPosition.w -= value;
		if (m_objectBase.m_localPosition.w < 0.01f)
			m_objectBase.m_localPosition.w = 0.01f;
	}

	void ChangeFOV(f32 mouseDeltaX, f32 deltaTime) {
		m_fov += mouseDeltaX * deltaTime;
		if (m_fov < 0.01f)
			m_fov = 0.01f;
		if (m_fov > math::PI)
			m_fov = math::PI;
	}

	void RotateZ(f32 mouseDeltaX, f32 deltaTime) {
		m_objectBase.m_rotation.z += mouseDeltaX * deltaTime;
	}

};


#endif