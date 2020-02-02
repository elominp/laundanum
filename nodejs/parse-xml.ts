import { parseString } from 'xml2js';

parseString(`<toto>
<tata>42</tata>
<titi>[
    <ano>
        <foo>Hello, World!</foo>
        <bar>3.14</bar>
    </ano>,
    I am a string,
    null
]</titi>
</toto>`, (_, result) => console.log(result));