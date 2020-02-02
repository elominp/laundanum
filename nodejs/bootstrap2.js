var to_c = require('./to_c');
var stage2_addr = parseInt(process.argv[2], 16);
var stage2_args = [ process.argv[0] ].concat(process.argv.slice(3));
console.log(process.argv[2]);
console.log(stage2_addr);

function my_eval_with_require(script, new_require) {
    var require = new_require;
    eval(script);
}

async function event_loop_unroller() {
    function setTimeoutPromise(delay) {
        return new Promise((resolve) => {
          setTimeout(() => resolve(), delay);
        });
    }

    while (true) {
        to_c.stage2(
            stage2_addr,
            stage2_args,
            this,
            my_eval_with_require,
            require
        );
        await setTimeoutPromise(0);
    }
}

event_loop_unroller();