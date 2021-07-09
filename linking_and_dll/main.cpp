#include <cstring>
#include <exception>
#include <filesystem>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <queue>
#include <utility>

#include "Windows.h"

#include "IPlugin.h"

namespace fs = std::filesystem;
using DllGetIPlugin = IPlugin*(*)();

namespace
{
	enum class ArgumentType;

	std::pair<bool, std::string> ProcessArguments(int argc, char** argv, fs::path &path);

	std::queue<std::pair<ArgumentType, std::string>> ParseArguments(int argc, char** argv);
}

int main(int argc, char** argv)
{
	fs::path dlls_dir_path;
	auto [is_arguments_valid, error_message] = ProcessArguments(argc, argv, dlls_dir_path);
	if (!is_arguments_valid) {
		std::cerr << error_message << std::endl;
		return 0;
	}

	fs::directory_iterator dll_it;

	try {
		dll_it = fs::directory_iterator(dlls_dir_path);
	} catch(fs::filesystem_error const& err) {
		std::cerr << err.what() << std::endl <<
			"Try '--help'" << std::endl;
	}

	std::set<std::string> dll_names;

	for (auto const& entry : dll_it) {
		if (entry.path().string().rfind(".dll") != std::string::npos) {
			dll_names.insert(entry.path().string());
		}
	}

	for (auto const& dll_name : dll_names) {
		auto dll = LoadLibraryA(dll_name.c_str());
		if (!dll) {
			std::cerr << "Could not load " << dll_name
				<< ". Try '--help'" << std::endl;

			return 0;
		}

		auto GetPlugin = reinterpret_cast<DllGetIPlugin>(GetProcAddress(dll, "GetPlugin"));

		if (!GetPlugin) {
			throw std::runtime_error("Could not locate function GetPlugin in DLL provided");
		}

		std::unique_ptr<IPlugin> plugin(GetPlugin());

		std::cout << plugin->GetPluginName() << std::endl;
		plugin->Perform();
	}

	return 0;
}

namespace
{
	enum class ArgumentType
	{
		kErrorType,
		kFlag,
		kArgument
	};


	std::pair<bool, std::string> ProcessArguments(int argc, char** argv, fs::path& path)
	{
		unsigned int expected_arguments_count = 2;
		if (argc < expected_arguments_count)
			return { false, "You provided not enough arguments. Try '--help'" };
		else if (argc > expected_arguments_count)
			return { false, "You provided too many arguments. Try '--help'" };

		auto arguments = ParseArguments(argc, argv);

		while (arguments.size() != 0) {
			auto [argument_type, argument] = arguments.front();
			arguments.pop();

			if (argument_type == ArgumentType::kErrorType) {
				return { false, argument + " is unknown. Try '--help'" };
			}
			else if (argument_type == ArgumentType::kFlag) {
				if (argument == "--help" || argument == "-h")
					return { false, "You have to provide a path to "
						"the directory fith dlls as an only argument. "
						"The path must meet Windows path requirements" };
				else
					return { false, argument + " is unknown. Try '--help'" };
			}
			else if (argument_type == ArgumentType::kArgument) {
				path = argument;
			}
		}

		return { true, "" };
	}

	std::queue<std::pair<ArgumentType, std::string>> ParseArguments(int argc, char** argv)
	{
		std::queue<std::pair<ArgumentType, std::string>> result;

		for (int i = 1; i < argc; ++i) {
			if (strstr(argv[i], "--") == argv[i] ||
				strstr(argv[i], "-") == argv[i]) {
				result.push({ ArgumentType::kFlag, argv[i] });
			} else {
				result.push({ ArgumentType::kArgument, argv[i] });
			}
		}

	return result;
	}
}