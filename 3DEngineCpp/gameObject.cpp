#include "gameObject.h"
#include "gameComponent.h"
#include "coreEngine.h"
#include <math.h>

float deltaSum;

GameObject::~GameObject()
{
	for(unsigned int i = 0; i < m_components.size(); i++)
		if(m_components[i])
			delete m_components[i];
	
	for(unsigned int i = 0; i < m_children.size(); i++)
		if(m_children[i])
			delete m_children[i];
}

GameObject* GameObject::AddChild(GameObject* child)
{
	m_children.push_back(child); 
	child->GetTransform().SetParent(&m_transform);
	child->SetEngine(m_coreEngine);
	return this;
}

GameObject* GameObject::AddComponent(GameComponent* component)
{
	m_components.push_back(component);
	component->SetParent(this);
	return this;
}

void GameObject::InputAll(float delta)
{
	Input(delta);

	for(unsigned int i = 0; i < m_children.size(); i++)
		m_children[i]->InputAll(delta);
}

void GameObject::UpdateAll(float delta)
{
	Update(delta);

	for(unsigned int i = 0; i < m_children.size(); i++)
		m_children[i]->UpdateAll(delta);
}

void GameObject::RenderAll(Shader* shader, RenderingEngine* renderingEngine)
{
	Render(shader, renderingEngine);

	for(unsigned int i = 0; i < m_children.size(); i++)
		m_children[i]->RenderAll(shader, renderingEngine);
}

void GameObject::Input(float delta)
{
	m_transform.Update();

	for(unsigned int i = 0; i < m_components.size(); i++)
		m_components[i]->Input(delta);
}

void GameObject::Update(float delta)
{
	for(unsigned int i = 0; i < m_components.size(); i++)
		m_components[i]->Update(delta);

	deltaSum += delta;

	if (m_children.size() >= 11){
		float firstX = m_children[7]->GetTransform().GetPos().GetX();
		float firstY = m_children[7]->GetTransform().GetPos().GetY();
		float firstZ = m_children[7]->GetTransform().GetPos().GetZ();

		m_children[7]->GetTransform().SetPos(Vector3f(firstX, firstY + (sin(deltaSum) * 0.5), firstZ));

		firstX = m_children[8]->GetTransform().GetPos().GetX();
		firstY = m_children[8]->GetTransform().GetPos().GetY();
		firstZ = m_children[8]->GetTransform().GetPos().GetZ();

		m_children[8]->GetTransform().SetPos(Vector3f(firstX, firstY + (cos(deltaSum) * 0.5), firstZ));

		//Vector<float, 4U> *vec = new Vector<float, 4U>();
		
		//m_children[9]->GetTransform().GetRot().Lerp(*vec, 1.0f);
			//SetRot(Quaternion(Vector3f(0, 1, 0), ToRadians(-sin(deltaSum))));

		m_children[11]->GetTransform().SetPos(Vector3f(10 * cos(deltaSum), firstY, 30 + 10 * sin(deltaSum)));
		//m_children[10]->GetTransform().SetRot(Quaternion(Vector3f(0, 1, 0), ToRadians(180.0f)));

		//printf("%f\n", m_children[6]->GetTransform().GetPos().GetY());
	}
}

void GameObject::Render(Shader* shader, RenderingEngine* renderingEngine)
{
	for(unsigned int i = 0; i < m_components.size(); i++)
		m_components[i]->Render(shader, renderingEngine);
}

void GameObject::SetEngine(CoreEngine* engine)
{
	if(m_coreEngine != engine)
	{
		m_coreEngine = engine;
		
		for(unsigned int i = 0; i < m_components.size(); i++)
			m_components[i]->AddToEngine(engine);

		for(unsigned int i = 0; i < m_children.size(); i++)
			m_children[i]->SetEngine(engine);
	}
}

std::vector<GameObject*> GameObject::GetAllAttached()
{
	std::vector<GameObject*> result;
	
	for(unsigned int i = 0; i < m_children.size(); i++)
	{
		std::vector<GameObject*> childObjects = m_children[i]->GetAllAttached();
		result.insert(result.end(), childObjects.begin(), childObjects.end());
	}
	
	result.push_back(this);
	return result;
}
