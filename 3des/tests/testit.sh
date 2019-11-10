#!/bin/sh

echo "DES Encrypt"
time ./des_encrypt
echo "DES Decrypt"
time ./des_decrypt
echo "3DES Encrypt"
time ./tripledes_encrypt
echo "3DES Decrypt"
time ./tripledes_decrypt

echo "OpenSSL DES Decrypt"
time openssl enc -d -des -K 0102030405060708 -iv FFFEFDFCFBFAF9F8 -out dec_openssl.des -in enc.des
echo "OpenSSL 3DES Decrypt"
time openssl enc -d -des3 -K 0102030405060708090A0B0C0D0E0F101112131415161718 -iv FFFEFDFCFBFAF9F8 -out dec_openssl.3des -in enc.3des

md5 testfile.dat
md5 dec.des
md5 dec.3des
md5 dec_openssl.des
md5 dec_openssl.3des

rm enc.des
rm enc.3des
rm dec.des
rm dec.3des
rm dec_openssl.des
rm dec_openssl.3des

echo "OpenSSL DES Encrypt"
time openssl enc -des -K 0102030405060708 -iv FFFEFDFCFBFAF9F8 -out enc.des -in testfile.dat
echo "OpenSSL 3DES Encrypt"
time openssl enc -des3 -K 0102030405060708090A0B0C0D0E0F101112131415161718 -iv FFFEFDFCFBFAF9F8 -out enc.3des -in testfile.dat
echo "DES Decrypt"
time ./des_decrypt
echo "3DES Decrypt"
time ./tripledes_decrypt

md5 testfile.dat
md5 dec.des
md5 dec.3des

rm enc.des
rm enc.3des
rm dec.des
rm dec.3des

