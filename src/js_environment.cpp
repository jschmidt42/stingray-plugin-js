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

		JsPropertyIdRef function_property_id;
		JS_ASSERT(JsGetPropertyIdFromName(function, &function_property_id));

		JsValueRef function_object;
		JS_ASSERT(JsCreateFunction(callback, callback_state, &function_object));
		JS_ASSERT(JsSetProperty(module_object, function_property_id, function_object, true));
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

		JsPropertyIdRef module_property_id;
		JS_ASSERT(JsGetPropertyIdFromName(L"stingray", &module_property_id));

		JsValueRef api_object;
		JS_ASSERT(JsGetProperty(global_object, module_property_id, &api_object));

		JsValueType type;
		JS_ASSERT(JsGetValueType(api_object, &type));

		if (type == _JsValueType::JsUndefined) {
			JS_ASSERT(JsCreateObject(&api_object));
			JS_ASSERT(JsSetProperty(global_object, module_property_id, api_object, true));
		}

		return api_object;
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
