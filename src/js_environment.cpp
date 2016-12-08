#include <js_environment.h>
#include <engine_plugin_api/plugin_api.h>
#include <plugin_foundation/allocator.h>
#include <plugin_foundation/string_stream.h>
#include <new>

using namespace stingray_plugin_foundation;

#define JS_ASSERT(v) \
{ \
    JsErrorCode error = (v); \
    if (error != JsNoError) \
    { \
        throw error; \
    } \
}

#define JS_FAIL_RETURN(v) \
{ \
    JsErrorCode error = (v); \
    if (error != JsNoError) \
    { \
        return error; \
    } \
}

namespace
{
	const char *SYSTEM = "Js";
	
	JsErrorCode call_function(JsValueRef object, const wchar_t *property_name, JsValueRef *arguments, unsigned short num_args, JsValueRef *result)
	{
		JsPropertyIdRef property_id;
		JS_FAIL_RETURN(JsGetPropertyIdFromName(property_name, &property_id));

		JsValueRef function;
		JS_FAIL_RETURN(JsGetProperty(object, property_id, &function));

		return JsCallFunction(function, arguments, num_args, result);
	}
	
	JsErrorCode set_callback(JsValueRef object, const wchar_t *property_name, JsNativeFunction callback, void *callback_state)
	{
		JsPropertyIdRef property_id;
		JS_FAIL_RETURN(JsGetPropertyIdFromName(property_name, &property_id));
		JsValueRef function;
		JS_FAIL_RETURN(JsCreateFunction(callback, callback_state, &function));
		return JsSetProperty(object, property_id, function, true);
	}

	JsErrorCode set_property(JsValueRef object, const wchar_t *property_name, JsValueRef property)
	{
		JsPropertyIdRef property_id;
		JS_FAIL_RETURN(JsGetPropertyIdFromName(property_name, &property_id));
		return JsSetProperty(object, property_id, property, true);
	}

	JsValueRef get_property(JsValueRef object, const wchar_t *property_name)
	{
		JsValueRef output;
		JsPropertyIdRef property_id;
		JS_ASSERT(JsGetPropertyIdFromName(property_name, &property_id));
		JS_ASSERT(JsGetProperty(object, property_id, &output));
		return output;
	}
}

class JsEnvironment final : public IJsEnvironment
{
	Allocator &_allocator;
	JsRuntimeHandle _runtime;
	JsContextRef _context;
	JsValueRef _api_object;
	unsigned _current_source_context;
	LoggingApi *_logger;
	ResourceManagerApi *_rm;
	AllocatorApi *_memory;

public:
	JsEnvironment(Allocator &a, GetApiFunction get_api) :
		_allocator(a),_runtime(), _context(), _api_object(), _current_source_context(0u), _logger(), _rm(), _memory()
	{
		_api_object = create_api_object();
		_logger = (LoggingApi *)get_api(LOGGING_API_ID);
		_rm = (ResourceManagerApi *)get_api(RESOURCE_MANAGER_API_ID);
		_memory = (AllocatorApi *)get_api(ALLOCATOR_API_ID);
	}
	
	~JsEnvironment()
	{
		destroy_runtime();
	}

	void init(const char *boot_script_name) override
	{
		const char *type = "js";

		if (!_rm->can_get(type, boot_script_name)) {
			_logger->error(SYSTEM, "Cannot find boot script");
			return;
		}
		
		auto script = (wchar_t *)_rm->get(type, boot_script_name);
		JS_ASSERT(JsSetCurrentContext(_context));

		JsValueRef result;
		JsErrorCode error_code = JsNoError;
		if ((error_code = JsRunScript(script, _current_source_context++, L"", &result)) != JsNoError) {
			switch (error_code) {
				case JsErrorScriptCompile:
					_logger->error(SYSTEM, "Failed to compile boot script");
					break;
				default:
					_logger->error(SYSTEM, "An error occurred while running boot script");
					break;
			}
			JS_ASSERT(JsSetCurrentContext(JS_INVALID_REFERENCE));
			return;
		}

		JsValueRef global_object;
		JS_ASSERT(JsGetGlobalObject(&global_object));

		JsValueRef args[1] = { global_object };
		if ((error_code = call_function(global_object, L"init", args, 1, &result)) != JsNoError) {
			_logger->warning(SYSTEM, "Failed to call setup game");
		}
			
		JS_ASSERT(JsSetCurrentContext(JS_INVALID_REFERENCE));
	}

	void update(float dt) override
	{
		JS_ASSERT(JsSetCurrentContext(_context));
		
		JsValueRef global_object;
		JS_ASSERT(JsGetGlobalObject(&global_object));

		JsValueRef delta, result;
		JsDoubleToNumber(dt, &delta);
		JsValueRef args[2] = { global_object, delta };
		call_function(global_object, L"update", args, 2, &result);
		
		JS_ASSERT(JsSetCurrentContext(JS_INVALID_REFERENCE));
	}

	void render() override
	{
		JS_ASSERT(JsSetCurrentContext(_context));

		JsValueRef global_object;
		JS_ASSERT(JsGetGlobalObject(&global_object));

		JsValueRef result;
		JsValueRef args[1] = { global_object };
		call_function(global_object, L"render", args, 1, &result);

		JS_ASSERT(JsSetCurrentContext(JS_INVALID_REFERENCE));
	}

	void shutdown() override
	{
		JS_ASSERT(JsSetCurrentContext(_context));

		JsValueRef global_object;
		JS_ASSERT(JsGetGlobalObject(&global_object));

		JsValueRef result;
		JsValueRef args[1] = { global_object };
		call_function(global_object, L"shutdown", args, 1, &result);

		JS_ASSERT(JsSetCurrentContext(JS_INVALID_REFERENCE));
	}

	void add_module_function(const wchar_t *module, const wchar_t *function, JsNativeFunction callback, void *callback_state) override
	{
		JS_ASSERT(JsSetCurrentContext(_context));

		JsPropertyIdRef module_property_id;
		JS_ASSERT(JsGetPropertyIdFromName(module, &module_property_id));

		JsValueRef module_object;
		JS_ASSERT(JsGetProperty(_api_object, module_property_id, &module_object));

		JsValueType type;
		JS_ASSERT(JsGetValueType(module_object, &type));
		
		if (type == _JsValueType::JsUndefined) {
			JS_ASSERT(JsCreateObject(&module_object));
			JS_ASSERT(JsSetProperty(_api_object, module_property_id, module_object, true));
		}

		JS_ASSERT(set_callback(module_object, function, callback, callback));
		JS_ASSERT(JsSetCurrentContext(JS_INVALID_REFERENCE));
	}

	JsValueRef console_log(JsValueRef *arguments, unsigned short num_args)
	{
		TempAllocator ta(_memory);
		StringStream ss(ta);
		
		for (auto i = 1u; i < num_args; i++) {
			if (i > 1)
				ss << L" ";
			
			JsValueRef string;
			JsConvertValueToString(arguments[i], &string);
			const wchar_t *s;
			size_t len;
			JsStringToPointer(string, &s, &len);
			ss << s;
		}

		_logger->info(SYSTEM, ss.c_str());
		return JS_INVALID_REFERENCE;
	}

private:
	JsValueRef create_api_object()
	{
		JS_ASSERT(JsCreateRuntime(JsRuntimeAttributeEnableExperimentalFeatures, nullptr, &_runtime));
		JS_ASSERT(JsCreateContext(_runtime, &_context));

		JS_ASSERT(JsSetCurrentContext(_context));

		JsValueRef global_object;
		JS_ASSERT(JsGetGlobalObject(&global_object));

		JsValueRef stingray_object;
		JS_ASSERT(JsCreateObject(&stingray_object));
		JS_ASSERT(set_property(global_object, L"stingray", stingray_object));

		JsValueRef console_object;
		JS_ASSERT(JsCreateObject(&console_object));
		JS_ASSERT(set_property(global_object, L"console", console_object));
		JS_ASSERT(set_callback(console_object, L"log", log, this));

		return stingray_object;
	}

	void destroy_runtime()
	{
		if (_runtime) {
			JsDisposeRuntime(_runtime);
			JsSetCurrentContext(JS_INVALID_REFERENCE);
			_runtime = nullptr;
			_context = nullptr;
		}
	}

	static JsValueRef CALLBACK log(JsValueRef callee, bool constructor_call, JsValueRef *arguments, unsigned short num_args, void *callback_state)
	{
		auto *jse = (JsEnvironment *)callback_state;
		return jse->console_log(arguments, num_args);
	}
};

IJsEnvironment *make_js_environment(Allocator &a, GetApiFunction get_api)
{
	return MAKE_NEW(a, JsEnvironment, a, get_api);
}

void destroy_js_environment(Allocator &a, IJsEnvironment *jse)
{
	MAKE_DELETE(a, jse);
}
