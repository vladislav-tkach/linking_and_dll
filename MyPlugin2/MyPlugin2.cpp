#include "MyPlugin2.h"

#include <iostream>
#include <string>

#include "IPlugin.h"

std::string MyPlugin2::GetPluginName() const
{
	return "MyPlugin2";
}


void MyPlugin2::Perform()
{

}


IPlugin* GetPlugin()
{
	return new MyPlugin2();
}