#pragma once

#include "resources.hpp"

#include <string_view>
#include <unordered_map>
#include <sstream>

namespace Utils {

	class ConfigSection
	{
	public:
		template<typename T, size_t S>
		using Initializer = std::array<std::pair<std::string, T>, S>;

		template<typename T, size_t S>
		ConfigSection(const Initializer<T,S>& _init)
		{
			for (auto&[key, value] : _init)
				m_values[key] = Config::Serialize(value);
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
				return Convert<T>(it->second);
			else return T{};
		}
	private:
		template<typename T>
		static T Convert(const std::string& _value)
		{
			std::stringstream ss(_value);
			T val;
			ss >> val;

			return val;
		}
		template<>
		static std::string Convert(const std::string& _value)
		{
			return _value;
		}

		std::unordered_map<std::string, std::string> m_values;

		friend class Config;
	};

	class Config
	{
	public:
		Config(const std::string& _fileName);
		Config();

		void Save(const std::string& _fileName);

		const ConfigSection& GetSection(const std::string& _name) const { return m_sections.at(_name); };
		ConfigSection& GetSection(const std::string& _name) { return m_sections[_name]; };

		template<typename T>
		static std::string Serialize(const T& _value)
		{
			std::stringstream ss;
			ss << _value;

			std::string s;
			ss >> s;
			return s;
		}
	private:
		void CreateDefault();
		// parser stuff
		static std::pair<std::string, ConfigSection> ReadSection(std::istream& _stream);

		std::unordered_map<std::string, ConfigSection> m_sections;
	};

	constexpr char* CONFIG_PATH = CONTENTPATH("settings.ini");
}