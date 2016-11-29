#include <interface/if_stingray.h>
#include <js_environment.h>
#include <engine_plugin_api/plugin_api.h>

namespace stingray
{
	void load_application_script_interface(IJsEnvironment *env, GetApiFunction get_engine_api);

	void load_script_interface(IJsEnvironment *env, GetApiFunction get_engine_api)
	{
		load_application_script_interface(env, get_engine_api);
	}
}