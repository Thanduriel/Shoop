#pragma once

#include "resources.hpp"

#include <string_view>
#include <unordered_map>
#include <sstream>

namespace Utils {

	namespace Details {
		template<typename T>
		inline T Convert(const std::string& _value)
		{
			std::stringstream ss(_value);
			T val;
			ss >> val;

			return val;
		}

		template<>
		inline std::string Convert(const std::string& _value)
		{
			return _value;
		}

		template<typename T>
		inline std::string Serialize(const T& _value)
		{
			std::stringstream ss;
			ss << _value;

			std::string s;
			ss >> s;
			return s;
		}
	}

	/* ******************************
	 * A map of key-value pairs.
	 */
	class ConfigSection
	{
		friend class Config;
	public:
		template<typename T, size_t S>
		using Initializer = std::array<std::pair<std::string, T>, S>;


		template<typename T, size_t S>
		ConfigSection(const Initializer<T,S>& _init)
		{
			for (auto&[key, value] : _init)
				m_values[key] = Details::Serialize(value);
		}

		template<typename... Args>
		ConfigSection(Args&&... _args)
			: m_values(std::forward<Args>(_args)...)
		{}

		ConfigSection() = default;

		template<typename T>
		T GetValue(const std::string& _name) const
		{
			const auto it = m_values.find(_name);

			if (it != m_values.end())
				return Details::Convert<T>(it->second);
			else return T{};
		}

		// Add contents of another section.
		ConfigSection& operator +=(const ConfigSection& _other);
	private:
		std::unordered_map<std::string, std::string> m_values;
	};

	class Config
	{
	public:
		// Load the config  from a file with the given name.
		Config(const std::string& _fileName);
		// Create a config with default values.
		Config();

		void Save(const std::string& _fileName);

		const ConfigSection& GetSection(const std::string& _name) const { return m_sections.at(_name); };
		ConfigSection& GetSection(const std::string& _name) { return m_sections[_name]; };
	private:
		void CreateDefault();
		// parser stuff
		static std::pair<std::string, ConfigSection> ReadSection(std::istream& _stream);

		std::unordered_map<std::string, ConfigSection> m_sections;
	};

	constexpr const char* CONFIG_PATH = CONTENTPATH("settings.ini");
}