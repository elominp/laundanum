import { fork } from 'child_process';

var worker = fork('./my_worker.js', ['1', '1', '1', 'Houston']);
worker.on('message', (message) => {
    // do something on parsed message
});
worker.send({ toto: "Hello, World!", tata: [1, 2, 3] }, (error) => {
    // fail :(
});

// Worker side:
process.on('message', (message) => {
    const ret: Boolean = process.send('Answer something',
    null, null, (error) => {
        // fail :(
    });
});