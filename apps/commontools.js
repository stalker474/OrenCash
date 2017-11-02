var xmlhttp = new XMLHttpRequest();

var addressMap = new Map();

function timeConverter(UNIX_timestamp){
    var a = new Date(UNIX_timestamp * 1000);
    var months = ['Jan','Feb','Mar','Apr','May','Jun','Jul','Aug','Sep','Oct','Nov','Dec'];
    var year = a.getFullYear();
    var month = months[a.getMonth()];
    var date = a.getDate();
    var hour = a.getHours();
    var min = a.getMinutes();
    var sec = a.getSeconds();
    var time = date + ' ' + month + ' ' + year + ' ' + hour + ':' + min + ':' + sec ;
    return time;
  }

function CreateLabel(name,value)
{
    var li = document.createElement("li");
    li.classList.add(name);
    var nameLabel = document.createElement("span");
    nameLabel.classList.add("name");
    var valueLabel = document.createElement("span");
    valueLabel.classList.add("value");
    nameLabel.innerHTML = name;
    valueLabel.innerHTML = value;
    li.appendChild(nameLabel);
    li.appendChild(valueLabel);
    return li;
}

function CreateLabelLink(name,value,url)
{
    var li = document.createElement("li");
    li.classList.add(name);
    var nameLabel = document.createElement("span");
    nameLabel.classList.add("name");
    var valueLabel = document.createElement("span");
    valueLabel.classList.add("value");
    var valueLink = document.createElement("a");
    valueLink.href = url;
    nameLabel.innerHTML = name;
    valueLabel.innerHTML = value;
    valueLink.appendChild(valueLabel);
    li.appendChild(nameLabel);
    li.appendChild(valueLink);
    return li;
}

function CreateInputHTML(input,blockid)
{
    var inputli = document.createElement("li");
    var inputul = document.createElement("ul");
    inputul.classList.add('input');
    var link = "transactions.html?id="+input.transactionID;
    if(blockid)
    {
        link += "&blockid="+blockid;
    }
    //index
    inputul.appendChild(CreateLabel("index",input.index));
    //transactionid
    inputul.appendChild(CreateLabelLink("transactionid",input.transactionID,link));
    //address
    inputul.appendChild(CreateLabel("address",input.address));
    //amount
    inputul.appendChild(CreateLabel("amount",input.amount));
    //signature
    inputul.appendChild(CreateLabel("signature",input.signature));

    inputli.appendChild(inputul);
    return inputli;
}

function CreateOutputHTML(output,blockid)
{
    var outputli = document.createElement("li");
    var outputul = document.createElement("ul");
    outputul.classList.add('output');
    var link = "transactions.html?id="+output.transactionID;
    if(blockid)
    {
        link += "&blockid="+blockid;
    }

    //index
    outputul.appendChild(CreateLabel("index",output.index));
    //transactionid
    outputul.appendChild(CreateLabelLink("transactionid",output.transactionID,link));
    //address
    outputul.appendChild(CreateLabel("address",output.address));
    //amount
    outputul.appendChild(CreateLabel("amount",output.amount));

    outputli.appendChild(outputul);
    return outputli;
}

function CreateTransactionHTML(transaction,blockid)
{
    var transactionli = document.createElement("li");
    
    var transactionul = document.createElement("ul");
    transactionul.name = "transaction";
    transactionul.classList.add('transaction');

    //id
    transactionul.appendChild(CreateLabelLink("id",transaction.id,"transactions.html?id="+transaction.id+"&blockid="+blockid));
    //type
    transactionul.appendChild(CreateLabel("type",transaction.type));
    //hash
    transactionul.appendChild(CreateLabel("hash",transaction.hash));

    //inputs
    var inputsList = document.createElement("ul");
    var inputLabel = document.createElement("span");
    inputLabel.innerHTML = "Inputs";
    inputLabel.classList.add("section");
    inputsList.appendChild(inputLabel);

    inputsList.classList.add('inputs');
    for(var k = 0; k < transaction.inputs.length; k++)
    {
        inputsList.appendChild(CreateInputHTML(transaction.inputs[k],blockid));
    }
    var inputli = document.createElement("li");
    inputli.classList.add("info");
    inputli.appendChild(inputsList);
    transactionul.appendChild(inputli);      
    
    //outputs
    var outputsList = document.createElement("ul");
    var outputLabel = document.createElement("span");
    outputLabel.innerHTML = "Outputs";
    outputLabel.classList.add("section");
    outputsList.appendChild(outputLabel);
    outputsList.classList.add('outputs');
    //for
    for(var k = 0; k < transaction.outputs.length; k++)
    {
        outputsList.appendChild(CreateOutputHTML(transaction.outputs[k],blockid));
    }
    var outputli = document.createElement("li");
    outputli.classList.add("info");
    outputli.appendChild(outputsList);
    transactionul.appendChild(outputli);  

    transactionli.appendChild(transactionul);

    return transactionli;
}

function CreateBlockHTML(block)
{
    var li = document.createElement("li");
    var newul = document.createElement("ul");
    newul.classList.add('block');
    //index
    newul.appendChild(CreateLabelLink("Block_index",block.index,"explorer.html?id="+block.index));
    //hash
    newul.appendChild(CreateLabelLink("hash",block.hash,"explorer.html?hash="+block.hash));
    //previousHash
    newul.appendChild(CreateLabelLink("previousHash",block.previousHash,"explorer.html?hash="+block.previousHash));
    //nonce
    newul.appendChild(CreateLabel("nonce",block.nonce));
    //time
    newul.appendChild(CreateLabel("time",timeConverter(block.time)));

    //transactions
    var transactionsList = document.createElement("ul");
    transactionsList.classList.add('transactions');
    
    for(var j = 0; j < block.transactions.length; j++)
    {
        transactionsList.appendChild(CreateTransactionHTML(block.transactions[j],block.index));
    }

    var newli = document.createElement("li");
    newli.classList.add("starttransactions");
    var transactionLabel = document.createElement("span");
    transactionLabel.innerHTML = "Transactions";
    transactionLabel.classList.add("section");
    newul.appendChild(transactionLabel);

    newli.appendChild(transactionsList);
    newul.appendChild(newli);

    li.appendChild(newul);

    return li;
}

function GetBlocks(blockDiv,id,hash)
{
    URL = "http://localhost:1666/blocks"
    if(id)
        URL += "?id="+id;
    else if(hash)
        URL += "?hash="+hash;

    xmlhttp.onreadystatechange = function(event) {
        // XMLHttpRequest.DONE === 4
        if (this.readyState === XMLHttpRequest.DONE) {
            if (this.status === 200) {
                console.log(this.responseText);
                let jsonData = JSON.parse(this.responseText);
                var ul = document.createElement("ul");
                ul.classList.add('blocks');
                for (var i = 0; i < jsonData.blocks.length; i++) {
                    ul.appendChild(CreateBlockHTML(jsonData.blocks[i]));
                }
                blockDiv.appendChild(ul);
            } else {
                console.log("Status de la réponse: %d (%s)", this.status, this.statusText);
            }
        }
    };

    xmlhttp.open("GET", URL, true);
    
    //xmlhttp.setRequestHeader("Content-Type", "application/json");
    xmlhttp.send(null);
}

function GetTransactions(id,blockid,transactionDiv)
{
    URL = "http://localhost:1666/transactions";
    if(id)
        URL += "?id="+id;
    if(id && blockid)
        URL += "&blockid="+blockid;
    
        xmlhttp.onreadystatechange = function(event) {
            // XMLHttpRequest.DONE === 4
            if (this.readyState === XMLHttpRequest.DONE) {
                if (this.status === 200) {
                    console.log(this.responseText);
                    let jsonData = JSON.parse(this.responseText);
                    if(jsonData.transactions.length > 0)
                    {
                        var trans = jsonData.transactions[0];
                        transactionDiv.appendChild(CreateTransactionHTML(trans,blockid));
                    }
                } else {
                    console.log("Status de la réponse: %d (%s)", this.status, this.statusText);
                }
            }
        };
    
        xmlhttp.open("GET", URL, true);
        xmlhttp.send(null);
}

function GenerateAddress()
{
    URL = "http://localhost:1666/wallet/generate"

    xmlhttp.onreadystatechange = function(event) {
        // XMLHttpRequest.DONE === 4
        if (this.readyState === XMLHttpRequest.DONE) {
            if (this.status === 200) {
                console.log(this.responseText);
                let jsonData = JSON.parse(this.responseText);
                var ul = document.getElementById("addresses");
                var li = document.createElement("li");
                li.appendChild(document.createTextNode(jsonData.publicKey));

                //Create an input type dynamically.   
                var element = document.createElement("input");
                element.type = "button";
                element.value = "check balance";
                element.name = jsonData.publicKey;
                element.onclick = function() { 
                    CheckBallance(this.name);
                };

                li.appendChild(element);

                 //Create an input type dynamically.   
                 var element2 = document.createElement("input");
                 element2.type = "text";
                 element2.value = "0";
                 element2.id = "balance_"+jsonData.publicKey;
 
                 li.appendChild(element2);

                ul.appendChild(li);
                addressMap.set(jsonData.publicKey,jsonData.privateKey);
            } else {
                console.log("Status de la réponse: %d (%s)", this.status, this.statusText);
            }
        }
    };

    xmlhttp.open("GET", URL, true);
    
    //xmlhttp.setRequestHeader("Content-Type", "application/json");
    xmlhttp.send(null);
}

function CheckBallance(addr)
{
    URL = "http://localhost:1666/wallet/checkbalance?addr="+addr;

    xmlhttp.onreadystatechange = function(event) {
        // XMLHttpRequest.DONE === 4
        if (this.readyState === XMLHttpRequest.DONE) {
            if (this.status === 200) {
                var text = document.getElementById("balance_"+addr);
                let jsonData = JSON.parse(this.responseText);
                text.value = jsonData.balance;
            } else {
                console.log("Status de la réponse: %d (%s)", this.status, this.statusText);
            }
        }
    };

    xmlhttp.open("GET", URL, true);
    xmlhttp.send(null);
}


function CreateTransaction(ifrom,ito,iamount,ifee)
{
    var from = document.getElementById(ifrom).value;  
    var to = document.getElementById(ito).value;  
    var amount = document.getElementById(iamount).value;  
    var fee = document.getElementById(ifee).value;
    var secret = addressMap.get(from);
   
    URL = "http://localhost:1666/wallet/createtransaction?from="+from+"&to="+to+"&amount="+amount+"&change="+from+"&privateKey="+secret+"&fee="+fee;

    xmlhttp.onreadystatechange = function(event) {
        // XMLHttpRequest.DONE === 4
        if (this.readyState === XMLHttpRequest.DONE) {
            if (this.status === 200) {
                let jsonData = JSON.parse(this.responseText);
                var text = document.getElementById("result");
                text.value = jsonData.result;
            } else {
                console.log("Status de la réponse: %d (%s)", this.status, this.statusText);
            }
        }
    };

    xmlhttp.open("GET", URL, true);
    xmlhttp.send(null);
}

function GetMinerStatus()
{
    URL = "http://localhost:1666/miner/status"
    
        xmlhttp.onreadystatechange = function(event) {
            // XMLHttpRequest.DONE === 4
            if (this.readyState === XMLHttpRequest.DONE) {
                if (this.status === 200) {
                    console.log(this.responseText);
                } else {
                    console.log("Status de la réponse: %d (%s)", this.status, this.statusText);
                }
            }
        };
    
        xmlhttp.open("GET", URL, true);
        xmlhttp.send(null);
}

function StartMining(iAddress)
{
    var address = document.getElementById(iAddress).value;  
    URL = "http://localhost:1666/miner/start?addr="+address;
    
        xmlhttp.onreadystatechange = function(event) {
            // XMLHttpRequest.DONE === 4
            if (this.readyState === XMLHttpRequest.DONE) {
                if (this.status === 200) {
                    console.log(this.responseText);
                } else {
                    console.log("Status de la réponse: %d (%s)", this.status, this.statusText);
                }
            }
        };
    
        xmlhttp.open("GET", URL, true);
        xmlhttp.send(null);
}

function StopMining()
{
    URL = "http://localhost:1666/miner/stop"
    
        xmlhttp.onreadystatechange = function(event) {
            // XMLHttpRequest.DONE === 4
            if (this.readyState === XMLHttpRequest.DONE) {
                if (this.status === 200) {
                    console.log(this.responseText);
                } else {
                    console.log("Status de la réponse: %d (%s)", this.status, this.statusText);
                }
            }
        };
    
        xmlhttp.open("GET", URL, true);
        xmlhttp.send(null);
}