#include <js_environment.h>
#include <engine_plugin_api/plugin_api.h>

IJsEnvironment *js_environment = nullptr;

static IJsEnvironment *get_or_create_environment(GetApiFunction get_engine_api)
{
	return js_environment ? js_environment : (js_environment = make_js_environment());
}

static void setup_game(GetApiFunction get_engine_api)
{
	auto env = get_or_create_environment(get_engine_api);
	env->setup_game();
}

static void update_game(float dt)
{
	js_environment->update_game(dt);
}

static void shutdown_game()
{
	js_environment->shutdown_game();
}

static const char *get_name()
{
	return "js_plugin";
}

static void unload()
{
	destroy_js_environment(js_environment);
	js_environment = nullptr;
}

extern "C" {

	__declspec(dllexport) void *get_plugin_api(unsigned api)
	{
		if (api == PLUGIN_API_ID) {
			static struct PluginApi api = {0};
			api.get_name = get_name;
			api.setup_game = setup_game;
			api.shutdown_game = shutdown_game;
			api.update_game = update_game;
			api.unloaded = unload;
			return &api;
		}
		return nullptr;
	}
}
