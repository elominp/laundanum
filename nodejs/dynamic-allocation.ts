interface Toto {
    toto: string;
}

function overwrite(a : Toto) : void {
    a.toto = "I overwrote it!";
    // Previous value in a has been freed
}

function init_my_toto(a : Toto) : void {
    a.toto = "Hello, World!".toLowerCase();
}
var a : Toto = {toto: null};
console.log(a);
init_my_toto(a);
console.log(a);
overwrite(a);
console.log(a);