
#pragma once

namespace ju{
#define ECC_KEY_LEN		68
#define ECC_SIGN_LEN	132
	enum EccCurve{
		ecc_curve_192,
		ecc_curve_224,
		ecc_curve_256,
		ecc_curve_384,
		ecc_curve_521
	};
	//签名验证
	class JUBASE_API Ecdsa : public _class{
	protected:
		String _errinf;
		void* _handle;
	public:
		Ecdsa();
		~Ecdsa();
		LPCWSTR GetError(){return _errinf;}
		static inline uint KeyLen(){return ECC_KEY_LEN;}
		static inline uint SignLen(){return ECC_SIGN_LEN;}
		bool CreateKey(EccCurve curve = ecc_curve_256);
		bool GetPrivateKey(void* pvk);
		bool GetPublicKey(void* kx,void* ky);
		bool GetPublicKey(void* puk);
		bool SetPrivateKey(const void* key,int len = ECC_KEY_LEN);
		bool MakePublicKey();
		bool SetPublicKey(const void* kx,const void* ky,int klen = ECC_KEY_LEN);
		bool SetPublicKey(const void* puk,int keylen = ECC_KEY_LEN*2);
		bool GetSign(void* sign,const void* data,int len);
		bool Verify(const void* data,int len,const void* sign,int signlen = ECC_SIGN_LEN);
	};
	//加密
	class JUBASE_API Ecies : public _class{
	protected:
		String _errinf;
		void* _handle;
	public:
		Ecies();
		~Ecies();
		LPCWSTR GetError(){return _errinf;}
		static inline uint KeyLen(){return ECC_KEY_LEN;}
		bool CreateKey(EccCurve curve = ecc_curve_256);
		bool GetPrivateKey(void* key);
		bool GetPublicKey(void* kx,void* ky);
		bool GetPublicKey(void* puk);
		bool SetPrivateKey(const void* key,int len = ECC_KEY_LEN);
		bool MakePublicKey();
		bool SetPublicKey(const void* kx,const void* ky,int klen = ECC_KEY_LEN);
		bool SetPublicKey(const void* puk,int len = ECC_KEY_LEN*2);
		uint Encrypt(Memory<char>& out,const void* src,int len);
		uint Decrypt(Memory<char>& out,const void* src,int len);
	};
	namespace ecc {
		bool Sha160(const void* src, int len, void* digest20);
		bool Sha224(const void* src, int len, void* digest28);
		bool Sha256(const void* src, int len, void* digest32);
		bool Sha384(const void* src, int len, void* digest48);
		bool Sha512(const void* src, int len, void* digest64);
		inline bool Sha1(const void* src, int len, void* digest) {
			return Sha160(src, len, digest);
		}
	}
}