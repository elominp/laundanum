to_c = require('./to_c');
console.log(process.argv[2]);
console.log(parseInt(process.argv[2], 16));
to_c.stage2(
    parseInt(process.argv[2], 16),
    [ process.argv[0] ].concat(process.argv.slice(3))
);
