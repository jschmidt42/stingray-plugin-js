#include <js_environment.h>
#include <engine_plugin_api/plugin_api.h>
#include <engine_plugin_api/c_api/c_api_application.h>

namespace
{
	ApplicationCApi *application;

	JsValueRef CALLBACK new_world(JsValueRef callee, bool constructor_call, JsValueRef *arguments, unsigned short num_args, void *callback_state)
	{
		SCRIPT_ASSERT(num_args == 1, "Invalid args");
		auto world = application->new_world(nullptr);
		JsValueRef world_object;
		JsCreateExternalObject(world, nullptr, &world_object);
		return world_object;
	}

	JsValueRef CALLBACK release_world(JsValueRef callee, bool constructor_call, JsValueRef *arguments, unsigned short num_args, void *callback_state)
	{
		void *world;
		JsGetExternalData(arguments[1], &world);
		application->release_world(static_cast<WorldPtr>(world));
		return JS_INVALID_REFERENCE;
	}
}

namespace stingray
{
	void load_application_script_interface(IJsEnvironment *env, GetApiFunction get_api)
	{
		auto engine = (ScriptApi *)get_api(C_API_ID);
		application = engine->Application;
		env->add_module_function(L"Application", L"newWorld", new_world, nullptr);
		env->add_module_function(L"Application", L"releaseWorld", release_world, nullptr);
	}
}
