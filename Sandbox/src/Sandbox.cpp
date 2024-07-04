#include "Null.h"
#include <iostream>

class Sandbox : public NULLENGINE::Application
{
public:
	Sandbox() : Application()
	{

	}
	~Sandbox()
	{

	}

private:

};

NULLENGINE::Application* NULLENGINE::CreateApplication()
{
	return new Sandbox;
}