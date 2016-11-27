#include <js_environment.h>
#include <cstdint>
#define _JSRT_
#include "ChakraCore.h"


class JsEnvironment final : public IJsEnvironment
{
	JsRuntimeHandle _runtime;
	JsContextRef _context;

public:
	JsEnvironment()
	{
		initialize();
	}

	void setup_game() override {}

	void update_game(float dt) override {}

	void shutdown_game() override {}

	~JsEnvironment()
	{
		uninitialize();
	}

private:
	void initialize()
	{
		if (JsCreateRuntime(JsRuntimeAttributeEnableExperimentalFeatures, nullptr, &_runtime) != JsNoError) {
			_runtime = nullptr;
			return;
		}

		if (JsCreateContext(_runtime, &_context) != JsNoError) {
			uninitialize();
		}
	}

	void uninitialize()
	{
		if (_runtime) {
			JsDisposeRuntime(_runtime);
			_runtime = nullptr;
			_context = nullptr;
		}
	}
};

IJsEnvironment *make_js_environment()
{
	return new JsEnvironment();
}

void destroy_js_environment(IJsEnvironment *jse)
{
	delete jse;
}
