{
	"targets": [
		{
			"target_name": "cubiomes",
			"sources": [
				"src/cubiomes.cc",
				"lib/cubiomes/finders.c",
				"lib/cubiomes/generator.c",
				"lib/cubiomes/layers.c",
				"lib/cubiomes/util.c",
				"lib/cubiomes/javarnd.h"
			],
			"include_dirs": [
				"lib/cubiomes",
			]
		}
	]
}
