import { Builder } from 'xml2js';

var builder = new Builder();
console.log(builder.buildObject({
    toto: {
        tata: 42,
        titi: [
            {
                foo: "Hello, World!",
                bar: 3.14
            },
            "I am a string",
            null
        ]
    }
}));