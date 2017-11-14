#pragma once

#include <string>

/* ******************************
 * Global manager for shared resources
 * usage: Resources::Load<sf::Image>("name_without_path_ending");
 */
class Resources 
{
public:
	template<typename T>
	static T& Load(const std::string& _name);
};

