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

	char* getString(napi_env env, napi_value in){
		size_t str_size;
		size_t str_size_read;

		napi_get_value_string_utf8(env, in, nullptr, 0, &str_size);
		char *str;
		str = (char*)calloc(str_size + 1, sizeof(char));
		napi_get_value_string_utf8(env, in, str, str_size + 1, &str_size_read);

		return str;
	}

	int64_t getInt(napi_env env, napi_value in){
		int64_t result;
		napi_get_value_int64(env, in, &result);
		return result;
	}

	napi_value createGenerator(napi_env env, napi_callback_info info){
		size_t argc = 1;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		char *str_version = getString(env, args[0]);

		MCversion version = getVersion(str_version);

		LayerStack g;
		setupGenerator(&g, version);

		// Wrap the pointer so js can hold it
		napi_value result;
		napi_create_object(env, &result);
		napi_wrap(env, result, &g, nullptr, nullptr, nullptr);

		// Make sure that the generator doesnt get garbage collected
		napi_create_reference(env, result, 1, nullptr);

		return result;
	}

	LayerStack* getGenerator(napi_env env, napi_value in){
		void *g_ptr;
		napi_unwrap(env, in, &g_ptr);

		LayerStack *g = static_cast<LayerStack *>(g_ptr);

		return g;
	}

	napi_value getStructurePos(napi_env env, napi_callback_info info){
		size_t argc = 4;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		const StructureConfig config = getStructureConfig(getString(env, args[0]));
		uint64_t seed = strtoull(getString(env, args[1]), nullptr, 10);
		int64_t regX = getInt(env, args[2]);
		int64_t regZ = getInt(env, args[3]);

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
		const int bits = getInt(env, args[1]);

		napi_value result;
		napi_create_int64(env, next(&seed, bits), &result);

		return result;
	}

	napi_value _nextInt(napi_env env, napi_callback_info info)
	{
		size_t argc = 2;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t seed = strtoull(getString(env, args[0]), nullptr, 10);
		const int n = getInt(env, args[1]);

		napi_value result;
		napi_create_int64(env, nextInt(&seed, n), &result);

		return result;
	}

	napi_value _nextLong(napi_env env, napi_callback_info info){
		size_t argc = 1;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t seed = strtoull(getString(env, args[0]), nullptr, 10);

		napi_value result;
		napi_create_int64(env, nextLong(&seed), &result);

		return result;
	}

	napi_value _nextFloat(napi_env env, napi_callback_info info){
		size_t argc = 1;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t seed = strtoull(getString(env, args[0]), nullptr, 10);

		napi_value result;
		napi_create_double(env, (double) nextFloat(&seed), &result);

		return result;
	}

	napi_value _nextDouble(napi_env env, napi_callback_info info){
		size_t argc = 1;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t seed = strtoull(getString(env, args[0]), nullptr, 10);

		napi_value result;
		napi_create_double(env, nextDouble(&seed), &result);

		return result;
	}

	napi_value _applySeed(napi_env env, napi_callback_info info){
		size_t argc = 2;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int64_t seed = strtoull(getString(env, args[1]), nullptr, 10);

		applySeed(getGenerator(env, args[0]), seed);

		return nullptr;
	}

	napi_value _isViableStructurePos(napi_env env, napi_callback_info info){
		size_t argc = 6;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int structureType = getStructureConfig(getString(env, args[0])).structType;
		int mcversion = getVersion(getString(env, args[1]));
		LayerStack* g = getGenerator(env, args[2]);
		int64_t seed = strtoull(getString(env, args[3]), nullptr, 10);
		int blockX = (int) getInt(env, args[4]);
		int blockZ = (int) getInt(env, args[5]);

		int viable = isViableStructurePos(structureType, mcversion, g, seed, blockX, blockZ);

		napi_value result;
		napi_create_int64(env, viable, &result);

		return result;
	}

	napi_value _getBiomeAtPos(napi_env env, napi_callback_info info){
		size_t argc = 3;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		LayerStack *g = getGenerator(env, args[0]);
		int blockX = (int) getInt(env, args[1]);
		int blockZ = (int) getInt(env, args[2]);

		int biome = getBiomeAtPos(g, { blockX, blockZ });

		napi_value result;
		napi_create_int64(env, biome, &result);

		return result;
	}

	napi_value _isOceanic(napi_env env, napi_callback_info info){
		size_t argc = 1;
		napi_value args[argc];

		napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);

		int id = (int) getInt(env, args[0]);
		int oceanic = isOceanic(id);

		napi_value result;
		napi_create_int64(env, oceanic, &result);

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

		return exports;
	}

	NAPI_MODULE(NODE_GYP_MODULE_NAME, init)
} // namespace cubiomes