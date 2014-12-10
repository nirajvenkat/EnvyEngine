#include "camera.h"
#include "renderingEngine.h"
#include "coreEngine.h"

Camera::Camera(const Matrix4f& projection) :
	m_projection(projection) {
	m_slicedProjection = m_projection; // Default to normal slicing
}

Matrix4f Camera::GetViewProjection() const
{
	Matrix4f cameraRotation = GetTransform().GetTransformedRot().Conjugate().ToRotationMatrix();
	Matrix4f cameraTranslation;
	
	cameraTranslation.InitTranslation(GetTransform().GetTransformedPos() * -1);
	
	return m_slicedProjection * cameraRotation * cameraTranslation;
	//return m_projection * cameraRotation * cameraTranslation;
}

void Camera::AddToEngine(CoreEngine* engine)
{
	engine->GetRenderingEngine()->AddCamera(this);
}

void Camera::setSlice(int slices, int idx, float renderHeight) {

	float viewHeight = renderHeight / (float)slices;

	float t0 = -(renderHeight) / 2.0f;
	float t1 = t0 + idx*viewHeight;
	float b1 = t1 + viewHeight;

	m_slicedProjection = m_projection;

	m_slicedProjection[1][1] *= renderHeight / viewHeight;
	m_slicedProjection[2][1] = (t1 + b1) / (b1 - t1);
}