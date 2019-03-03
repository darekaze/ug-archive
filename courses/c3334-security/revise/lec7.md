# Web Security

## Web Architecture

### HTTP overview

- Hypertext transfer protocol (HTTP)
- An application layer protocol (Layer 7 in OSI model)
- Client/server mode
- Stateless: Server maintains no information about past client request

## 3 Top Website Vulnerabilities

- Cross-site scripting (XSS)
  - Bad website sends innocent victim a script that steals information from an honest web site.
- Cross-site request forgery (CSRF)
  - Bad website sends browser request to good website, using credentials of an innocent victim.
- SQL Injection
  - Browser sends malicious input to server
  - Bad input validation leads to malicious SQL query

### Non-persistent XSS

Malicious code is executed by the victim’s browser, and the payload is not stored anywhere; instead, it is returned as part of the response HTML that the server sends.

Therefore, the victim is being tricked into sending malicious code to the vulnerable web application, which is then reflected back to the victim’s browser where the XSS payload executes.

Non-Persistent XSS is also called Type 1 XSS because the attack is carried out through a single request/response cycle.

- Exploit the vulnerability of a webpage that the user input is used in the response HTML page.
- E.g.
  - Original link: www.example.com/search.asp?term=lol
  - Malicious link: www.example.com/search.asp?term=\<script\>MaliciousFunction(...)\<script\>
- MaliciousFunction will get valuable information from innocent client/vulnerable web server (e.g., cookies) and returns it to the attacker.

### Persistent XSS

- The payload is saved in a secondary storage (database)
- The damage caused by persistent attack can be more severe than the non-persistent attack

#### Countermeasures

- Client Side Input Validation
- Server Side Filtering
  - Remove \<script\> \& \</script\> tags
  - More advanced protection: filtering of attribute and script values

### Cross-Site Request Forgery (CSRF)

- An attack that **forces an end user to execute unwanted actions** on a web application in which they are currently authenticated.
- CSRF attacks specifically target state-changing requests, not theft of data, since the attacker has no way to see the response to the forged request
  - With a little help of social engineering (such as sending a link via email or chat), an attacker may trick the users of a web application into executing actions of the attacker's choosing
- If the victim is a normal user, a successful CSRF attack can force the user to perform state changing requests like transferring funds, changing their email address, and so forth
- If the victim is an administrative account, CSRF can compromise the entire web application

- Attacker could generate a request from hidden code on a webpage.
- Browser will also attach the Facebook session cookie!
- Now the malicious code can send an HTTP GET/POST request to delete your friends on Facebook!

#### Countermeasures

The *referer* Header

- Server-side solution 
  - Check where the request is coming from
  - Is the HTTP request coming from facebook.com or xyz.com?
  - There is a referer header for each HTTP request

Secret Number in Cookie
- Generate a secret number, X, and store in the cookie
- Put X in the request as well
  - Note: xyz.com cannot access the cookie of facebook.com
- Cons: Burden on the developers

### SQL Injection Attack

- Many Web applications take user input from a form
- Often this user input is used literally in the construction of a SQL query submitted to a database
- A SQL injection attack involves placing SQL statements in the user input

- Using SQL injections, attackers can
  - Add new data to the database
    - Perform an INSERT in the injected SQL
  -  Modify data currently in the database
    - Perform an UPDATE in the injected SQL – Often can gain access to other user’s system 
  - Often can gain access to other user’s system capabilities by obtaining their password

#### Countermeasures

- Use prepared statements with variable binding
- Force the developer to first define all the SQL code, and then pass in each parameter to the query later. 

---
## Certification Authority (CA) \& Digital certificate

- CA: An entity which issues digital certificates to all participants, including itself
- Digital cert.: Data object which binds a set of relevant information together. 
  - The set of information includes the certificate owner's identity and his public key
  - The CA certifies the binding by (digitally) signing over the data

- A digital certificate has the following components:
  - **owner-id and public-key**
    - The identifier (name) and the public key of the owner of the certificate, respectively
  - **other-info**
    - The issuing CA's name, the certificate serial number, the owner's e-mail address and other related information (such as her organization, address, etc.)
  - **signature**
    - the digital signature of the CA
    - The CA uses its private key to sign on the rest of the data in the certificate

### Trusting CA

- By signing the certificate, the CA
  - vouches for the validity of the information contained in the certificate. Specifically, it certifies that the public key contained in the certificate belongs to the participant identified by owner-id. 
  - ensures that data integrity is maintained (by virtue of the signature)

- Our trust in the CA is based on that it
  - has a well defined and rigorously followed procedure for validating the owner's identity and related information in the certificate;
  - uses a secure system to carry out its functions;
  -  has adequate level of responsibility for its liability

### Digital Certificate Standard: X.509

- subject's (owner's) identity
- time of validity of the certification (starting and expiration dates)
- public key of subject (and public key algorithm and key length used)
- name of issuing CA
- serial number of the certificate (as assigned by the issuing CA)
- signature of the CA on this certificate (and signature algorithm used)

## Protecting Web Communication

TWO main approaches

1. Implement security functions at application level.
      - More flexible, more control – may have tighter security
      - S/MIME, PGP, SET, etc.
2. Implement security functions at transport or network level
      - Common service for upper layer applications
      - Less burden on applications
      - Transport layer: SSL/TLS
      - Network layer: IPSec (IETF)

### Transport Layer Security

- Responsible for the authentication and key exchange necessary to establish secure sessions
- It is now recommended to use TLS v1.1 or above version for both clients and servers
- Primarily used for Web (http) traffic but can be used with other application protocols (e.g., ftp, telnet, etc.). 
- When establishing a secure session, the Handshake Protocol manages the following:
  - Cipher suite negotiation
  - Authentication of the server and optionally, the client
  - Session key information exchange

#### TLS Handshake

1. The client sends a "Client hello" message to the server, along with the client's random value and supported cipher suites (E.g., TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384).
2. The server responds by sending a "Server hello" message to the client, along with the server's random value.
3. The server sends its certificate to the client for authentication and may request a certificate from the client. The server sends the "Server hello done" message. 
4. If the server has requested a certificate from the client, the client sends it.
5. The client creates a random Pre-Master Secret and encrypts it with the public key from the server's certificate, sending the encrypted Pre-Master Secret to the server.
6. The server receives the Pre-Master Secret. The server and client each generate the Master Secret and session keys based on the Pre-Master Secret.
7. The client sends "Change cipher spec" notification to server to indicate that the client will start using the new session keys for hashing and encrypting messages. Client also sends "Client finished" message.
8. Server receives "Change cipher spec" and switches its record layer security state to symmetric encryption using the session keys. Server sends "Server finished" message to the client. 
9. Client and server can now exchange application data over the secured channel they have established. All messages sent from client to server and from server to client are encrypted using session key.

### Quick UDP Internet Connections

- A New encrypted-by-default Internet transport protocol, that provides a number of improvements designed to accelerate HTTP traffic as well as make it more secure, with the intended goal of eventually replacing TCP and TLS on the web. 
- Not only does this ensure that the connection is always authenticated and encrypted, but it also makes the initial connection establishment faster as a result
