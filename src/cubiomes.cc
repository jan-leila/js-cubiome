#include <node_api.h>
#include <iostream>
#include "finders.h"
#include "javarnd.h"

namespace cubiomes {

	MCversion getVersion(std::string str){
		if(str == "1.7") return MC_1_7;
		if(str == "1.8") return MC_1_8;
		if(str == "1.9") return MC_1_9;
		if(str == "1.10") return MC_1_10;
		if(str == "1.11") return MC_1_11;
		if(str == "1.12") return MC_1_12;
		if(str == "1.13") return MC_1_13;
		if(str == "1.14") return MC_1_14;
		if(str == "1.15") return MC_1_15;
		if(str == "1.16") return MC_1_16;
		return MC_1_16;
	}

	StructureConfig getStructureConfig(std::string str){
		if(str == "feature") return FEATURE_CONFIG;
		if(str == "igloo_1.12") return IGLOO_CONFIG_112;
		if(str == "swamp_hut_1.12") return SWAMP_HUT_CONFIG_112;
		if(str == "desert_pyramid_1.12") return DESERT_PYRAMID_CONFIG_112;
		if(str == "jungle_pyramid_1.12") return JUNGLE_PYRAMID_CONFIG_112;
		if(str == "ocean_ruin_1.12") return OCEAN_RUIN_CONFIG_115;
		if(str == "shipwreck_1.15") return SHIPWRECK_CONFIG_115;
		if(str == "desert_pyramid") return DESERT_PYRAMID_CONFIG;
		if(str == "igloo") return IGLOO_CONFIG;
		if(str == "jungle_pyramid") return JUNGLE_PYRAMID_CONFIG;
		if(str == "swamp_hut") return SWAMP_HUT_CONFIG;
		if(str == "outpost") return OUTPOST_CONFIG;
		if(str == "village") return VILLAGE_CONFIG;
		if(str == "ocean_ruin") return OCEAN_RUIN_CONFIG;
		if(str == "shipwreck") return SHIPWRECK_CONFIG;
		if(str == "monument") return MONUMENT_CONFIG;
		if(str == "mansion") return MANSION_CONFIG;
		if(str == "ruined_portal") return RUINED_PORTAL_CONFIG;
		return FEATURE_CONFIG;
	}

	// Short hand function for getting a string from args
	char *getString(napi_env env, napi_value in)
	{
		size_t str_size;
		size_t str_size_read;

		napi_get_value_string_utf8(env, in, nullptr, 0, &str_size);
		char *str;
		str = (char*)calloc(str_size + 1, sizeof(char));
		napi_get_value_string_utf8(env, in, str, str_size + 1, &str_size_read);

		return str;
	}

	// Short hand function for getting a number from args
	int64_t getNumberInt(napi_env env, napi_value in){
		int64_t result;
		napi_get_value_int64(env, in, &result);
		return result;
	}

	// Function to get back an int from the heap
	int64_t& getInt(napi_env env, napi_value in){
		void *int_ptr;
		napi_unwrap(env, in, &int_ptr);

		int64_t &value = *static_cast<int64_t *>(int_ptr);

		return value;
	}

	napi_value _createInt(napi_env env, napi_value in){
		int64_t* value = new int64_t;
		*value = strtoull(getString(env, in), nullptr, 10);

		napi_value result;
		napi_create_object(env, &result);
		napi_wrap(env, result, value, nullptr, nullptr, nullptr);

		return result;
	}

	napi_value _createInt(napi_env env, int64_t in){
		int64_t* value = new int64_t;
		*value = in;

		napi_value result;
		napi_create_object(env, &result);
		napi_wrap(env, result, value, nullptr, nullptr, nullptr);

		return result;
	}

	// Function to create an int on the heap
	napi_value createInt(napi_env env, napi_callback_info info){
		size_t argc = 1;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		return _createInt(env, args[0]);
	}

	// Function to get the value of an int on the heap as a string
	napi_value intToString(napi_env env, napi_callback_info info){
		size_t argc = 1;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t &value = getInt(env, args[0]);

		napi_value result;
		napi_create_string_utf8(env, std::to_string(value).c_str(), NAPI_AUTO_LENGTH, &result);

		return result;
	}

	// Function to destroy a created on the heap
	napi_value destroyInt(napi_env env, napi_callback_info info){
		size_t argc = 1;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t &value = getInt(env, args[0]);
		delete &value;

		return nullptr;
	}

	// Function to get back a genorator from the heap
	LayerStack& getGenerator(napi_env env, napi_value in){
		void *g_ptr;
		napi_unwrap(env, in, &g_ptr);

		LayerStack &g = *static_cast<LayerStack *>(g_ptr);

		return g;
	}

	// Function to create a genorator on the heap
	napi_value createGenerator(napi_env env, napi_callback_info info){
		size_t argc = 1;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		char *str_version = getString(env, args[0]);

		MCversion version = getVersion(str_version);

		LayerStack* g = new LayerStack();
		setupGenerator(g, version);

		// Wrap the pointer so js can hold it
		napi_value result;
		napi_create_object(env, &result);
		napi_wrap(env, result, g, nullptr, nullptr, nullptr);

		// Make sure that the generator doesnt get garbage collected
		napi_create_reference(env, result, 1, nullptr);

		return result;
	}

	// Function to get the position of a structure in a seed
	napi_value getStructurePos(napi_env env, napi_callback_info info){
		size_t argc = 4;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		const StructureConfig config = getStructureConfig(getString(env, args[0]));
		uint64_t seed = getInt(env, args[1]);
		int64_t regX = getNumberInt(env, args[2]);
		int64_t regZ = getNumberInt(env, args[3]);

		int valid;
		Pos p = getStructurePos(config, seed, regX, regZ, &valid);

		napi_value result, out_x, out_z, out_valid;
		napi_create_object(env, &result);

		napi_create_int64(env, p.x, &out_x);
		napi_set_named_property(env, result, "x", out_x);

		napi_create_int64(env, p.z, &out_z);
		napi_set_named_property(env, result, "z", out_z);

		napi_create_int64(env, valid, &out_valid);
		napi_set_named_property(env, result, "valid", out_valid);

		return result;
	}

	napi_value _next(napi_env env, napi_callback_info info){
		size_t argc = 2;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t seed = getInt(env, args[0]);
		const int bits = getNumberInt(env, args[1]);

		napi_value result;
		napi_create_int64(env, next(&seed, bits), &result);

		return result;
	}

	napi_value _nextInt(napi_env env, napi_callback_info info){
		size_t argc = 2;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t seed = getInt(env, args[0]);
		const int n = getNumberInt(env, args[1]);

		napi_value result;
		napi_create_int64(env, nextInt(&seed, n), &result);

		return result;
	}

	napi_value _nextLong(napi_env env, napi_callback_info info){
		size_t argc = 1;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t seed = getInt(env, args[0]);

		napi_value result;
		napi_create_int64(env, nextLong(&seed), &result);

		return result;
	}

	napi_value _nextFloat(napi_env env, napi_callback_info info){
		size_t argc = 1;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t seed = getInt(env, args[0]);

		napi_value result;
		napi_create_double(env, (double) nextFloat(&seed), &result);

		return result;
	}

	napi_value _nextDouble(napi_env env, napi_callback_info info){
		size_t argc = 1;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t seed = getInt(env, args[0]);

		napi_value result;
		napi_create_double(env, nextDouble(&seed), &result);

		return result;
	}

	napi_value _applySeed(napi_env env, napi_callback_info info){
		size_t argc = 2;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		LayerStack& g = getGenerator(env, args[0]);
		int64_t seed = getInt(env, args[1]);

		applySeed(&g, seed);

		return nullptr;
	}

	napi_value _isViableStructurePos(napi_env env, napi_callback_info info){
		size_t argc = 6;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int structureType = getStructureConfig(getString(env, args[0])).structType;
		int mcversion = getVersion(getString(env, args[1]));
		LayerStack& g = getGenerator(env, args[2]);
		int64_t seed = getInt(env, args[3]);
		int blockX = (int) getNumberInt(env, args[4]);
		int blockZ = (int)getNumberInt(env, args[5]);

		int viable = isViableStructurePos(structureType, mcversion, &g, seed, blockX, blockZ);

		napi_value result;
		napi_create_int64(env, viable, &result);

		return result;
	}

	napi_value _getBiomeAtPos(napi_env env, napi_callback_info info){
		size_t argc = 3;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		LayerStack& g = getGenerator(env, args[0]);
		int biome = getBiomeAtPos(&g, {getNumberInt(env, args[1]), getNumberInt(env, args[2]) });

		napi_value result;
		napi_create_int64(env, biome, &result);

		return result;
	}

	napi_value _isOceanic(napi_env env, napi_callback_info info){
		size_t argc = 1;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int id = (int) getNumberInt(env, args[0]);
		int oceanic = isOceanic(id);

		napi_value result;
		napi_create_int64(env, oceanic, &result);

		return result;
	}

	napi_value copy(napi_env env, napi_callback_info info){
		size_t argc = 1;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t value = getInt(env, args[0]);

		return _createInt(env, value);
	}

	napi_value add(napi_env env, napi_callback_info info){
		size_t argc = 2;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t a = getInt(env, args[0]);
		int64_t b = getInt(env, args[1]);

		return _createInt(env, a + b);
	}

	napi_value increment(napi_env env, napi_callback_info info){
		size_t argc = 1;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t &a = getInt(env, args[0]);
		a++;

		return nullptr;
	}

	napi_value mod(napi_env env, napi_callback_info info){
		size_t argc = 2;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t a = getInt(env, args[0]);
		int64_t b = getNumberInt(env, args[1]);

		napi_value result;
		napi_create_int64(env, a % b, &result);

		return result;
	}

	napi_value _xor(napi_env env, napi_callback_info info){
		size_t argc = 2;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t a = getInt(env, args[0]);
		int64_t b = getInt(env, args[1]);

		return _createInt(env, a ^ b);
	}

	napi_value shiftLeft(napi_env env, napi_callback_info info){
		size_t argc = 2;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t value = getInt(env, args[0]);
		int64_t bits = getNumberInt(env, args[1]);

		return _createInt(env, value << bits);
	}

	napi_value shiftRight(napi_env env, napi_callback_info info){
		size_t argc = 2;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t value = getInt(env, args[0]);
		int64_t bits = getNumberInt(env, args[1]);

		return _createInt(env, value >> bits);
	}

	napi_value eq(napi_env env, napi_callback_info info)
	{
		size_t argc = 2;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t a = getInt(env, args[0]);
		int64_t b = getInt(env, args[1]);

		napi_value result;

		if (a == b)
		{
			napi_create_int32(env, 1, &result);
		}
		else
		{
			napi_create_int32(env, 0, &result);
		}
		return result;
	}

	napi_value lt(napi_env env, napi_callback_info info)
	{
		size_t argc = 2;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t a = getInt(env, args[0]);
		int64_t b = getInt(env, args[1]);

		napi_value result;

		if (a < b)
		{
			napi_create_int32(env, 1, &result);
		}
		else
		{
			napi_create_int32(env, 0, &result);
		}
		return result;
	}

	napi_value gt(napi_env env, napi_callback_info info)
	{
		size_t argc = 2;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t a = getInt(env, args[0]);
		int64_t b = getInt(env, args[1]);

		napi_value result;

		if (a > b)
		{
			napi_create_int32(env, 1, &result);
		}
		else
		{
			napi_create_int32(env, 0, &result);
		}
		return result;
	}

	napi_value lteq(napi_env env, napi_callback_info info)
	{
		size_t argc = 2;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t a = getInt(env, args[0]);
		int64_t b = getInt(env, args[1]);

		napi_value result;

		if (a <= b)
		{
			napi_create_int32(env, 1, &result);
		}
		else
		{
			napi_create_int32(env, 0, &result);
		}
		return result;
	}

	napi_value gteq(napi_env env, napi_callback_info info)
	{
		size_t argc = 2;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t a = getInt(env, args[0]);
		int64_t b = getInt(env, args[1]);

		napi_value result;

		if (a >= b)
		{
			napi_create_int32(env, 1, &result);
		}
		else
		{
			napi_create_int32(env, 0, &result);
		}
		return result;
	}

	napi_status addFunction(napi_env env, napi_value exports, const char *name, napi_callback cb){
		napi_status status;
		napi_value fn;

		status = napi_create_function(env, nullptr, 0, cb, nullptr, &fn);
		if (status != napi_ok) return status;
		status = napi_set_named_property(env, exports, name, fn);
		return status;
	}

	napi_value init(napi_env env, napi_value exports) {
		initBiomes();

		napi_status status;

		status = addFunction(env, exports, "createInt", createInt);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "intToString", intToString);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "destroyInt", destroyInt);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "createGenerator", createGenerator);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "getStructurePos", getStructurePos);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "next", _next);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "nextInt", _nextInt);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "nextLong", _nextLong);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "nextFloat", _nextFloat);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "nextDouble", _nextDouble);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "applySeed", _applySeed);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "isViableStructurePos", _isViableStructurePos);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "getBiomeAtPos", _getBiomeAtPos);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "isOceanic", _isOceanic);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "copy", copy);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "add", add);
		if (status != napi_ok) return nullptr;
		
		status = addFunction(env, exports, "increment", increment);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "mod", mod);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "xor", _xor);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "shiftLeft", shiftLeft);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "shiftRight", shiftRight);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "eq", eq);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "lt", lt);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "gt", gt);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "lteq", lteq);
		if (status != napi_ok) return nullptr;

		status = addFunction(env, exports, "gteq", gteq);
		if (status != napi_ok) return nullptr;

		return exports;
	}

	NAPI_MODULE(NODE_GYP_MODULE_NAME, init)
} // namespace cubiomes