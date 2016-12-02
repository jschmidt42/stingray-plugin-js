#include <js_environment.h>
#include <plugin_foundation/allocator.h>
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
	JsValueRef CALLBACK log(JsValueRef callee, bool constructor_call, JsValueRef *arguments, unsigned short num_args, void *callback_state)
	{
		//TODO
		return JS_INVALID_REFERENCE;
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

public:
	JsEnvironment(Allocator &a) : _allocator(a), _runtime(), _context(), _api_object(), _current_source_context(0u)
	{
		_api_object = initialize();
	}
	
	~JsEnvironment()
	{
		uninitialize();
	}

	void setup_game() override
	{
		JS_ASSERT(JsSetCurrentContext(_context));

		const wchar_t *script = L"(()=>{ stingray.Application.create_world(); })()";

		JsValueRef result;
		auto code = JsRunScript(script, _current_source_context++, L"", &result);
			
		JS_ASSERT(JsSetCurrentContext(JS_INVALID_REFERENCE));
	}

	void update_game(float dt) override {}

	void shutdown_game() override {}

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

private:
	JsValueRef initialize()
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

	void uninitialize()
	{
		if (_runtime) {
			JsDisposeRuntime(_runtime);
			JsSetCurrentContext(JS_INVALID_REFERENCE);
			_runtime = nullptr;
			_context = nullptr;
		}
	}
};

IJsEnvironment *make_js_environment(Allocator &a)
{
	return MAKE_NEW(a, JsEnvironment, a);
}

void destroy_js_environment(Allocator &a, IJsEnvironment *jse)
{
	MAKE_DELETE(a, jse);
}
