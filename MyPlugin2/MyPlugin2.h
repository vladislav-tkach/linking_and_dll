#pragma once

#include <string>

#include "IPlugin.h"

class __declspec(dllexport) MyPlugin2 : public IPlugin
{
public:
	std::string GetPluginName() const override;
	void Perform() override;
};

extern "C" __declspec(dllexport) IPlugin * GetPlugin();