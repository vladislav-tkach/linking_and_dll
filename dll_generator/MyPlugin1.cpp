#include "MyPlugin1.h"

#include <iostream>
#include <string>

#include "IPlugin.h"

std::string MyPlugin1::GetPluginName() const
{
	return "MyPlugin1";
}


void MyPlugin1::Perform()
{
	
}


IPlugin* GetPlugin()
{
	return new MyPlugin1();
}