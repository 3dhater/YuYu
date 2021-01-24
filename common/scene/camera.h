#ifndef _YUYU_SCENE_CAMERA_H_
#define _YUYU_SCENE_CAMERA_H_

enum class yyCameraType : u32
{
	LookAt,
	Free,
	Custom,
	Ortho
};

struct yyCameraFrustum
{
	v4f m_planes[ 6u ];

	/*float DistanceToPlane(v4f vPlane, v4f vPoint)
	{
		return vPoint.dot(vPlane);
	}*/

	// перед использованием обязательно нужно сделать point.KK_W = 1.f
	bool pointInFrustum( const v4f& point )
	{
		for( s32 i = 0; i < 6; ++i )
		{
			if ((
				m_planes[i].x * point.x +
				m_planes[i].y * point.y +
				m_planes[i].z * point.z +
				m_planes[i].w)
				< 0.f)
				return false;
		}
		return true;
	}

	bool sphereInFrustum( f32 radius, const v4f& position )
	{
		for( u32 i = 0u; i < 6u; ++i )
		{
			if(m_planes[i].x * position.x +
				m_planes[i].y * position.y +
				m_planes[i].z * position.z +
				m_planes[i].w <= -radius)
				return false;
		}
		return true;
		/*float dist01 = std::min(DistanceToPlane(m_planes[0], position), DistanceToPlane(m_planes[1], position));
		float dist23 = std::min(DistanceToPlane(m_planes[2], position), DistanceToPlane(m_planes[3], position));
		float dist45 = std::min(DistanceToPlane(m_planes[4], position), DistanceToPlane(m_planes[5], position));
		return std::min(std::min(dist01, dist23), dist45) + radius;*/
	}

	enum FrustumSide
	{
		RIGHT = 0,		// The RIGHT side of the frustum
		LEFT = 1,		// The LEFT	 side of the frustum
		BOTTOM = 2,		// The BOTTOM side of the frustum
		TOP = 3,		// The TOP side of the frustum
		BACK = 4,		// The BACK	side of the frustum
		FRONT = 5			// The FRONT side of the frustum
	};

	void NormalizePlane(v4f& plane)
	{
		// Here we calculate the magnitude of the normal to the plane (point A B C)
		// Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
		// To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
		float magnitude = (float)sqrt(plane.x * plane.x +
			plane.y * plane.y +
			plane.z * plane.z);

		// Then we divide the plane's values by it's magnitude.
		// This makes it easier to work with.
		plane.x /= magnitude;
		plane.y /= magnitude;
		plane.z /= magnitude;
		plane.w /= magnitude;
	}

	void calculateFrustum( Mat4& P, Mat4& V )
	{
		f32 *proj = P.getPtr();
		f32 *modl = V.getPtr();
		float   clip[16]; //clipping planes

		clip[0] = modl[0] * proj[0]   + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
		clip[1] = modl[0] * proj[1]   + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
		clip[2] = modl[0] * proj[2]   + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
		clip[3] = modl[0] * proj[3]   + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

		clip[4] = modl[4] * proj[0]   + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
		clip[5] = modl[4] * proj[1]   + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
		clip[6] = modl[4] * proj[2]   + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
		clip[7] = modl[4] * proj[3]   + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

		clip[8] = modl[8] * proj[0]   + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
		clip[9] = modl[8] * proj[1]   + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
		clip[10] = modl[8] * proj[2]  + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
		clip[11] = modl[8] * proj[3]  + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

		clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
		clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
		clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
		clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];


		//RIGHT 
		m_planes[RIGHT].x = ( clip[ 3 ] - clip[ 0 ]);
		m_planes[RIGHT].y = ( clip[ 7 ] - clip[ 4 ]);
		m_planes[RIGHT].z = ( clip[ 11u ] - clip[ 8 ]);
		m_planes[RIGHT].w = ( clip[ 15u ] - clip[ 12u ]);
		NormalizePlane(m_planes[RIGHT]);

		//LEFT  
		m_planes[LEFT].x = ( clip[ 3 ] + clip[ 0 ]);
		m_planes[LEFT].y = ( clip[ 7 ] + clip[ 4 ]);
		m_planes[LEFT].z = ( clip[ 11u ] + clip[ 8 ]);
		m_planes[LEFT].w = ( clip[ 15u ] + clip[ 12u ]);
		NormalizePlane(m_planes[LEFT]);

		//BOTTOM  
		m_planes[BOTTOM].x = ( clip[ 3 ] + clip[ 1 ]);
		m_planes[BOTTOM].y = ( clip[ 7 ] + clip[ 5 ]);
		m_planes[BOTTOM].z = ( clip[ 11u ] + clip[ 9 ]);
		m_planes[BOTTOM].w = ( clip[ 15u ] + clip[ 13u ]);
		NormalizePlane(m_planes[BOTTOM]);

		//TOP  
		m_planes[TOP].x = ( clip[ 3 ] - clip[ 1 ]);
		m_planes[TOP].y = ( clip[ 7 ] - clip[ 5 ]);
		m_planes[TOP].z = ( clip[ 11u ] - clip[ 9 ]);
		m_planes[TOP].w = ( clip[ 15u ] - clip[ 13u ]);
		NormalizePlane(m_planes[TOP]);

		//FAR  
		m_planes[BACK].x = ( clip[ 3 ] - clip[ 2 ]);
		m_planes[BACK].y = ( clip[ 7 ] - clip[ 6 ]);
		m_planes[BACK].z = ( clip[ 11u ] - clip[ 10u ]);
		m_planes[BACK].w = ( clip[ 15u ] - clip[ 14u ]);
		NormalizePlane(m_planes[BACK]);

		//NEAR  
		m_planes[FRONT].x = ( clip[ 3 ] + clip[ 2 ]);
		m_planes[FRONT].y = ( clip[ 7 ] + clip[ 6 ]);
		m_planes[FRONT].z = ( clip[ 11u ] + clip[ 10u ]);
		m_planes[FRONT].w = ( clip[ 15u ] + clip[ 14u ]);
		NormalizePlane(m_planes[FRONT]);
	}
};


void yyCamera_update(void * impl);
struct yyCamera
{
	yyCamera()
	{
		m_objectBase.m_objectType = yySceneObjectBase::ObjectType::Camera;
		m_objectBase.m_implementationPtr = this;
		m_objectBase.m_updateImplementation = yyCamera_update;

		m_cameraType = yyCameraType::LookAt;
		m_up     = v4f(0.f,1.f,0.f,1.f);
		m_fov    = 0.785398185f;
		m_near   = 1.f;
		m_far    = 100.f;
		m_aspect = 1.333333f;
		m_updateCallback = nullptr;
	}
	~yyCamera(){}

	yySceneObjectBase m_objectBase;

	yyCameraType m_cameraType;
	v4f m_up;
	f32	m_fov;
	f32	m_near;
	f32	m_far;
	f32	m_aspect;
	Mat4 m_viewMatrix;
	Mat4 m_projectionMatrix;
	Mat4 m_rotationMatrix;
	Mat4 m_viewProjectionMatrix;
	Mat4 m_viewProjectionInvertMatrix;
	v4f m_viewport;
	v4f m_target;
	yyCameraFrustum m_frustum;

	void Update()
	{
		//if(m_updateCallback)
		//	m_updateCallback(this);
		//else 
			this->m_objectBase.m_updateImplementation(this);
	}
	void(*m_updateCallback)(yyCamera*);
};

YY_FORCE_INLINE void yyCamera_update(void * impl)
{
	auto camera = (yyCamera*)impl;

	camera->m_objectBase.m_globalMatrix[3].x = -camera->m_objectBase.m_localPosition.x;
	camera->m_objectBase.m_globalMatrix[3].y = -camera->m_objectBase.m_localPosition.y;
	camera->m_objectBase.m_globalMatrix[3].z = -camera->m_objectBase.m_localPosition.z;
	switch(camera->m_cameraType )
	{
	case yyCameraType::LookAt:
		math::makePerspectiveRHMatrix(
			camera->m_projectionMatrix,
			camera->m_fov,
			camera->m_aspect,
			camera->m_near,
			camera->m_far );
		math::makeLookAtRHMatrix(
			camera->m_viewMatrix,
			camera->m_objectBase.m_localPosition,
			camera->m_target,
			camera->m_up
		);
		break;
	case yyCameraType::Free:
	{
		math::makePerspectiveRHMatrix(
			camera->m_projectionMatrix,
			camera->m_fov,
			camera->m_aspect,
			camera->m_near,
			camera->m_far );

		Quat qPitch( v4f( camera->m_objectBase.m_rotation.x, 0.f, 0.f, 1.f ) );
		Quat qYaw( v4f( 0.f, camera->m_objectBase.m_rotation.y, 0.f, 1.f ) );
		Quat qRoll( v4f( 0.f, 0.f, camera->m_objectBase.m_rotation.z, 1.f ) );
		
		Mat4 R;
		Mat4 P;
		Mat4 Y;
		R.setRotation(qRoll);
		P.setRotation(qPitch);
		Y.setRotation(qYaw);

		camera->m_rotationMatrix = R * P * Y;

		camera->m_viewMatrix = camera->m_rotationMatrix * camera->m_objectBase.m_globalMatrix;
	}break;
	case yyCameraType::Custom:
		if(camera->m_updateCallback)
			camera->m_updateCallback(camera);
		break;
	case yyCameraType::Ortho:
		break;
	default:
		break;
	}
	camera->m_viewProjectionMatrix = camera->m_projectionMatrix * camera->m_viewMatrix;
	camera->m_viewProjectionInvertMatrix = camera->m_viewProjectionMatrix;
	camera->m_viewProjectionInvertMatrix.invert();
	camera->m_frustum.calculateFrustum( camera->m_projectionMatrix, camera->m_viewMatrix );
}

#endif