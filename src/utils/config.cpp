#include "config.hpp"
#include "spdlog/spdlog.h"

// default config
#include "input/keyboardinputmanager.hpp"

#include <sstream>
#include <fstream>
#include <string>

using namespace std::string_literals;

namespace Utils {

	Config::Config()
	{
		CreateDefault();
	}

	Config::Config(const std::string& _fileName)
	{
		using namespace std;

		std::ifstream file(_fileName.c_str());
		if (file.fail())
		{
			spdlog::warn("Could not load config file: "s + _fileName);
			CreateDefault();
			Save(CONFIG_PATH);
		}
		else
		{

			while (!file.eof())
			{
				auto section = ReadSection(file);
				m_sections.emplace(section);
			}
		}
	}

	void Config::Save(const std::string& _fileName)
	{
		using namespace std;

		std::ofstream file(_fileName);

		for (auto& [name, section] : m_sections)
		{
			file << '[' << name << ']' << "\n";

			for (auto&[key, value] : section.m_values)
			{
				file << key << "=" << value << "\n";
			}
			file << "\n";
		}
	}

	std::pair<std::string, ConfigSection> Config::ReadSection(std::istream& _stream)
	{
		using namespace std;

		// title
		// [name]
		string line;
		getline(_stream, line);
		const auto begin = line.find('[');
		const auto end = line.find(']');
		if (begin == std::string::npos || end == std::string::npos)
			throw "expected \"[name]\" but found: "s + line;

		const std::string name = line.substr(begin + 1, end - begin);
		std::unordered_map<std::string, std::string> values;

		// key value pairs
		// key=value
		while (!_stream.eof() && _stream.peek() != '[')
		{
			getline(_stream, line);
			const auto pos = line.find('=');

			if (pos == string::npos) continue;

			values[line.substr(0, pos - 1)] = line.substr(pos+1);
		}

		return { name, values };
	}

	void Config::CreateDefault()
	{
		spdlog::info("creating default config");
		m_sections["keyboard1"] = Input::Keyboard1;
		m_sections["keyboard2"] = Input::Keyboard2;
	}
}