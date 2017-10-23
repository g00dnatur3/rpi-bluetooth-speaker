#include "dbus_utils.h"

namespace dbus_utils {

	const char *A2DP_SINK_ENDPOINT = "/MediaEndpoint/A2DPSink";
	const char *A2DP_SINK_UUID = "0000110b-0000-1000-8000-00805f9b34fb";
	const char *BLUEZ_SERVICE = "org.bluez";
	const char *BLUEZ_MEDIA_INTERFACE = "org.bluez.Media1";
	const char *BLUEZ_MEDIA_ENDPOINT_INTERFACE = "org.bluez.MediaEndpoint1";

	#define ENDPOINT_INTROSPECT_XML                                         \
		DBUS_INTROSPECT_1_0_XML_DOCTYPE_DECL_NODE                           \
		"<node>"                                                            \
		" <interface name=\"org.bluez.MediaEndpoint1\">"          			\
		"  <method name=\"SetConfiguration\">"                              \
		"   <arg name=\"transport\" direction=\"in\" type=\"o\"/>"          \
		"   <arg name=\"properties\" direction=\"in\" type=\"ay\"/>"        \
		"  </method>"                                                       \
		"  <method name=\"SelectConfiguration\">"                           \
		"   <arg name=\"capabilities\" direction=\"in\" type=\"ay\"/>"      \
		"   <arg name=\"configuration\" direction=\"out\" type=\"ay\"/>"    \
		"  </method>"                                                       \
		"  <method name=\"ClearConfiguration\">"                            \
		"   <arg name=\"transport\" direction=\"in\" type=\"o\"/>"          \
		"  </method>"                                                       \
		"  <method name=\"Release\">"                                       \
		"  </method>"                                                       \
		" </interface>"                                                     \
		" <interface name=\"org.freedesktop.DBus.Introspectable\">"         \
		"  <method name=\"Introspect\">"                                    \
		"   <arg name=\"data\" type=\"s\" direction=\"out\"/>"              \
		"  </method>"                                                       \
		" </interface>"                                                     \
		"</node>"

	const char *signature_from_basic_type(int type) {
		switch (type) {
			case DBUS_TYPE_BOOLEAN: return DBUS_TYPE_BOOLEAN_AS_STRING;
			case DBUS_TYPE_BYTE: return DBUS_TYPE_BYTE_AS_STRING;
			case DBUS_TYPE_INT16: return DBUS_TYPE_INT16_AS_STRING;
			case DBUS_TYPE_UINT16: return DBUS_TYPE_UINT16_AS_STRING;
			case DBUS_TYPE_INT32: return DBUS_TYPE_INT32_AS_STRING;
			case DBUS_TYPE_UINT32: return DBUS_TYPE_UINT32_AS_STRING;
			case DBUS_TYPE_INT64: return DBUS_TYPE_INT64_AS_STRING;
			case DBUS_TYPE_UINT64: return DBUS_TYPE_UINT64_AS_STRING;
			case DBUS_TYPE_DOUBLE: return DBUS_TYPE_DOUBLE_AS_STRING;
			case DBUS_TYPE_STRING: return DBUS_TYPE_STRING_AS_STRING;
			case DBUS_TYPE_OBJECT_PATH: return DBUS_TYPE_OBJECT_PATH_AS_STRING;
			case DBUS_TYPE_SIGNATURE: return DBUS_TYPE_SIGNATURE_AS_STRING;
			default:
				printf("\n[dbus_error] - Failed to get signature_from_basic_type (type=%d) \n", type);
				return NULL;
		}
	}

	unsigned basic_type_size(int type) {
		switch (type) {
			case DBUS_TYPE_BOOLEAN: return sizeof(dbus_bool_t);
			case DBUS_TYPE_BYTE: return 1;
			case DBUS_TYPE_INT16: return sizeof(dbus_int16_t);
			case DBUS_TYPE_UINT16: return sizeof(dbus_uint16_t);
			case DBUS_TYPE_INT32: return sizeof(dbus_int32_t);
			case DBUS_TYPE_UINT32: return sizeof(dbus_uint32_t);
			case DBUS_TYPE_INT64: return sizeof(dbus_int64_t);
			case DBUS_TYPE_UINT64: return sizeof(dbus_uint64_t);
			case DBUS_TYPE_DOUBLE: return sizeof(double);
			case DBUS_TYPE_STRING:
			case DBUS_TYPE_OBJECT_PATH:
			case DBUS_TYPE_SIGNATURE: return sizeof(char*);
			default:
				printf("\n[dbus_error] - Failed to get basic_type_size (type=%d) \n", type);
				return 0;
		}
	}

	void utils_dbus_append_basic_variant(DBusMessageIter *iter, int type, void *data) {
		DBusMessageIter variant_iter;
		dbus_message_iter_open_container(iter, DBUS_TYPE_VARIANT, signature_from_basic_type(type), &variant_iter);
		dbus_message_iter_append_basic(&variant_iter, type, data);
		dbus_message_iter_close_container(iter, &variant_iter);
	}

	void utils_dbus_append_basic_variant_dict_entry(DBusMessageIter *dict_iter, const char *key, int type, void *data) {
		DBusMessageIter dict_entry_iter;
		dbus_message_iter_open_container(dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &dict_entry_iter);
		dbus_message_iter_append_basic(&dict_entry_iter, DBUS_TYPE_STRING, &key);
		utils_dbus_append_basic_variant(&dict_entry_iter, type, data);
		dbus_message_iter_close_container(dict_iter, &dict_entry_iter);
	}

	void utils_dbus_append_basic_array(DBusMessageIter *iter, int item_type, const void *array, unsigned n) {
		DBusMessageIter array_iter;
		unsigned i;
		unsigned item_size;
		item_size = basic_type_size(item_type);
		dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY, signature_from_basic_type(item_type), &array_iter);
		for (i = 0; i < n; ++i)
			dbus_message_iter_append_basic(&array_iter, item_type, &((uint8_t*) array)[i * item_size]);
		dbus_message_iter_close_container(iter, &array_iter);
	}

	void utils_dbus_append_basic_array_variant(DBusMessageIter *iter, int item_type, const void *array, unsigned n) {
		DBusMessageIter variant_iter;
		char array_signature[2];
		sprintf(array_signature, "a%c", *signature_from_basic_type(item_type));
		dbus_message_iter_open_container(iter, DBUS_TYPE_VARIANT, array_signature, &variant_iter);
		utils_dbus_append_basic_array(&variant_iter, item_type, array, n);
		dbus_message_iter_close_container(iter, &variant_iter);
	}

	void utils_dbus_append_basic_array_variant_dict_entry(
			DBusMessageIter *dict_iter,
			const char *key,
			int item_type,
			const void *array,
			unsigned n) {
		DBusMessageIter dict_entry_iter;
		dbus_message_iter_open_container(dict_iter, DBUS_TYPE_DICT_ENTRY, NULL, &dict_entry_iter);
		dbus_message_iter_append_basic(&dict_entry_iter, DBUS_TYPE_STRING, &key);
		utils_dbus_append_basic_array_variant(&dict_entry_iter, item_type, array, n);
		dbus_message_iter_close_container(dict_iter, &dict_entry_iter);
	}

	void send_and_add_to_pending(
			DBusConnection *connection, DBusMessage *m,
			DBusPendingCallNotifyFunction func, void *call_data) {

		DBusPendingCall *call;
		dbus_connection_send_with_reply(connection, m, &call, -1);
		dbus_pending_call_set_notify(call, func, call_data, NULL);
	}

	const char *dbus_utils_get_error_message(DBusMessage *m) {
		const char *message;
		if (dbus_message_get_signature(m)[0] != 's')
			return "<no explanation>";
		dbus_message_get_args(m, NULL, DBUS_TYPE_STRING, &message, DBUS_TYPE_INVALID);
		return message;
	}

	void register_endpoint_reply(DBusPendingCall *pending, void *endpoint) {
		//printf("\n!! ----- register_endpoint_reply -> endpoint: %s \n", (char*) endpoint);
		DBusMessage *r;
		r = dbus_pending_call_steal_reply(pending);
		if (dbus_message_get_type(r) == DBUS_MESSAGE_TYPE_ERROR) {
			printf("[dbus_error] - %s.RegisterEndpoint() failed: %s: %s \n", BLUEZ_MEDIA_INTERFACE, dbus_message_get_error_name(r),
					dbus_utils_get_error_message(r));
			goto fail;
		}
		fail:
			dbus_message_unref(r);
			free(endpoint);
	}

	void register_endpoint(DBusConnection *connection, const char *adapter_path, const char *endpoint, const char *uuid) {
		//printf("\n!! ----- register_endpoint -> adapter_path: %s, endpoint: %s \n", (char*) adapter_path, (char*) endpoint);
		DBusMessage *m;
		DBusMessageIter i, d;
		uint8_t codec = 0;
		m = dbus_message_new_method_call(BLUEZ_SERVICE, adapter_path, BLUEZ_MEDIA_INTERFACE, "RegisterEndpoint");
		dbus_message_iter_init_append(m, &i);
		dbus_message_iter_append_basic(&i, DBUS_TYPE_OBJECT_PATH, &endpoint);
		dbus_message_iter_open_container(&i, DBUS_TYPE_ARRAY, "{sv}", &d);
		utils_dbus_append_basic_variant_dict_entry(&d, "UUID", DBUS_TYPE_STRING, &uuid);
		utils_dbus_append_basic_variant_dict_entry(&d, "Codec", DBUS_TYPE_BYTE, &codec);
		a2dp_sbc_t capabilities;
		capabilities.channel_mode = SBC_CHANNEL_MODE_MONO | SBC_CHANNEL_MODE_DUAL_CHANNEL | SBC_CHANNEL_MODE_STEREO |
									SBC_CHANNEL_MODE_JOINT_STEREO;
		capabilities.frequency = SBC_SAMPLING_FREQ_16000 | SBC_SAMPLING_FREQ_32000 | SBC_SAMPLING_FREQ_44100 |
								 SBC_SAMPLING_FREQ_48000;
		capabilities.allocation_method = SBC_ALLOCATION_SNR | SBC_ALLOCATION_LOUDNESS;
		capabilities.subbands = SBC_SUBBANDS_4 | SBC_SUBBANDS_8;
		capabilities.block_length = SBC_BLOCK_LENGTH_4 | SBC_BLOCK_LENGTH_8 | SBC_BLOCK_LENGTH_12 | SBC_BLOCK_LENGTH_16;
		capabilities.min_bitpool = MIN_BITPOOL;
		capabilities.max_bitpool = MAX_BITPOOL;
		utils_dbus_append_basic_array_variant_dict_entry(&d, "Capabilities", DBUS_TYPE_BYTE, &capabilities, sizeof(capabilities));
		dbus_message_iter_close_container(&i, &d);
		char *call_data = (char *) malloc(strlen(endpoint)+1);
		strcpy(call_data, endpoint);
		send_and_add_to_pending(connection, m, register_endpoint_reply, call_data);
	}

	DBusMessage *endpoint_set_configuration(DBusConnection *conn, DBusMessage *m, void *userdata) {
		return dbus_message_new_method_return(m);
	}

	DBusMessage *endpoint_select_configuration(DBusConnection *conn, DBusMessage *m, void *userdata) {
		a2dp_sbc_t *cap, config;
		uint8_t *pconf = (uint8_t *) &config;
		int size;
		DBusMessage *r;
		DBusError err;
		//const char *endpoint_path;
		//endpoint_path = dbus_message_get_path(m);
		dbus_error_init(&err);
		if (!dbus_message_get_args(m, &err, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE, &cap, &size, DBUS_TYPE_INVALID)) {
			printf("\n[dbus_error] - endpoint_select_configuration: %s \n", err.message);
			dbus_error_free(&err);
			goto fail;
		}
		if (size != sizeof(config)) {
			printf("\n[dbus_error] - endpoint_select_configuration: Capabilities array has invalid size \n");
			goto fail;
		}
		config.min_bitpool = 2;
		config.max_bitpool = 64;
		config.subbands = 1;
		config.frequency = 2;
		config.channel_mode = 1;
		config.block_length = 1;
		config.allocation_method = 1;
		r = dbus_message_new_method_return(m);
		dbus_message_append_args(r, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE, &pconf, size, DBUS_TYPE_INVALID);
		return r;
		fail:
			r = dbus_message_new_error(m, "org.bluez.Error.InvalidArguments", "Unable to select configuration");
			return r;
	}

	DBusMessage *endpoint_clear_configuration(DBusConnection *conn, DBusMessage *m, void *userdata) {
		DBusMessage *r;
		DBusError err;
		const char *path;
		dbus_error_init(&err);
		if (!dbus_message_get_args(m, &err, DBUS_TYPE_OBJECT_PATH, &path, DBUS_TYPE_INVALID)) {
			printf("\n[dbus_error] - endpoint_clear_configuration: %s \n", err.message);
			dbus_error_free(&err);
			goto fail;
		}
		r = dbus_message_new_method_return(m);
		return r;
		fail:
			r = dbus_message_new_error(m, "org.bluez.Error.InvalidArguments", "Unable to clear configuration");
			return r;
	}

	DBusMessage *endpoint_release(DBusConnection *conn, DBusMessage *m, void *userdata) {
		DBusMessage *r;
		r = dbus_message_new_error(m, "org.bluez.MediaEndpoint1.Error.NotImplemented", "Method not implemented");
		return r;
	}

	DBusHandlerResult endpoint_handler(DBusConnection *c, DBusMessage *m, void *userdata) {
		DBusMessage *r = NULL;
		const char *path, *interface, *member;
		path = dbus_message_get_path(m);
		interface = dbus_message_get_interface(m);
		member = dbus_message_get_member(m);
		printf("\n!! ----- endpoint_handler: path=%s, interface=%s, member=%s \n", path, interface, member);
		if (dbus_message_is_method_call(m, "org.freedesktop.DBus.Introspectable", "Introspect")) {
			const char *xml = ENDPOINT_INTROSPECT_XML;
			r = dbus_message_new_method_return(m);
			dbus_message_append_args(r, DBUS_TYPE_STRING, &xml, DBUS_TYPE_INVALID);
		} else if (dbus_message_is_method_call(m, BLUEZ_MEDIA_ENDPOINT_INTERFACE, "SetConfiguration"))
			r = endpoint_set_configuration(c, m, userdata);
		else if (dbus_message_is_method_call(m, BLUEZ_MEDIA_ENDPOINT_INTERFACE, "SelectConfiguration"))
			r = endpoint_select_configuration(c, m, userdata);
		else if (dbus_message_is_method_call(m, BLUEZ_MEDIA_ENDPOINT_INTERFACE, "ClearConfiguration"))
			r = endpoint_clear_configuration(c, m, userdata);
		else if (dbus_message_is_method_call(m, BLUEZ_MEDIA_ENDPOINT_INTERFACE, "Release"))
			r = endpoint_release(c, m, userdata);
		else
			return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
		if (r != NULL) {
			assert( dbus_connection_send(c, r, NULL) );
			dbus_connection_flush(c);
			dbus_message_unref(r);
		}
		return DBUS_HANDLER_RESULT_HANDLED;
	}

	void endpoint_init(DBusConnection *connection, const char *endpoint) {
		DBusObjectPathVTable vtable_endpoint;
		vtable_endpoint.message_function = endpoint_handler;
		dbus_connection_register_object_path(connection, endpoint, &vtable_endpoint, NULL);
	}

	void endpoint_sink_init(DBusConnection *connection) {
		endpoint_init(connection, A2DP_SINK_ENDPOINT);
	}

	DBusConnection * getSystemBus() {
		DBusConnection *connection = NULL;
		DBusError error;
		dbus_error_init(&error);
		connection = dbus_bus_get(DBUS_BUS_SYSTEM, &error);
		return connection;
	}

	bool is_initialized = false;

	void register_sink_endpoint(uv_work_t *req) {
		dbus_args_t *dbus_args = static_cast<dbus_args_t *>(req->data);
		if (!is_initialized) {
			endpoint_sink_init(getSystemBus());
			is_initialized = true;
		}
		printf("\n!! ----- RegisterSinkEndpoint, adapter_path: %s \n", dbus_args->adapter_path);
		register_endpoint(getSystemBus(), dbus_args->adapter_path, A2DP_SINK_ENDPOINT, A2DP_SINK_UUID);
	}

	void register_sink_endpoint_after(uv_work_t *req, int status) {
		dbus_args_t *dbus_args = static_cast<dbus_args_t *>(req->data);
		Nan::HandleScope scope;
		Local<Function> cb = Nan::New(dbus_args->callback);
		const unsigned argc = 0;
		Local<Value> argv[argc];
		Nan::MakeCallback(Nan::GetCurrentContext()->Global(), cb, argc, argv);
		dbus_args->callback.Reset();
		delete dbus_args;
	}

	NAN_METHOD(RegisterSinkEndpoint) {
		dbus_args_t *dbus_args = new dbus_args_t;
		dbus_args->adapter_path = strdup(*String::Utf8Value(info[0]->ToString()));
		dbus_args->callback.Reset(info[1].As<Function>());
		dbus_args->request.data = dbus_args;
		uv_queue_work(
				uv_default_loop(), &dbus_args->request, register_sink_endpoint, register_sink_endpoint_after);
	}

}
