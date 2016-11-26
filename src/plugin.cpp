#include <engine_plugin_api/plugin_api.h>

static void setup_game(GetApiFunction get_engine_api)
{
}

static void update_game(float dt)
{
}

static void shutdown_game()
{
}

static const char *get_name()
{
	return "js_plugin";
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
			return &api;
		}
		return nullptr;
	}
}
