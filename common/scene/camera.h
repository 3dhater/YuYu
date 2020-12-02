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
	// перед использованием обязательно нужно сделать point.KK_W = 1.f
	bool pointInFrustum( const v4f& point )
	{
		for( s32 i = 0; i < 6; ++i )
		{
			if( m_planes[i].dot(point) < 0 )
				return false;
		}
		return true;
	}

	bool sphereInFrustum( f32 radius, const v4f& position )
	{
		for( u32 i = 0u; i < 6u; ++i )
		{
			if( ( m_planes[ i ].x * position.x + m_planes[ i ].y * position.y + m_planes[ i ].z * position.z
				+ m_planes[ i ].w ) <= -radius)
				return false;
		}
		return true;
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
		m_planes[ 0u ].x = ( clip[ 3 ] - clip[ 0 ]);
		m_planes[ 0u ].y = ( clip[ 7 ] - clip[ 4 ]);
		m_planes[ 0u ].z = ( clip[ 11u ] - clip[ 8 ]);
		m_planes[ 0u ].w = ( clip[ 15u ] - clip[ 12u ]);
		m_planes[ 0u ].normalize2();

		//LEFT  
		m_planes[ 1 ].x = ( clip[ 3 ] + clip[ 0 ]);
		m_planes[ 1 ].y = ( clip[ 7 ] + clip[ 4 ]);
		m_planes[ 1 ].z = ( clip[ 11u ] + clip[ 8 ]);
		m_planes[ 1 ].w = ( clip[ 15u ] + clip[ 12u ]);
		m_planes[ 1 ].normalize2();

		//BOTTOM  
		m_planes[ 2 ].x = ( clip[ 3 ] + clip[ 1 ]);
		m_planes[ 2 ].y = ( clip[ 7 ] + clip[ 5 ]);
		m_planes[ 2 ].z = ( clip[ 11u ] + clip[ 9 ]);
		m_planes[ 2 ].w = ( clip[ 15u ] + clip[ 13u ]);
		m_planes[ 2 ].normalize2();

		//TOP  
		m_planes[ 3 ].x = ( clip[ 3 ] - clip[ 1 ]);
		m_planes[ 3 ].y = ( clip[ 7 ] - clip[ 5 ]);
		m_planes[ 3 ].z = ( clip[ 11u ] - clip[ 9 ]);
		m_planes[ 3 ].w = ( clip[ 15u ] - clip[ 13u ]);
		m_planes[ 3 ].normalize2();

		//FAR  
		m_planes[ 4 ].x = ( clip[ 3 ] - clip[ 2 ]);
		m_planes[ 4 ].y = ( clip[ 7 ] - clip[ 6 ]);
		m_planes[ 4 ].z = ( clip[ 11u ] - clip[ 10u ]);
		m_planes[ 4 ].w = ( clip[ 15u ] - clip[ 14u ]);
		m_planes[ 4 ].normalize2();

		//NEAR  
		m_planes[ 5 ].x = ( clip[ 3 ] + clip[ 2 ]);
		m_planes[ 5 ].y = ( clip[ 7 ] + clip[ 6 ]);
		m_planes[ 5 ].z = ( clip[ 11u ] + clip[ 10u ]);
		m_planes[ 5 ].w = ( clip[ 15u ] + clip[ 14u ]);
		m_planes[ 5 ].normalize2();
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
	}
	~yyCamera(){}

	yySceneObjectBase m_objectBase;

	yyCameraType m_cameraType = yyCameraType::LookAt;
	v4f m_up     = v4f(0.f,1.f,0.f,1.f);
	f32	m_fov    = 0.785398185f;
	f32	m_near   = 1.f;
	f32	m_far    = 100.f;
	f32	m_aspect = 1.333333f;
	Mat4 m_viewMatrix;
	Mat4 m_projectionMatrix;
	v4f m_viewport;
	v4f m_target;
	yyCameraFrustum m_frustum;

	void Update()
	{
		if(m_updateCallback)
			m_updateCallback(this);
		else this->m_objectBase.m_updateImplementation(this);
	}
	void(*m_updateCallback)(yyCamera*) = nullptr;
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
		math::makeRotationMatrix( R, qRoll );
		math::makeRotationMatrix( P, qPitch );
		math::makeRotationMatrix( Y, qYaw );

		auto rotationMatrix = R * P * Y;

		camera->m_viewMatrix = rotationMatrix * camera->m_objectBase.m_globalMatrix;
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
	camera->m_frustum.calculateFrustum( camera->m_projectionMatrix, camera->m_viewMatrix );
}

#endif