#include "Null.h"
#include <iostream>
#include "Null/Engine/Submodules/Layers/ImGuiLayer.h"

class Sandbox : public NULLENGINE::Application
{
public:
	Sandbox() : Application()
	{
	}
	~Sandbox()
	{

	}
	void Load() override
	{
		Application::Load();
	}

private:

};

NULLENGINE::Application* NULLENGINE::CreateApplication()
{
	return new Sandbox;
}