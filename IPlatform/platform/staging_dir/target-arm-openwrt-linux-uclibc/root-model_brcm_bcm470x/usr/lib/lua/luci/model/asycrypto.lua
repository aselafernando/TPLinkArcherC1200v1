--[[
Copyright(c) 2008-2014 Shenzhen TP-LINK Technologies Co.Ltd.

File    :  asycrypto.lua
Details :  Asy crypto library. Depends on openssl.
Author  :  Zhang Zhongwei <zhangzhongwei@tp-link.net>
Version :  1.0.0
Date    :  19 May, 2014
]]--

module("luci.model.asycrypto", package.seeall)


local uci = require "luci.model.uci"


--- RSA in default.
Crypto = luci.util.class()


--- Generate RSA key pairs. Public key (n, e), Private key (n, d)
-- @class function
-- @name  luarsa.gen_keys
-- @function [parent=#global] gen_keys
-- @param N/A
-- @return Three values of key pairs: n(modulus), e(public exponent), d(private exponent)

--- Encrypt message by public key (n, e).
-- @class function
-- @name  luarsa.encrypt
-- @param message Message to be encrypted.
-- @param n       Modulus.
-- @param e       Public exponent.
-- @return        String after encrypted, in hex format.

--- Decrypt message by private key (n, d)
-- @class function
-- @name  luarsa.decrypt
-- @param message Message (in hex format) to be decrypted.
-- @param n       Modulus, part of private key.
-- @param e       Public exponent.
-- @param d       Private exponent.
-- @return        String after decrypted.


--- Read the public key.
-- @param N/A
-- @return#table Table of public key.
function rsa_read_pubkey()
    local uci_r = uci.cursor()
    local keys = {
        n = uci_r:get("accountmgnt", "keys", "n") or "",
        e = uci_r:get("accountmgnt", "keys", "e") or ""
    }
    return keys
end

--- Read the private key.
-- @param N/A
-- @return #table Table of private key.
function rsa_read_prikey()
    local uci_r = uci.cursor()
    local keys = {
        n = uci_r:get("accountmgnt", "keys", "n") or "",
        e = uci_r:get("accountmgnt", "keys", "e") or "",
        d = uci_r:get("accountmgnt", "keys", "d") or ""
    }
    return keys
end

--- Generate the rsa key and save it into the uci config file.
-- @param  N/A
-- @return #bool The result.
function rsa_gen_keys()
    local luarsa = require "luarsa"
    local uci_r = uci.cursor()
    local n, e, d = luarsa.gen_keys()
    uci_r:set("accountmgnt", "keys", "n", n)
    uci_r:set("accountmgnt", "keys", "e", e)
    uci_r:set("accountmgnt", "keys", "d", d)
    uci_r:commit("accountmgnt")
    return true
end

--- Encrypt message by using public key.
-- @param str       Message to be encrypted.
-- @param pubkey    Public key.
-- @return #string  String after encrypted(in hex format).
function rsa_encrypt(str, pubkey)
    local luarsa = require "luarsa"
    if not str then 
        return nil 
    end
    if not pubkey then
        pubkey = rsa_read_pubkey()
    end
    pubkey.n = pubkey.n or ""
    pubkey.e = pubkey.e or ""
    local hexcode = luarsa.encrypt(str, pubkey.n, pubkey.e)
    return hexcode
end

--- Decrypt message by using private key.
-- @param hexcode   Message(in hex format) to be decrypted.
-- @param prikey    Private key.
-- @return #string String after decrypted.
function rsa_decrypt(hexcode, prikey)
    local luarsa = require "luarsa"
    if not hexcode then 
        return nil 
    end
    if not prikey then
        prikey = rsa_read_prikey()
    end
    prikey.n = prikey.n or ""
    prikey.e = prikey.e or ""
    prikey.d = prikey.d or ""
    local str = luarsa.decrypt(hexcode, prikey.n, prikey.e, prikey.d)
    return str
end

--- Construct function
function Crypto:__init__(type)  
    if not type or string.upper(type) == "RSA" then
        self.encrypt     = rsa_encrypt
        self.decrypt     = rsa_decrypt
        self.gen_keys    = rsa_gen_keys
        self.read_pubkey = rsa_read_pubkey
        self.read_prikey = rsa_read_prikey
    else
        
    end
end