#include <js_environment.h>
#include <engine_plugin_api/plugin_api.h>
#include <interface/if_stingray.h>
#include <plugin_foundation/allocator.h>

using namespace stingray_plugin_foundation;

const char *JS_EXTENSION = "js";
const unsigned JS_VERSION = 1;
const char *PLUGIN_NAME = "js_plugin";

IJsEnvironment *js_environment = nullptr;
DataCompilerApi *data_compiler = nullptr;
AllocatorApi *allocator = nullptr;
AllocatorObject *plugin_allocator_object = nullptr;

IJsEnvironment *get_js_environment(GetApiFunction get_api)
{
	if (js_environment)
		return js_environment;

	ApiAllocator a(allocator, plugin_allocator_object);
	js_environment = make_js_environment(a);
	stingray::load_script_interface(js_environment, get_api);
	return js_environment;
}

static const char *get_name()
{
	return PLUGIN_NAME;
}

static void loaded(GetApiFunction get_api)
{
	allocator = (AllocatorApi *)get_api(ALLOCATOR_API_ID);
	plugin_allocator_object = allocator->make_plugin_allocator(PLUGIN_NAME);	
	data_compiler = (DataCompilerApi*)get_api(DATA_COMPILER_API_ID);
}

static void unload()
{
	ApiAllocator a(allocator, plugin_allocator_object);
	destroy_js_environment(a, js_environment);
	js_environment = nullptr;
	allocator->destroy_plugin_allocator(plugin_allocator_object);
}

static DataCompileResult compile(DataCompileParameters *compile_params)
{
	DataCompileResult result = {0};
	return result;
}

static void setup_data_compiler(GetApiFunction get_api)
{
	data_compiler->add_compiler(JS_EXTENSION, JS_VERSION, compile);
}

static void setup_game(GetApiFunction get_api)
{
	get_js_environment(get_api)->setup_game();
}

static void shutdown_game()
{
	js_environment->shutdown_game();
}

static void update_game(float dt)
{
	js_environment->update_game(dt);
}

extern "C" {

	__declspec(dllexport) void *get_plugin_api(unsigned api)
	{
		if (api == PLUGIN_API_ID) {
			static struct PluginApi api = {0};
			api.get_name = get_name;
			api.loaded = loaded;
			api.unloaded = unload;
			api.setup_data_compiler = setup_data_compiler;
			api.setup_game = setup_game;
			api.shutdown_game = shutdown_game;
			api.update_game = update_game;
			return &api;
		}
		return nullptr;
	}
}
