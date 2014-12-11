#include "camera.h"
#include "renderingEngine.h"
#include "coreEngine.h"

// Sliced projection will be a singleton value since the Camera may start
// up after the renderer does.
Matrix4f gSlicedProjection;
Matrix4f gLastDefaultProjection;

Camera::Camera(const Matrix4f& projection) :
	m_projection(projection) {
	gLastDefaultProjection = m_projection; // Default to normal slicing
	gSlicedProjection = gLastDefaultProjection;
}

Matrix4f Camera::GetViewProjection() const
{
	Matrix4f cameraRotation = GetTransform().GetTransformedRot().Conjugate().ToRotationMatrix();
	Matrix4f cameraTranslation;
	
	cameraTranslation.InitTranslation(GetTransform().GetTransformedPos() * -1);
	
	return gSlicedProjection * cameraRotation * cameraTranslation;
	//return m_projection * cameraRotation * cameraTranslation;
}

void Camera::AddToEngine(CoreEngine* engine)
{
	engine->GetRenderingEngine()->AddCamera(this);
}

void Camera::setProjection(Matrix4f *newProjection) {
	m_projection = *newProjection;
}

void Camera::setSlice(int slices, int idx, float renderHeight) {

	float viewHeight = renderHeight / (float)slices;

	float t0 = -(renderHeight) / 2.0f;
	float t1 = t0 + idx*viewHeight;
	float b1 = t1 + viewHeight;

	gSlicedProjection = gLastDefaultProjection;

	gSlicedProjection[1][1] *= renderHeight / viewHeight;
	gSlicedProjection[2][1] = (t1 + b1) / (b1 - t1);
}