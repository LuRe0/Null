#include "Null.h"
#include <iostream>

class Sandbox : public Application
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

Application* CreateApplication()
{
	return new Sandbox;
}