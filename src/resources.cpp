#include <string>
#include <unordered_map>

#include "SFML/Graphics.hpp"
#include "resources.hpp"


namespace Details {

		template<class Ty, class T, typename FuncType = decltype(&T::loadFromFile), FuncType Fn = &T::loadFromFile>
		 T& Load(const std::string& _name)
		{
			static std::unordered_map<std::string, T> map;

			auto it = map.find(_name);

			if (it == map.end())
			{
				T obj;
				(obj.*Fn)(Ty::ROOT + _name + Ty::FILE_ENDING);
				map.emplace(_name, std::move(obj));
			}

			return map[_name];
		}

	template<typename T>
	class PlaceHolder {};
}


// default fall back
template<typename T>
T& Resources::Load(const std::string& _name)
{
    //	static_assert(false, "This managed resource is not implemented.");
}


/**************************************************************
* short variant for SFML based resources
* @params:
*	TYPE		- the type to be loaded
*	TRAITNAME	- simply a name that is used intern that should not collide with anything else
*	ROOTSTR		- relative root path that is used as prefix to the name
*	ENDINGSTR	- standard file ending that is appended as a postfix
*/
#define SFML_RESOURCE(TYPE, ROOTSTR, ENDINGSTR)			            	\
    namespace Details{                                                  \
    template<>                                                          \
    class PlaceHolder<TYPE>                                             \
    {                                                                   \
    public:                                                             \
        constexpr static const char* ROOT = ROOTSTR;                    \
        constexpr static const char* FILE_ENDING = ENDINGSTR;           \
    };                                                                  \
    }																	\
	template<>															\
	TYPE& Resources::Load<TYPE>(const std::string& _s)					\
	{																	\
		return Details::Load< Details::PlaceHolder<TYPE>, TYPE >(_s);	\
	}																	

/**************************************************************
* generic variant for all resources
* @params:
*	TYPE		- the type to be loaded
*	TRAITNAME	- simply a name that is used intern that should not collide with anything else
*	ROOTSTR		- relative root path that is used as prefix to the name
*	ENDINGSTR	- standard file ending that is appended as a postfix
*	LOADFUNC	- name of a member function of TYPE with signature void(const std::string&)
*
* example:
RESOURCE(sf::Image, "../content/texture/", ".png", loadFromFile);
*/
#define RESOURCE(TYPE, ROOTSTR, ENDINGSTR, LOADFUNC)				\
	template<>															\
	class Details::PlaceHolder<TYPE>									\
	{																	\
	public:																\
		constexpr static const char* ROOT = ROOTSTR;					\
		constexpr static const char* FILE_ENDING = ENDINGSTR;			\
	};																	\
																		\
	template<>															\
	TYPE& Resources::Load<TYPE>(const std::string& _s)					\
	{																	\
		return Details::Load< Details::PlaceHolder<TYPE>, TYPE, decltype(&TYPE::LOADFUNC), &TYPE::LOADFUNC >(_s);	\
	}														\


SFML_RESOURCE(sf::Image, "../content/texture/", ".png");

/*
class TextureTrait
{
public:
constexpr static const char* ROOT = "../content/texture/";
constexpr static const char* FILE_ENDING = ".png";
};

template<>
class Resources<sf::Image> : public Details::ResourcesImpl<TextureTrait, sf::Image>
{};*/

