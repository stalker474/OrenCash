[![Build Status](https://travis-ci.org/stalker474/OrenCash.svg?branch=master)](https://travis-ci.org/stalker474/OrenCash)
# OrenCash
A simplistic crypto-currency implementation inspired by Naivecoin project
Original project : https://github.com/conradoqg/naivecoin

In order to fully understand the crypto-development environment I decided to build this little project, which is basically a simplistic blockchain based currency.
I believe this to be the easiest way to understand what's going on in the crypto world, without digging deep into the complexity of the current bitcoin and eth implementation.

This project is made of two parts:

# apps

This directory contains html tools used as a backend controller for your node and miners. Its using REST to communicate with OrenCash services. You can create and process new transactions as well as manage wallets.

# orencash

This is the crypto-currency c++ code. It contains a node, a miner, an http server for REST services and an RPC server to communicate with other nodes.
