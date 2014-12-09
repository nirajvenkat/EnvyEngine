#include "renderer.h"
#include "3DEngine.h"
#include "freeLook.h"
#include "freeMove.h"
#include "renderer.h"
#include "framedriver.h"
#include "mastercontroller.h"

// TEMP
#include "renderTask.h"
#include "renderTaskFactory.h"

#define TEST_MC

FrameDriver *gFrameDriver = NULL;

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
	cameraObject->AddChild((new GameObject())
		->AddComponent(new Camera(Matrix4f().InitPerspective(ToRadians(70.0f), Window::GetAspect(), 0.1f, 1000.0f)))
		->AddComponent(new FreeLook())
		->AddComponent(new FreeMove()));
	
	directionalLightObject->GetTransform().SetRot(Quaternion(Vector3f(1,0,0), ToRadians(-45)));
}

int main()
{
#ifdef TEST_MC
	MasterController *mc = new MasterController(60); // New MC, 60FPS target rate
	mc->init();

	// Framedriver
	gFrameDriver = new FrameDriver(mc);
	// gFrameDriver->loadFrames();
	
	mc->run();

	//while (1) {
	//	_sleep(50);
	//	gFrameDriver->tick();
	//}
#endif

	/* Unit test for rendertask
	RenderTask::SimpleMat4 view;
	RenderTask::SimpleMat4 proj;

	memset(&view, 0, sizeof(view));
	memset(&proj, 0, sizeof(proj));
	RenderTask *rt = RenderTaskFactory::createCameraTask(0, view, proj);
	*/

	// Game / Envy Rendering
	TestGame game;
	CoreEngine engine(1366, 720, 60, &game);
	engine.CreateWindow("EnvyEngine");
	engine.Start();
	
	return 0;
}
