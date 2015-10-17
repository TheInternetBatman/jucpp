
#pragma once

namespace ju{
#define ECC_KEY_LEN		68
#define ECC_SIGN_LEN	132
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
		bool CreateKey();
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
		bool CreateKey();
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
}