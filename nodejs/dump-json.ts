const toto = {
    tata: 42,
    titi: [
        {
            foo: 'Hello, World!',
            bar: 3.14 
        },
        "I am a string!",
        null
    ]
};

console.log(JSON.stringify(toto));