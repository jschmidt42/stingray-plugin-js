#pragma once

class IJsEnvironment;
typedef void *(*GetApiFunction)(unsigned api);

namespace stingray
{
	void load_script_interface(IJsEnvironment *env, GetApiFunction get_api);
}