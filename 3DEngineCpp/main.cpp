#include "renderer.h"
#include "3DEngine.h"
#include "freeLook.h"
#include "freeMove.h"
#include "renderer.h"
#include "framedriver.h"
#include "mastercontroller.h"
#include "renderTask.h"
#include "envy_server.h"
#include "time.h"
#include <Mmsystem.h>
#include <Windows.h>
#include <SDL2/SDL.h>

#include "bmpconverter.h"

FrameDriver *gFrameDriver = NULL;
Camera *gCamera;
MasterMode gMode;
Game *gGame;

#ifdef TEST_MC
CoreEngine *gEngine;
#endif

void runNode();

class MCGame : public Game
{
public:
	MCGame() {}
	virtual void Init();
protected:
private:
	MCGame(const MCGame& other) {}
	void operator=(const MCGame& other) {}
};

void MCGame::Init()
{
	GameObject* cameraObject = new GameObject();
	cameraObject->GetTransform().SetPos(Vector3f(15, 10, 15));

	// Give this camera to the renderer class
	gCamera = new Camera(Matrix4f().InitPerspective(ToRadians(70.0f), Window::GetAspect(), 0.1f, 1000.0f));

	cameraObject->AddChild((new GameObject())
		->AddComponent(gCamera)
		->AddComponent(new FreeLook())
		->AddComponent(new FreeMove()));
}

class TestGame : public Game
{
public:
	TestGame() {}
	
	virtual void Init();
protected:
private:
	TestGame(const TestGame& other) {}
	void operator=(const TestGame& other) {}
};

void TestGame::Init()
{
	GameObject* cameraObject = new GameObject();
	GameObject* waterObject = new GameObject();
	GameObject* planeObject = new GameObject();
	GameObject* spotLightObject = new GameObject();
	GameObject* pointLightObject = new GameObject();
	GameObject* directionalLightObject = new GameObject();

	//Demo objects
	GameObject* rabbitObject = new GameObject();
	GameObject* rabbitLightObject = new GameObject();
	GameObject* rabbitLightObject2 = new GameObject();
	GameObject* rabbitLightObject3 = new GameObject();

	GameObject* monkeyObject = new GameObject();
	GameObject* monkeyLightObject = new GameObject();

	planeObject->AddComponent(new MeshRenderer(new Mesh("./res/models/plane3.obj"), new Material(new Texture("bricks2.jpg"), 1, 8, new Texture("bricks2_normal.jpg"))));
	planeObject->GetTransform().SetPos(Vector3f(0, -4, 5));
	planeObject->GetTransform().SetScale(4.0f);

	waterObject->AddComponent(new MeshRenderer(new Mesh("./res/models/plane3.obj"), new Material(new Texture("white.png"), 1, 8, new Texture("water_normal.jpg"))));
	waterObject->GetTransform().SetPos(Vector3f(0, -1, 5));
	waterObject->GetTransform().SetScale(4.0f);
	
	pointLightObject->AddComponent(new PointLight(Vector3f(0,1,0),0.4f,Attenuation(0,0,1)));
	pointLightObject->GetTransform().SetPos(Vector3f(7,0,7));
	
	spotLightObject->AddComponent(new SpotLight(Vector3f(0,1,1),0.4f,Attenuation(0,0,0.1f),0.7f));
	spotLightObject->GetTransform().SetRot(Quaternion(Vector3f(0,1,0), ToRadians(90.0f)));
	
	directionalLightObject->AddComponent(new DirectionalLight(Vector3f(1,1,1), 0.4f));

	//Demo objects
	rabbitObject->AddComponent(new MeshRenderer(new Mesh("./res/models/robbierabbit01.obj"), new Material(new Texture("bricks.jpg"), 1, 8,
																									new Texture("bricks_normal.jpg"))));
	rabbitObject->GetTransform().SetPos(Vector3f(0, 6, 5));
	rabbitObject->GetTransform().SetScale(1.0f);

	rabbitLightObject->AddComponent(new SpotLight(Vector3f(0, 0, 10), 0.4f, Attenuation(0, 0, 0.1f), 0.7f));
	rabbitLightObject->GetTransform().SetPos(Vector3f(0, 8, 1));

	rabbitLightObject2->AddComponent(new PointLight(Vector3f(10, 10, 0), 0.4f, Attenuation(0, 0, 0.1f)));
	rabbitLightObject2->GetTransform().SetPos(Vector3f(0, 16, 10));

	rabbitLightObject3->AddComponent(new DirectionalLight(Vector3f(1, 0, 0), 0.2f));
	rabbitLightObject3->GetTransform().SetPos(Vector3f(0, 20, 0));
	rabbitLightObject3->GetTransform().SetRot(Quaternion(Vector3f(0, 1, 0), ToRadians(180.0f)));

	monkeyObject->AddComponent(new MeshRenderer(new Mesh("./res/models/monkey3.obj"), new Material(new Texture("white.png"), 1, 8,
																									new Texture("bricks_normal.jpg"))));
	monkeyObject->GetTransform().SetPos(Vector3f(0, 15, 30));
	monkeyObject->GetTransform().SetRot(Quaternion(Vector3f(0, 1, 0), ToRadians(180.0f)));
	monkeyObject->GetTransform().SetScale(5.0);

	monkeyLightObject->AddComponent(new PointLight(Vector3f(0, 10, 10), 0.4f, Attenuation(0, 0, 0.1f)));
	monkeyLightObject->GetTransform().SetPos(Vector3f(0, 15, 40));

	
	//GameObject* testMesh1 = new GameObject();
	//GameObject* testMesh2 = new GameObject();
	//
	//testMesh1->AddComponent(new MeshRenderer(new Mesh("./res/models/plane3.obj"), new Material(new Texture("bricks2.jpg"), 1, 8)));
	//testMesh2->AddComponent(new MeshRenderer(new Mesh("./res/models/plane3.obj"), new Material(new Texture("bricks2.jpg"), 1, 8)));
	//
	//testMesh1->GetTransform().SetPos(Vector3f(0, 2, 0));
	//testMesh1->GetTransform().SetRot(Quaternion(Vector3f(0,1,0), 0.4f));
	//testMesh1->GetTransform().SetScale(1.0f);
	//
	//testMesh2->GetTransform().SetPos(Vector3f(0, 0, 25));
	//
	//testMesh1->AddChild(testMesh2);
	
	AddToScene(cameraObject);
	AddToScene(waterObject);
	AddToScene(planeObject);
	AddToScene(pointLightObject);
	AddToScene(spotLightObject);
	AddToScene(directionalLightObject);
	//AddToScene(testMesh1);
	AddToScene(rabbitObject);
	AddToScene(rabbitLightObject);
	AddToScene(rabbitLightObject2);
	AddToScene(rabbitLightObject3);
	AddToScene(monkeyObject);
	AddToScene(monkeyLightObject);

	cameraObject->GetTransform().SetPos(Vector3f(15, 10, 15));

	// Give this camera to the renderer class
	gCamera = new Camera(Matrix4f().InitPerspective(ToRadians(70.0f), Window::GetAspect(), 0.1f, 1000.0f));

	cameraObject->AddChild((new GameObject())
		->AddComponent(gCamera)
		->AddComponent(new FreeLook())
		->AddComponent(new FreeMove()));
	
	directionalLightObject->GetTransform().SetRot(Quaternion(Vector3f(1,0,0), ToRadians(-45)));
}

int main(int argc, char **argv)
{
	char *mainFlag;

	// Get command line arguments.
	gMode = MASTER_CONTROLLER; // Default
	if (argc > 1) {
		mainFlag = argv[1];
		if (!strcmp(mainFlag, "-master")) {
			gMode = MASTER_CONTROLLER;
		}
		else if (!strcmp(mainFlag, "-node")) {
			gMode = RENDER_NODE;
		}
	}

	/* Unit test for rendertask
	RenderTask::SimpleMat4 view;
	RenderTask::SimpleMat4 proj;

	memset(&view, 0, sizeof(view));
	memset(&proj, 0, sizeof(proj));
	RenderTask *rt = RenderTaskFactory::createCameraTask(0, view, proj);
	*/

	MCGame mcGame;
	MasterController *mc = NULL;

	// Announce
	switch (gMode) {
	case MASTER_CONTROLLER:
		fprintf(stderr, "Master controller mode.\n");
		PlaySound(TEXT("jaz.wav"), NULL, SND_FILENAME | SND_ASYNC);
		mc = new MasterController(60, &mcGame); // New MC, 60FPS target rate
		mc->init(1366, 720);
		break;
	case RENDER_NODE:
		fprintf(stderr, "Render node mode.\n");
		runNode();
		break;
	}

#ifdef TEST_MC
	gEngine = &engine;
	MasterController *mc = new MasterController(60, &game); // New MC, 60FPS target rate
	mc->init(1366, 720);

	// Framedriver
	gFrameDriver = new FrameDriver(mc);
	// gFrameDriver->loadFrames();

	mc->run();

	//while (1) {
	//	_sleep(50);
	//	gFrameDriver->tick();
	//}
#endif
	
	return 0;
}

void runNode() {
	TestGame *game = new TestGame(); // For camera position, input.
	
	// Game / Envy Rendering
	double timeStep = 1.0/60.0;
	RenderingEngine *renderingEngine;
	Renderer *renderer;
	CoreEngine *engine = new CoreEngine(1366, 720, 60, game);
	engine->CreateWindowCE("EnvyEngine");
	game->Init();

	renderingEngine = engine->GetRenderingEngine();
	renderer = new Renderer(game);
	renderer->setDimensions(1366, 720);
	renderer->setCoreEngine(engine);
	game->Update((float)timeStep);
	gGame = game;

	// Test jpeg
	/*
	char *pix;
	game->Render(renderingEngine);
	Window::Render();
	game->Render(renderingEngine);
	Window::Render();

	Gdiplus::Bitmap *origImage = renderer->getFrameBuffer((void**)&pix, NULL);
	size_t finalSize;
	BYTE *jpegBytes = convertBMP(origImage, 75, &finalSize);
	Gdiplus::Bitmap *inflatedImage = convertJPG(jpegBytes, finalSize);

	int width = inflatedImage->GetWidth();
	int height = inflatedImage->GetHeight();

	CLSID imageCLSID;
	GetEncoderClsid(L"image/png", &imageCLSID);
	inflatedImage->Save(L"H:\\inflatedimage.png", &imageCLSID);
	//Gdiplus::Bitmap *origCopy = inflatedImage->Clone(0, 0,
	//												 inflatedImage->GetWidth(), 
	//	                                             inflatedImage->GetHeight(),
	//												 PixelFormat32bppARGB);

	SDL_Rect tempRect;
	tempRect.x = tempRect.y = 0;
	tempRect.w = 1366;
	tempRect.h = 720;
	//Frame *f = new Frame(&tempRect, Time::GetTime());
	Gdiplus::BitmapData bmd;
	inflatedImage->LockBits(&Gdiplus::Rect(0, 0, width, height),Gdiplus::ImageLockModeRead,PixelFormat32bppARGB,&bmd);
	
	SDL_Surface *newSurf = SDL_CreateRGBSurfaceFrom(bmd.Scan0, width, height, 32, bmd.Stride, 
						  0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
	inflatedImage->UnlockBits(&bmd);

	SDL_Window *renderWindow = SDL_CreateWindow("Jpeg Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height,
		0);

	SDL_SaveBMP(newSurf, "H:\\Test.bmp");

	// Create our renderer
	SDL_Renderer *r = SDL_CreateRenderer(renderWindow, -1, SDL_RENDERER_ACCELERATED);
	SDL_Texture *tex = SDL_CreateTextureFromSurface(r, newSurf);
	if (!tex) {
		fprintf(stderr, "%s", SDL_GetError());
	}
	SDL_RenderCopy(r, tex, NULL, NULL);
	SDL_RenderPresent(r);
	*/

	nodeMain();
}