#include <dbus/dbus.h>
#include "utils.h"
#include "a2dp-codecs.h"

namespace dbus_utils {

	using namespace utils;

	struct dbus_args_t {
		char* adapter_path;
	    uv_work_t request;
	    Nan::Persistent<Function> callback;
	};

	NAN_METHOD(RegisterSinkEndpoint);

}
