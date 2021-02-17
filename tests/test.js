const cubiomes = require('../src/cubiomes');

function originVillage(g){
	let output = false;

	let one = cubiomes.createInt('1');
	let lowermax = cubiomes.shiftLeft(one, 48);
	let uppermax = cubiomes.shiftLeft(one, 64);
	cubiomes.destroyInt(one);

	let lower = cubiomes.createInt('1');
	while(cubiomes.lteq(lower, lowermax) && output === false){
		let { valid, x, z } = cubiomes.getStructurePos("village", lower, 0, 0);

		let upper = cubiomes.createInt('0');
		while(cubiomes.lteq(upper, uppermax)  && output === false){
			let shifted = cubiomes.shiftLeft(upper, 48);
			let seed = cubiomes.add(lower, shifted);
			cubiomes.destroyInt(shifted);
			if(cubiomes.isViableStructurePos("village", "1.16", g, seed, x, z)){
				output = cubiomes.intToString(seed);
			}
			cubiomes.destroyInt(seed);
			cubiomes.increment(upper);
		}
		cubiomes.destroyInt(upper);
		cubiomes.increment(lower);
	}
	cubiomes.destroyInt(lower);
	cubiomes.destroyInt(lowermax);
	cubiomes.destroyInt(uppermax);

	return output;
}

function findJungleEdge(g){
	let output = false;
	let seed = cubiomes.createInt('0');
	while(output === false){
		cubiomes.increment(seed);
		cubiomes.applySeed(g, seed);
		biomeID = cubiomes.getBiomeAtPos(g, 0, 0);
		if(biomeID == 23){
			output = cubiomes.intToString(seed);
		}
	}
	cubiomes.destroyInt(seed);

	return output;
}

let g = cubiomes.createGenerator("1.16");
console.log(originVillage(g));
console.log(findJungleEdge(g));