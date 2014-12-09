#include "renderingEngine.h"
#include "window.h"
#include "gameObject.h"
#include "shader.h"
#include "sdl_backend.h"
#include <GL/glew.h>

RenderingEngine::RenderingEngine()
{
	m_samplerMap.insert(std::pair<std::string, unsigned int>("diffuse", 0));
	m_samplerMap.insert(std::pair<std::string, unsigned int>("normalMap", 1));
	
	AddVector3f("ambient", Vector3f(0.1f, 0.1f, 0.1f));
	m_defaultShader = new Shader("forward-ambient");

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glFrontFace(GL_CW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_CLAMP);
}

RenderingEngine::~RenderingEngine() 
{
	if(m_defaultShader) delete m_defaultShader;
}

void RenderingEngine::Render(GameObject* object)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	object->RenderAll(m_defaultShader, this);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_EQUAL);
	
	for(unsigned int i = 0; i < m_lights.size(); i++)
	{
		m_activeLight = m_lights[i];
		object->RenderAll(m_activeLight->GetShader(), this);
	}

	//glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 0, 0, 1920, 1080, 0);
	//SDLSetRenderTarget();
	
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glDisable(GL_BLEND);
}
/*
void RenderingEngine::RenderWater(GameObject* object){
	glViewport(0, 0, texSize, texSize);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(......)

		glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glScalef(1.0, -1.0, 1.0);
	double plane[4] = { 0.0, 1.0, 0.0, 0.0 }; //water at y=0
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);
	RenderScene();
	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();

	//render reflection to texture
	glBindTexture(GL_TEXTURE_2D, reflection);
	//glCopyTexSubImage2D copies the frame buffer
	//to the bound texture
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, texSize, texSize);
}
*/
