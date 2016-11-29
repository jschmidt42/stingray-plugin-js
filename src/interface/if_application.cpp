#include <js_environment.h>
#include <engine_plugin_api/plugin_api.h>

namespace
{
	JsValueRef CALLBACK create_world(JsValueRef callee, bool constructor_call, JsValueRef *arguments, unsigned short num_args, void *callback_state)
	{
		return JS_INVALID_REFERENCE;
	}

	JsValueRef CALLBACK get_world(JsValueRef callee, bool constructor_call, JsValueRef *arguments, unsigned short num_args, void *callback_state)
	{
		return JS_INVALID_REFERENCE;
	}
}

namespace stingray
{
	void load_application_script_interface(IJsEnvironment *env, GetApiFunction get_engine_api)
	{
		env->add_module_function(L"Application", L"new_world", create_world, nullptr);
		env->add_module_function(L"Application", L"get_world", get_world, nullptr);
	}
}
