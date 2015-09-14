#include "../stdafx.h"
#include <cryptopp/oids.h>
#include <cryptopp/osrng.h>
#include <cryptopp/eccrypto.h>
#include <cryptopp/asn.h>
#include <cryptopp/ec2n.h>
#include <cryptopp/ecp.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/base64.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/integer.h>
#include <cryptopp/randpool.h>
#include <cryptopp/filters.h>
#include <cryptopp/sha.h>

using namespace std;
//using namespace CryptoPP;
#ifdef _DEBUG
#pragma comment(lib,"cryptlibd.lib")
#else
#pragma comment(lib,"cryptlib.lib")
#endif
#include "../../concise.h"

namespace cs{
	/*template<typename T> bool _sha(const void* src,int len,void* digest){
		bool rt = true;
		try{
			T sha;
			//计算哈希值并且存储在二进制块中
			sha.CalculateDigest((uchar*)digest,(const uchar*)src,len);
		}catch(exception e){
			rt = false;
		}
		return rt;
	}
	bool Sha(const void* src,int len,void* digest){
		return _sha<CryptoPP::SHA>(src,len,digest);
	}
	bool Sha1(const void* src,int len,void* digest){
		return _sha<CryptoPP::SHA>(src,len,digest);
	}
	bool Sha160(const void* src,int len,void* digest){
		return _sha<CryptoPP::SHA>(src,len,digest);
	}
	bool Sha224(const void* src,int len,void* digest){
		return _sha<CryptoPP::SHA224>(src,len,digest);
	}
	bool Sha256(const void* src,int len,void* digest){
		return _sha<CryptoPP::SHA256>(src,len,digest);
	}
	bool Sha384(const void* src,int len,void* digest){
		return _sha<CryptoPP::SHA384>(src,len,digest);
	}
	bool Sha512(const void* src,int len,void* digest){
		return _sha<CryptoPP::SHA512>(src,len,digest);
	}*/
	int _getIntegerByte(CryptoPP::Integer p,uchar* buf){
		CryptoPP::ByteQueue bq;
		p.DEREncode(bq);
		CryptoPP::lword len = bq.MaxRetrievable();
		if(len>ECC_KEY_LEN){
			_ASSERT(0);
			return 0;
		}
		size_t sz = bq.Get(buf,(uint)len);
		for(uint i=ECC_KEY_LEN-1;i>=sz;i--){
			buf[i] = 0;
		}
		return (int)len;
	}
	class _Ecdsa : public _class{
	public:
		CryptoPP::ECDSA<CryptoPP::ECP,CryptoPP::SHA512>::PrivateKey _privkey;
		CryptoPP::ECDSA<CryptoPP::ECP,CryptoPP::SHA512>::PublicKey _pubkey;
	};
#define hd ((_Ecdsa*)_handle)
	Ecdsa::Ecdsa(){
		_handle = new _Ecdsa;
	}
	Ecdsa::~Ecdsa(){
		delete (_Ecdsa*)_handle;
	}
	bool Ecdsa::CreateKey(){
		_errinf = L"";
		bool rt = true;
		try{
			CryptoPP::AutoSeededRandomPool rng;
			hd->_privkey.Initialize(rng,CryptoPP::ASN1::secp521r1());
			hd->_privkey.MakePublicKey(hd->_pubkey);
		}catch(exception e){
			_errinf = e.what();
			rt = false;
		}
		return rt;
	}
	bool Ecdsa::GetPrivateKey(void* key){
		_errinf = L"";
		bool rt = true;
		try{
			CryptoPP::Integer p = hd->_privkey.GetPrivateExponent();
			_getIntegerByte(p,(uchar*)key);
		}catch(exception e){
			_errinf = e.what();
			rt = false;
		}
		return rt;
	}
	bool Ecdsa::GetPublicKey(void* kx,void* ky){
		_errinf = L"";
		bool rt = true;
		try{
			CryptoPP::Integer px = hd->_pubkey.GetPublicElement().x;
			_getIntegerByte(px,(uchar*)kx);
			CryptoPP::Integer py = hd->_pubkey.GetPublicElement().y;
			_getIntegerByte(py,(uchar*)ky);
		}catch(exception e){
			_errinf = e.what();
			rt = false;
		}
		return rt;
	}
	bool Ecdsa::SetPrivateKey(const void* key,int len){
		if(len!=ECC_KEY_LEN) return 0;
		_errinf = L"";
		bool rt = true;
		try{
			hd->_privkey.AccessGroupParameters().Initialize(CryptoPP::ASN1::secp521r1());
			CryptoPP::Integer p;
			p.BERDecode((const uchar*)key,len);
			hd->_privkey.SetPrivateExponent(p);
			CryptoPP::AutoSeededRandomPool rng;
			rt = hd->_privkey.Validate(rng,3);
		}catch(exception e){
			_errinf = e.what();
			rt = false;
		}
		return rt;
	}
	bool Ecdsa::MakePublicKey(){
		_errinf = L"";
		bool rt = true;
		try{
			hd->_privkey.MakePublicKey(hd->_pubkey);
		}catch(exception e){
			_errinf = e.what();
			rt = false;
		}
		return rt;
	}
	bool Ecdsa::SetPublicKey(const void* kx,const void* ky,int klen){
		if(klen!=ECC_KEY_LEN) return 0;
		_errinf = L"";
		bool rt = true;
		try{
			CryptoPP::Integer px;
			CryptoPP::Integer py;
			px.BERDecode((const uchar*)kx,klen);
			py.BERDecode((const uchar*)ky,klen);
			hd->_pubkey.AccessGroupParameters().Initialize(CryptoPP::ASN1::secp521r1());
			hd->_pubkey.SetPublicElement(CryptoPP::ECP::Point(px,py));
			CryptoPP::AutoSeededRandomPool rng;
			rt = hd->_pubkey.Validate(rng,3);
		}catch(exception e){
			_errinf = e.what();
			rt = false;
		}
		return rt;
	}
	bool Ecdsa::SetPublicKey(const void* pubkey,int keylen){
		if(keylen!=ECC_KEY_LEN*2) return 0;
		return SetPublicKey(pubkey,(char*)pubkey+ECC_KEY_LEN,ECC_KEY_LEN);
	}
	bool Ecdsa::GetPublicKey(void* pubkey){
		return GetPublicKey(pubkey,(char*)pubkey+ECC_KEY_LEN);
	}
	bool Ecdsa::GetSign(void* sign,const void* hash,int len){
		_errinf = L"";
		bool rt = true;
		try{
			CryptoPP::ECDSA<CryptoPP::ECP,CryptoPP::SHA512>::Signer signer(hd->_privkey);
			CryptoPP::AutoSeededRandomPool rng;
			//计算签名数据（通过二进制哈希数据），并且保持在 sbbCipherText 中。
			if(ECC_SIGN_LEN!=signer.SignatureLength()){
				_ASSERT(0);
				return false;
			}
			signer.SignMessage(rng,(const uchar*)hash,len,(uchar*)sign);
		}catch(exception e){
			_errinf = e.what();
			rt = false;
		}
		return rt;
	}
	bool Ecdsa::Verify(const void* hash,int hashlen,const void* sign,int signlen){
		if(signlen!=ECC_SIGN_LEN){
			_ASSERT(0);
			return 0;
		}
		_errinf = L"";
		bool rt = true;
		try{
			CryptoPP::ECDSA<CryptoPP::ECP,CryptoPP::SHA512>::Verifier verifier(hd->_pubkey);
			rt = verifier.VerifyMessage((const uchar*)hash,hashlen,(const uchar*)sign,signlen);
		}catch(exception e){
			_errinf = e.what();
			rt = false;
		}
		return rt;
	}
	class _Ecies : public _class{
	public:
		CryptoPP::ECIES<CryptoPP::ECP>::PrivateKey _privkey;
		CryptoPP::ECIES<CryptoPP::ECP>::PublicKey _pubkey;
	};
#define he ((_Ecies*)_handle)
	Ecies::Ecies(){
		_handle = new _Ecies;
	}
	Ecies::~Ecies(){
		delete (_Ecies*)_handle;
	}
	bool Ecies::CreateKey(){
		_errinf = L"";
		bool rt = true;
		try{
			CryptoPP::AutoSeededRandomPool rng;
			he->_privkey.Initialize(rng,CryptoPP::ASN1::secp521r1());
			he->_privkey.MakePublicKey(he->_pubkey);
			rt = he->_privkey.Validate(rng,3)&&he->_pubkey.Validate(rng,3);
		}catch(exception e){
			_errinf = e.what();
			rt = false;
		}
		return rt;
	}
	bool Ecies::GetPrivateKey(void* key){
		_errinf = L"";
		bool rt = true;
		try{
			CryptoPP::Integer p = he->_privkey.GetPrivateExponent();
			_getIntegerByte(p,(uchar*)key);
		}catch(exception e){
			_errinf = e.what();
			rt = false;
		}
		return rt;
	}
	bool Ecies::GetPublicKey(void* kx,void* ky){
		_errinf = L"";
		bool rt = true;
		try{
			CryptoPP::Integer px = he->_pubkey.GetPublicElement().x;
			_getIntegerByte(px,(uchar*)kx);
			CryptoPP::Integer py = he->_pubkey.GetPublicElement().y;
			_getIntegerByte(py,(uchar*)ky);
		}catch(exception e){
			_errinf = e.what();
			rt = false;
		}
		return rt;
	}
	bool Ecies::SetPrivateKey(const void* key,int len){
		if(len!=ECC_KEY_LEN) return 0;
		_errinf = L"";
		bool rt = true;
		try{
			he->_privkey.AccessGroupParameters().Initialize(CryptoPP::ASN1::secp521r1());
			CryptoPP::Integer p;
			p.BERDecode((const uchar*)key,len);
			he->_privkey.SetPrivateExponent(p);
			CryptoPP::AutoSeededRandomPool rng;
			rt = he->_privkey.Validate(rng,3);
		}catch(exception e){
			_errinf = e.what();
			rt = false;
		}
		return rt;
	}
	bool Ecies::MakePublicKey(){
		_errinf = L"";
		bool rt = true;
		try{
			he->_privkey.MakePublicKey(he->_pubkey);
		}catch(exception e){
			_errinf = e.what();
			rt = false;
		}
		return rt;
	}
	bool Ecies::SetPublicKey(const void* kx,const void* ky,int len){
		if(len!=ECC_KEY_LEN) return 0;
		_errinf = L"";
		bool rt = true;
		try{
			CryptoPP::Integer px;
			CryptoPP::Integer py;
			px.BERDecode((const uchar*)kx,len);
			py.BERDecode((const uchar*)ky,len);
			he->_pubkey.AccessGroupParameters().Initialize(CryptoPP::ASN1::secp521r1());
			he->_pubkey.SetPublicElement(CryptoPP::ECP::Point(px,py));
			CryptoPP::AutoSeededRandomPool rng;
			rt = he->_pubkey.Validate(rng,3);
		}catch(exception e){
			_errinf = e.what();
			rt = false;
		}
		return rt;
	}
	bool Ecies::SetPublicKey(const void* pubkey,int keylen){
		if(keylen!=ECC_KEY_LEN*2) return 0;
		return SetPublicKey(pubkey,(char*)pubkey+ECC_KEY_LEN,ECC_KEY_LEN);
	}
	bool Ecies::GetPublicKey(void* pubkey){
		return GetPublicKey(pubkey,(char*)pubkey+ECC_KEY_LEN);
	}
	uint Ecies::Encrypt(Memory<char>& out,const void* src,int len){
		_errinf = L"";
		uint rt = 0;
		try{
			CryptoPP::ECIES<CryptoPP::ECP>::Encryptor encryptor(he->_pubkey);
			CryptoPP::AutoSeededRandomPool rng;
			size_t outlen = encryptor.CiphertextLength(len);
			if(out.Length()<outlen) 
				out.SetLength(outlen);
			encryptor.Encrypt(rng,(const uchar*)src,len,(byte*)out.Handle());
			rt = outlen;
		}catch(exception e){
			_errinf = e.what();
			rt = 0;
		}
		return rt;
	}
	uint Ecies::Decrypt(Memory<char>& out,const void* src,int len){
		_errinf = L"";
		try{
			CryptoPP::ECIES<CryptoPP::ECP>::Decryptor decryptor(he->_privkey);
			CryptoPP::AutoSeededRandomPool rng;
			if((int)out.Length()<len) out.SetLength(len);
			CryptoPP::DecodingResult result(decryptor.Decrypt(rng,(const uchar*)src,len,(byte*)out.Handle()));
			out.SetLength(result.messageLength);
			return result.messageLength;
		}catch(exception e){
			_errinf = e.what();
		}
		return 0;
	}
}