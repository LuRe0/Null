#include "Null.h"
#include <iostream>


class Sandbox : public NULLENGINE::Application
{
public:
	Sandbox() : Application("Sandbox")
	{
		//add create function to any modules added by user and add them to the modules json file in the order that they need to be read in
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