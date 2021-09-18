#include "config.hpp"
#include "spdlog/spdlog.h"

// default config
#include "input/keyboardinputmanager.hpp"
#include "input/gamepadinputmanager.hpp"
#include "graphics/device.hpp"

#include <sstream>
#include <fstream>
#include <string>

using namespace std::string_literals;

namespace Game {
	const Utils::ConfigSection::Initializer<int, 2> GamplaySettings(
		{ {
			{"autoChargeJump", 0},
			{"numWinsRequired", 11}
		} });
}

namespace Utils {

	ConfigSection& ConfigSection::operator+=(const ConfigSection& _other)
	{
		m_values.insert(_other.m_values.begin(), _other.m_values.end());

		return *this;
	}

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
			spdlog::info("Creating default config.");
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

		const std::string name = line.substr(begin + 1, end - begin-1);
		std::unordered_map<std::string, std::string> values;

		// key value pairs
		// key=value
		while (!_stream.eof() && _stream.peek() != '[')
		{
			getline(_stream, line);
			const auto pos = line.find('=');

			if (pos == string::npos) continue;

			values[line.substr(0, pos)] = line.substr(pos+1);
		}

		return { name, values };
	}

	void Config::CreateDefault()
	{
		m_sections["keyboard1"] = Input::Keyboard1;
		m_sections["keyboard2"] = Input::Keyboard2;
		m_sections["gamepad1"] = Input::Gamepad1Buttons;
		m_sections["gamepad1"] += Input::Gamepad1Axis;
		m_sections["video"] = Graphics::VideoSettings;
		m_sections["gameplay"] = Game::GamplaySettings;
	}
}