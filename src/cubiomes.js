const cubiomes = require('../build/Release/cubiomes');

module.exports = {
	createGenerator: cubiomes.createGenerator,
	getStructurePos: cubiomes.getStructurePos,
	next: cubiomes.next,
	nextInt: cubiomes.nextInt,
	nextLong: cubiomes.nextLong,
	nextFloat: cubiomes.nextFloat,
	nextDouble: cubiomes.nextDouble,
	applySeed: cubiomes.applySeed,
	isViableStructurePos: cubiomes.isViableStructurePos,
	getBiomeAtPos: cubiomes.getBiomeAtPos,
	isOceanic: cubiomes.isOceanic,
}