#pragma once

#include <string>

class IPlugin
{
public:
	IPlugin() = default;
	virtual ~IPlugin() = default;
	virtual std::string GetPluginName() const = 0;
	virtual void Perform() = 0;
};