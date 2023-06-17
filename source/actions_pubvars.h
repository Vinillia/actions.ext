#ifndef _INCLUDE_ACTIONS_PUBVARS_H
#define _INCLUDE_ACTIONS_PUBVARS_H

#include <amtl/am-hashmap.h>

#include <string.h>

#include <string>
#include <array>
#include <string_view>

struct StringPolicy
{
	static inline uint32_t hash(const char* key)
	{
		return ke::FastHashCharSequence(key, strlen(key));
	}
	static inline bool matches(const char* find, const std::string_view& key)
	{
		return key.compare(find) == 0;
	}
};

using HashValue = unsigned int;
using PublicVars = ke::HashMap<std::string_view, HashValue, StringPolicy>;

namespace compile
{
	template<typename T>
	constexpr HashValue hash(const T& to)
	{
		HashValue val = (HashValue)(to);
		val += (val << 3);
		val ^= (val >> 11);
		val += (val << 15);
		return val;
	}

	constexpr HashValue hash(const std::string_view& data)
	{
		HashValue val = data[0];
		for (char c : data)
		{
			val += (val << 3);
			val ^= 0x53AB43DE;
			val += hash(c);
		}
		return val;
	}

	constexpr HashValue hash(const char* data)
	{
		return hash(std::string_view(data));
	}

	template<const std::string_view&... strs>
	struct concat_t
	{
		// https://stackoverflow.com/questions/38955940/how-to-concatenate-static-strings-at-compile-time
		static constexpr auto impl() noexcept
		{
			constexpr std::size_t len = (strs.size() + ... + 0);
			std::array<char, len + 1> arr{};
			auto append = [i = 0, &arr](auto const& s) mutable
			{
				for (auto c : s) arr[i++] = c;
			};
			(append(strs), ...);
			arr[len] = 0;
			return arr;
		}

		// Give the joined string static storage
		static constexpr auto arr = impl();
		// View as a std::string_view
		static constexpr std::string_view value{ arr.data(), arr.size() - 1 };
	};

	template <std::string_view const&... strs>
	static constexpr auto concat = concat_t<strs...>::value;
}

namespace SourcePawn
{
	class IPluginContext;
}

class ActionPublicsManager
{
	friend class SDKActions;

public:
	static inline constexpr std::string_view method_prefix = "&ActionProcessor::";
	static inline constexpr std::string_view pubvar_prefix = "__action_processor_";

public:
	ActionPublicsManager();
	~ActionPublicsManager();

	bool AddHash(const char* pubvar, HashValue hash, bool force = false);
	HashValue GetHash(const char* name);
	const char* GetName(HashValue hash);

	void SyncPlugin(SourcePawn::IPluginContext* pl);

	void SyncListeners(SourcePawn::IPluginContext* pl);
	void SyncEncoders(SourcePawn::IPluginContext* pl);

	bool SetPluginPubVar(SourcePawn::IPluginContext* pl, const char* name, void* value);

private:
	bool IsUnique(HashValue value);
	void InitializePublicVariables();

public:
	PublicVars m_varMap;
};

extern ActionPublicsManager g_publicsManager;

#endif // !_INCLUDE_ACTIONS_PUBVARS_H
