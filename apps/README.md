# OrenCash webapps

These web pages are deigned to locally control your OrenCash node. Feel free to make a better use of the OrenCash rest api.

Functions :

### /blocks

Retrieves a list of blocks in JSON format.
opt params:
id : Retrieves a single block by its id
hash : Retrieves a single block by its hash

### /transactions

Retrieves a list of all pending transactions in JSON format.
opt params:
id + blockid : Retrives a single treated transaction by its id and the block its supposed to be in
id : Retrieves a pending transaction by its id

### /miner/status

Result is in JSON format {"Result" : "Mining"/"Stopped"/"No miner available"}

### /miner/start

Starts the miner, returns the new miner status in JSON format
addr : The address to mine to.

### /miner/stop

Stops the miner, returns the new miner status in JSON format

### /wallet/generate

Generates a new key pair, the result is in the JSON form {"publicKey" : publicKeyString, "privateKey" : privateKeyString}

### /wallet/createtransaction

Creates a new transaction.

params:
from : public address of the sender
privateKey : private key of the sender
to : public address of the destination
amount : amount of orencash to be sent
change : public address to send the change
fee : miners fee value

### /wallet/checkballance

Allows to check the balance of an address

params:
addr : public address

Result is in JSON format {"balance":value} or {"error":msg}

## explorer.html

Displays the current blockchain, allowing to navigate between different blocks and transactions.

## miner.html

Allows to check miners' status, to start and stop it. (for the moment all messages are displayed in the debugger console of your browser)

## transactions.html

Displays the list of the pending transactions, can also display detail of a completed transaction

# wallet.html

Allows you to manage your OrenCash wallet, generate adresses and order transactions
