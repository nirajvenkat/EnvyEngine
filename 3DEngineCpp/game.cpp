#include "game.h"
#include "renderingEngine.h"
#include "mastercontroller.h"
#include "renderer.h"
#include "rendernode.h"
#include "rendertask.h"
#include "time.h"
#include <iostream>
#include <map>
#include "frame.h"
#include "SDL2/SDL.h"

#ifdef TEST_MC
extern MasterController *gMasterController;
extern std::map<RenderNode*, RenderTask*> gTasks;
extern Renderer *gRenderer;
#endif

void Game::Input(float delta)
{
	m_root.InputAll(delta);
}

void Game::Update(float delta)
{
	m_root.UpdateAll(delta);
}

void Game::Render(RenderingEngine* renderingEngine)
{
#ifdef TEST_MC
	static int ct = 0;
	double start = Time::GetTime();
	double t;
	double avg = 0;
	
	std::map<RenderNode*, RenderTask*>::iterator it;
	for (it = gTasks.begin(); it != gTasks.end(); it++) {
		RenderNode *n = it->first;
		RenderTask *t = it->second;

		// This code will be moved to where we need it after network integration.
		if (n->getStatus() == RenderNode::LOADING_DATA) {
			gRenderer->renderTask(t);
			Frame *f = Renderer::convertFinishedTaskToFrame(t);
			n->clearTask();
			n->setFinishedFrame(f);
		}
	}

	t = Time::GetTime() - start;
	if (ct++ == 0)
		avg = t;
	else
		(avg = avg + t) / 2.0;
	if (ct == 60) {
		fprintf(stderr, "Rendered in %lf ms\n", avg);
		ct = 0;
	}
#else
	renderingEngine->Render(&m_root);
#endif
}
