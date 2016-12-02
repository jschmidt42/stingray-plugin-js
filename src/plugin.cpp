#include <js_environment.h>
#include <engine_plugin_api/plugin_api.h>
#include <interface/if_stingray.h>
#include <plugin_foundation/allocator.h>
#include <plugin_foundation/encoding.h>

using namespace stingray_plugin_foundation;

const char *JS_EXTENSION = "js";
const unsigned JS_VERSION = 1;
const char *PLUGIN_NAME = "JsPlugin";

IJsEnvironment *js_environment = nullptr;
DataCompilerApi *data_compiler = nullptr;
DataCompileParametersApi *data_compile_params = nullptr;
AllocatorApi *allocator = nullptr;
AllocatorObject *plugin_allocator_object = nullptr;

IJsEnvironment *get_js_environment(GetApiFunction get_api)
{
	if (js_environment)
		return js_environment;

	ApiAllocator a(allocator, plugin_allocator_object);
	auto rm = (ResourceManagerApi *)get_api(RESOURCE_MANAGER_API_ID);
	js_environment = make_js_environment(a, get_api);
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
	data_compiler = (DataCompilerApi *)get_api(DATA_COMPILER_API_ID);
	data_compile_params = (DataCompileParametersApi *)get_api(DATA_COMPILE_PARAMETERS_API_ID);
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
	DataCompileResult compile_result = {0};
	DataCompileResult read_result = data_compile_params->read(compile_params);
	if (read_result.error)
		return compile_result;
	
	auto num_tokens = read_result.data.len + 1;
	wchar_t *script = (wchar_t *)allocator->allocate(data_compile_params->allocator(compile_params), num_tokens, 4);
	encoding::utf8_to_wstr(read_result.data.p, script, num_tokens);

	compile_result.data.p = (char *)script;
	compile_result.data.len = num_tokens*sizeof(wchar_t);
	return compile_result;
}

static void setup_data_compiler(GetApiFunction get_api)
{
	data_compiler->add_compiler(JS_EXTENSION, JS_VERSION, compile);
}

static void setup_resources(GetApiFunction get_api)
{
	auto rm = (ResourceManagerApi *)get_api(RESOURCE_MANAGER_API_ID);
	rm->register_type(JS_EXTENSION);
}

static void setup_game(GetApiFunction get_api)
{
	get_js_environment(get_api)->init("content/main");
}

static void shutdown_game()
{
	js_environment->shutdown();
}

static void update_game(float dt)
{
	js_environment->update(dt);
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
			api.setup_resources = setup_resources;
			api.setup_game = setup_game;
			api.shutdown_game = shutdown_game;
			api.update_game = update_game;
			return &api;
		}
		return nullptr;
	}
}
