const cubiomes = require('../src/cubiomes');

function originVillage(g){
	for(lower = 0; lower < 0x1000000000000; lower++){
		lower_str = lower.toString();
		let { valid, x, z } = cubiomes.getStructurePos("village", lower_str, 0, 0);
		for(let upper = 0; upper < 0x10000; upper++){
			let seed = upper.toString() + lower_str;
			if (cubiomes.isViableStructurePos("village", "1.16", g, seed, x, z)){
				return seed;
			}
		}
	}
}

function findJungleEdge(g){
	for (seed = 0; seed < 0x1000000000000; seed++){
		// console.log("test1")
		cubiomes.applySeed(g, seed.toString());
		// console.log("test2")
		let biomeID = cubiomes.getBiomeAtPos(g, 0, 0);
		// console.log("test3")
		if(biomeID == 23){
			return seed;
		}
	}
}

let g = cubiomes.createGenerator("1.16");
console.log(originVillage(g));
console.log(findJungleEdge(g));