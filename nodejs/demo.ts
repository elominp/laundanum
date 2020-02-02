import {
    Agent,
    AgentOptions,
    request
} from 'https';
import {
    ClientRequest,
    IncomingMessage
} from 'http';
/*import {
    getCiphers
} from 'tls';
import {
    getCurves
} from 'crypto';*/

const agent_options: AgentOptions = {
    // host: 'localhost', // Domain name of the server we're sending the request to
    // port: 443, // Port of the server we're sending the request to
    enableTrace: true, // Write SSL errors in standard error output if set to true
    // path: '/tmp/mysocket.shm', // Can be used on UNIX sockets
    // socket: mysocket, // Can be used on existing sockets
    keepAlive: true, // Keep the connection opened after the request if the server support it
    keepAliveMsecs: 5000, // Set a timeout of 5s to keep the connection opened set by the option above
    timeout: 10000, // Set a timeout of 10s to connect to the server
    minVersion: 'TLSv1.2', // Require the request to be protected by at least a TLSv1.2 connection
    // pfx: [], // Can be set to provide an array of PKCS12 public/private certificates with their passphrases insted of individual attributes below
    // ca: [], // Can be used to set custom CA root certificates to check identity of peer received certificate
    // crl: [], // Can be used to set a list of revocated peer certificates,
    // cert: [], // Can be used to set custom public PEM certificates to send to peer
    // clientCertEngine: '', // Can be used to set the name of an OpenSSL engine to retrieve the client public certificate
    // key: [], // Can be used to set private certificates, eventually encrypted
    // privateKeyEngine: undefined, // Can be used to set an OpenSSL engine to retrieve the private key
    // privateKeyIdentifier: undefined, // Can be used to set the identifier of the private key to retrieve in the OpenSSL engine set above,
    // passphrase: '', // Can be used to set the passphrase to decipher the private key retrieve by options above,
    // pskCallback: (hint?) => { return null; }, // Can be used to handle pre-shared keys to reduce SSL handshake latency
    // dhparam: undefined, // Can be used to set the private - shared by both parties - DH parameter
    // ecdhCurve: getCurves().join(':').toUpperCase(), // Can be set to give a list of ECDH curve to use with this algorithm
    // checkServerIdentity: (host, cert) => { return undefined; }, // Can be used for custom verifier or ignore server identity
    // sigalgs: 'SHA224,SHA256,SHA384,SHA512,ECDSA,RSA,ECDH,PSK', // Set a custom list of hashing and signature algorithms
    // ciphers: getCiphers().join(':').toUpperCase(), // Set a custom list of ciphersuites
    honorCipherOrder: true, // If set to true, pick the preferred compatible ciphersuite of the server instead of the client
    // secureOptions: 0 // Can be used to set binary flags on OpenSSL options
    // lookup: (hostname, options, callback) => {} // to override default DNS resolver
};

const agent: Agent = new Agent(agent_options);

const my_request: ClientRequest = request('https://www.google.fr/',
{
    agent: agent,
    timeout: 10000, // Set the timeout of the request, in this case 10s
    // protocol: 'https:', // Set the protocol used for requests
    // host: 'google.fr', // Set the host for requests - either hostname or ip
    // hostname: 'google.fr', // Alias for host
    // defaultPort: 443, // Set the port for the HTTP request
    // path: '/', // Set the path of the HTTP request like /myapi/id/comments for example
    // method: 'GET', // Set the HTTP verb to use for request such as GET, POST, PUT, DELETE, HELP, ...etc
    // auth: 'id:secret', // Basic authentication protocol, like SGConnect
    // headers: { 'Accept-Type': '*' }, // Set custom HTTP headers
    // setHost: true, // If set to false, add the HOST header to the HTTP request
    // family: 6, // Uncomment to force IPv6 or set to 4 to force IPv4
    // maxHeaderSize: 65535, // Uncomment to set an higher maximum header length in responses than default 8KB
}, (response: IncomingMessage) => {
    console.log(`HTTP status code: ${response.statusCode}`); // Get HTTP status code of the response
    console.log(`HTTP ${response.statusMessage}`); // Get HTTP status message of the response
    // console.log(response.headers); // Get HTTP headers of the response
    // response.setEncoding('utf8'); // Set body of the response as UTF8 string for decoding
    // response.on('data', (data: Buffer) => { console.log(data.toString()); }); // Print body of the response
});
//my_request.write('Hello, World!'); // Write in the body of the request
my_request.end(); // Send the request and return immediately, when the server answer, the callback will be called
// Don't forget to check if you're request was sent successfully or errored!
console.log('Request sent :)');