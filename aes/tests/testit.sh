#!/bin/sh

echo "AES128 Encrypt"
time ./aes128_encrypt
echo "AES256 Encrypt"
time ./aes256_encrypt
echo "AES128 Decrypt"
time ./aes128_decrypt
echo "AES256 Decrypt"
time ./aes256_decrypt

echo "OpenSSL AES128 Decrypt"
time openssl enc -d -aes-128-cbc -K 01010101010101010101010101010101 -iv fffefdfcfbfaf9f8f7f6f5f4f3f2f1f0 -out dec_openssl.aes128 -in enc.aes128
echo "OpenSSL AES256 Decrypt"
time openssl enc -d -aes-256-cbc -K 0101010101010101010101010101010101010101010101010101010101010101 -iv fffefdfcfbfaf9f8f7f6f5f4f3f2f1f0 -out dec_openssl.aes256 -in enc.aes256

md5 testfile.dat
md5 dec.aes128
md5 dec.aes256
md5 dec_openssl.aes128
md5 dec_openssl.aes256

rm enc.aes128
rm enc.aes256
rm dec.aes128
rm dec.aes256
rm dec_openssl.aes128
rm dec_openssl.aes256

echo "OpenSSL AES Encrypt"
time openssl enc -aes-128-cbc -K 01010101010101010101010101010101 -iv fffefdfcfbfaf9f8f7f6f5f4f3f2f1f0 -out enc.aes128 -in testfile.dat
echo "OpenSSL AES Encrypt"
time openssl enc -aes-256-cbc -K 0101010101010101010101010101010101010101010101010101010101010101 -iv fffefdfcfbfaf9f8f7f6f5f4f3f2f1f0 -out enc.aes256 -in testfile.dat
echo "AES128 Decrypt"
time ./aes128_decrypt
echo "AES256 Decrypt"
time ./aes256_decrypt

md5 testfile.dat
md5 dec.aes128
md5 dec.aes256

rm enc.aes128
rm enc.aes256
rm dec.aes128
rm dec.aes256

