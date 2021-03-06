#include "stdafx.h"
#include <cryptopp/randpool.h>
#include <cryptopp/rsa.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>

using namespace std;
#ifdef _DEBUG
#pragma comment(lib,"cryptlibd.lib")
#else
#pragma comment(lib,"cryptlib.lib")
#endif
#include "concise.h"

namespace concise{
	void GenerateRSAKey( unsigned int keyLength, const char *privFilename, const char *pubFilename, const char *seed  );
	string RSAEncryptString( const char *pubFilename, const char *seed, const char *message );
	string RSADecryptString( const char *privFilename, const char *ciphertext );
	RandomPool & GlobalRNG();
	main(){
		char priKey[ 128 ] = { 0 };
		char pubKey[ 128 ] = { 0 };
		char seed[ 1024 ]  = { 0 };
		// 生成 RSA 密钥对
		strcpy( priKey, "pri" );  // 生成的私钥文件
		strcpy( pubKey, "pub" );  // 生成的公钥文件
		strcpy( seed, "seed" );
		GenerateRSAKey( 1024, priKey, pubKey, seed );
		// RSA 加解密
		char message[ 1024 ] = { 0 };
		cout<< "Origin Text:\t" << "Hello World!" << endl << endl;
		strcpy( message, "Hello World!" );
		string encryptedText = RSAEncryptString( pubKey, seed, message );  // RSA 公匙加密
		cout<<"Encrypted Text:\t"<< encryptedText << endl << endl;
		string decryptedText = RSADecryptString( priKey, encryptedText.c_str() );  // RSA 私匙解密
	}
	void GenerateRSAKey(unsigned int keyLength, const char *privFilename, const char *pubFilename, const char *seed){
		RandomPool randPool;
		randPool.Put((byte *)seed, strlen(seed));
		RSAES_OAEP_SHA_Decryptor priv(randPool, keyLength);
		HexEncoder privFile(new FileSink(privFilename));
		priv.DEREncode(privFile);
		privFile.MessageEnd();
		RSAES_OAEP_SHA_Encryptor pub(priv);
		HexEncoder pubFile(new FileSink(pubFilename));
		pub.DEREncode(pubFile);
		pubFile.MessageEnd();
		return;
	}
	// RSA加密
	string RSAEncryptString( const char *pubFilename, const char *seed, const char *message ){
		FileSource pubFile( pubFilename, true, new HexDecoder );
		RSAES_OAEP_SHA_Encryptor pub( pubFile );
		RandomPool randPool;
		randPool.Put( (byte *)seed, strlen(seed) );
		string result;
		StringSource( message, true, new PK_EncryptorFilter(randPool, pub, new HexEncoder(new StringSink(result))) );
		return result;
	}
	// RSA解密
	string RSADecryptString( const char *privFilename, const char *ciphertext ){
		FileSource privFile( privFilename, true, new HexDecoder );
		RSAES_OAEP_SHA_Decryptor priv(privFile);
		string result;
		StringSource( ciphertext, true, new HexDecoder(new PK_DecryptorFilter(GlobalRNG(), priv, new StringSink(result))) );
		return result;
	}
	// 定义全局的随机数池
	RandomPool & GlobalRNG(){
		static RandomPool randomPool;
		return randomPool;
	}
}