#pragma once

#include <string>
#include <type_traits>

/* ******************************
 * Global manager for shared resources
 * usage: Resources::Load<sf::Image>("name_without_path_ending");
 */
class Resources 
{
public:
	// a linking error indicates that the specific resource load has not been implemented
	// See resource.cpp
	template<typename T>
	static T& Load(const std::string& _name);
};

