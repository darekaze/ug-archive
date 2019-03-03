# Auth pt2

## Password Authentication over a Network

### Transmit password in the clear

- Passive attack
  - Password sniffing
- Active attack
  - Open to impersonation of Bob

### Challenge-Response

Let f be a secret function that is known only to Alice and Bob.

In a challenge-response protocol,
- Bob sends a random message C to Alice
- Alice replies with R = f(C)
- Bob validates R by computing it separately

The scheme is open to dictionary attack by eavesdropper or someone impersonating Bob.

### One-time Passwords

A one-time password is a password that is invalidated as soon as it is used.

### Lamport's Hash (S/Key)

- Let H be a one-way hash function
- Alice chooses an initial seed K0
- Calculates
  - K1 = H(K0)
  - K2 = H(K1)
  - ...
  - Kn = H(Kn-1)
- Each of these Ki can then be used as a one-time password

- Setup 
  - Alice sends Kn to Bob
  - Bob initializes the database entry
- Authentication
  - Alice sends Kn-1 to Bob.
  - Bob checks if Kn = H(Kn-1)
  - If yes, Alice is authenticated
    - Bob replaces Kn with Kn-1 in his database

### Weak Password

- Alice has a "weak" password pwd
- Bob stores a hash W = h(pwd) of the password
- Alice's workstation knows how to calculate W on the fly, once Alice types in her password
- Use W in a way that does not give any hints on pwd

### Strong Password Protocols

- EKE is one example of Strong Password Protocols
  - Secure against eavesdropper
    - No available plaintext-ciphertext pairs for dictionary attack
  - Secure against impersonation
    - Malice must guess the correct W in order to impersonate Bob in the first message flow
  - Mutual authentication
    - Bob must know W to encrypt PKE(K)
    - Alice must know W to decrypt PKE(K) and use K to encrypt Ra

## Master Password

Make the master password a good one, apply the appropriate rules to maintain its security, and never remember any of the others passwords derived from it.

### Single Sign-On

- Designed to reduce the volume of authentication information, i.e. the number of passwords, that need to be remembered
- Idea: Sign in once and access a lot of resources
- User in registered with multiple entities which share information
- Centralized authentication generates a "token" which can be used for authentication to access independent systems.

#### Case study - Google Sign-in

- Use OAuth protocol
  - An authorization framework enables a third-party app to obtain limited access to an HTTP service either on behalf of a resource owner
  - By orchestrating an approval interaction between the resource owner and the HTTP service, or
  - By allowing the 3rd-party app to obtain access on its own behalf

- Before an application can access private data using a Google API, it must obtain an **access token** that grants access to that API
- A single access token can grant varying degrees of access to multiple APIs.
- If the user grants the permission, the Google Auth Server sends the application an access token (or an authorization code)
- If the user does not grant the permission, the server returns an error

### Biometrics

Fingerprints, Face recognition, Iris, Handwriting, Voice, etc.

### Multiple Factor Authentication

E.g Password + Credit Card (Card Security Code), Password + Token...

## CAPTCHA

- Protects websites against bots by generating and grading tests that humans can pass but current computer programs cannot
- Google reCAPTCHA: It uses advanced risk analysis techniques, considering the user's entire engagement with the CAPTCHA, and evaluates a broad rage of cues that distinguish humans form bots



