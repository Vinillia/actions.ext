#ifndef _INCLUDE_ACTIONS_CONTAINER
#define _INCLUDE_ACTIONS_CONTAINER

#include <list>

namespace actions
{
	template<typename T>
	class link
	{
		link() = default;

		void add(T&& value)
		{
			list.push_back(value);
		}

		void add(const T& value)
		{
			list.push_back(value);
		}

	private:
		std::list<T> list;
	};
}

#endif // !_INCLUDE_ACTIONS_CONTAINER