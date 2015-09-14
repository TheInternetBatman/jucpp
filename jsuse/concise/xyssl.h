#pragma once
namespace cs{
	class CONCISE_API Md5 : public _class{
	protected:
		void* _md5_ctx;
	public:
		Md5();
		~Md5();
		void Reset();
		void Update(const void* data,uint len);
		uint Finish(void* digest16);
		static uint GetDigest(void* digest16,const void* data,uint len);
		static uint GetFileDigest(void* digest16,LPCWSTR fn);
	};
	class CONCISE_API Md5Hmac : public _class{
	protected:
		void* _md5_ctx;
	public:
		Md5Hmac();
		~Md5Hmac();
		void Create(const void* key,uint keylen);
		void Update(const void* data,uint len);
		uint Finish(void* digest16);
		static uint GetDigest(void* digest16,const void* data,uint len,const void* key,uint klen);
	};
	enum SHA_BITS{
		sha_160 = 20,
		sha_224 = 28,
		sha_256 = 32,
		sha_384 = 48,
		sha_512 = 64
	};
	//Reset 不是必须调用的，因为构造函数里已经调用了一次。
	class CONCISE_API Sha : public _class{
	protected:
		void*	_sha_ctx;
		uint	_bits;
	public:
		Sha(SHA_BITS bits = sha_160);
		~Sha();
		void Reset();
		void Update(const void* data,uint len);
		uint Finish(void* digest);
		static uint GetDigest(void* digest,const void* data,uint len,SHA_BITS bits = sha_160);
		static uint GetFileDigest(void* digest,LPCWSTR fn,SHA_BITS bits = sha_160);
	};
	class CONCISE_API ShaHmac : public _class{
	protected:
		void*	_sha_ctx;
		uint	_bits;
	public:
		ShaHmac(SHA_BITS bits = sha_160);
		~ShaHmac();
		void Create(const void* key,uint klen);
		void Update(const void* data,uint len);
		uint Finish(void* digest);
		static uint GetDigest(void* digest,const void* data,uint len,const void* key,uint klen,SHA_BITS bits = sha_160);
	};
	/*@class Aes
	@text 这个类用于Aes加解密*/
	//text 设置密码。Aes使用的密码有3种，128位，192位，256位，对应的Key长度分别为16字节、24字节、32字节，如果用户给定的长度不足指定的类型，会用0补足。
	enum AES_BITS{
		aes_128 = 16,
		aes_192 = 24,
		aes_256 = 32
	};
	class CONCISE_API Aes : public _class{
	protected:
		void* _aes_ctx;
		char _iv[16];
	public:
		Aes();
		~Aes();
		//设置向量，向量是一个 16 字节的数据，如果 len 不足16 字节，会用 0 补足，如果
		//多于16 字节，会被舍弃。
		void SetIV(const void* iv16 = 0,int len = 16);
		inline char* GetIV(){return _iv;}
		/*function bool SetEncKey(const void* userkey,int len,AES_BITS keytype)
		设置当前对象为加密状态，并且设置密码。
		userkey：密码，真正有效位依赖于 keytype，Aes支持 3 种长度的密码：128位、192位、256位，
		对应的密码有效字节是 16,24,32 三种情况，多余的数据被抛弃。如果 userkey 的长度
		不足对应类型密码的长度，会用 0 在末尾补足，如果大于指定的长度，多余的字节会被舍弃。
		当 keylen 小于等于 0 时，userkey 被看成字串，可用长度就是字串的长度。
		@text 函数没有返回值。*/
		void SetEncKey(const void* userkey,int keylen = 0,AES_BITS keytype = aes_128);
		void SetDecKey(const void* userkey,int keylen = 0,AES_BITS keytype = aes_128);
		/*@function Encrypt(const void* in16,void* out16)
		@text 加密数据，加密是以16字节为单位的，输入和输出缓存都不能小于16字节，
		输入和输出字节数是相同的。
		@param in16 -- const void*
		@text 输入数据，不能小于16字节
		@param out16 -- void*
		@text 输出数据，不能小于16字节
		@return void
		@text 没有返回值。*/
		void EncryptEcb(const void* in16,void* out16);
		/*@function void Decrypt(const void* in16,void* out16)
		@text 解密数据，解密是以16字节为单位的，输入和输出缓存都不能小于16字节，
		输入和输出字节数是相同的。
		@param in16 -- const void*
		@text 输入数据，不能小于16字节
		@param out16 -- void*
		@text 输出数据，不能小于16字节
		@return void
		@text 没有返回值。*/
		void DecryptEcb(const void* in16,void* out16);
		//iv16是一个16字节数据，加密解密时必须相同，这个数据和源数据进行位运算，每次都是变化的。
		//cbc 模式支持16字节一组的流式处理方式，解密是向量 iv 必须初始化为和加密时相同。
		//iv 如果设为 0 ，使用一个全为 0 的 16 字节向量。
		void EncryptCbc(void* output,const void* input,int len);
		//cbc 模式支持16字节一组的流式处理方式，解密是向量 iv 必须初始化为和加密时相同。
		//cbc 解密可以随便选2个相邻的密文块，解密之后，后一个块是正确的原文。
		void DecryptCbc(void* output,const void* input,int len);
		void EncryptCfb(void* output,const void* input,int len,void* iv16,int* ivoffset);
		void DecryptCfb(void* output,const void* input,int len,void* iv16,int* ivoffset);
		/*@function void Encrypt(const void* input,int len,void* output)
		@text 加密指定长度的数据，输入可以是任意长度，但是输出总是16字节整数倍。
		比如加密长度是17字节，输出就需要32字节的空间。
		@param input -- const void*
		@text 要加密的数据
		@param len -- int
		@text 输入数据大小
		@param output -- void*
		@text 输出数据缓存，需要的大小总是16的倍数。
		@return int
		@text 返回输出的长度，它是一个16的倍数。*/
		int EncryptEcb(void* output,int buflen,const void* input,int len);
		/*@function Decrypt(const void* input,int len,void* output)
		@text 解密数据，输入数据的实际长度必须是16的倍数，但是可以指定 len
		为解密的数据长度。因为Aes加密总是处理16字节，非16字节的整数倍的尾部数据是用
		随机数据补足的，解密的时候需要截掉这部分数据。但是，Aes的加密数据本身不记录
		数据长度，它总是一次16字节，必须由用户自己记录原始数据的实际长度。
		@text 比如17字节的原始数据，加密后得到32字节的加密数据，这32字节是不能截断的，
		否则将无法解密。只有用这32字节数据解密后的32字节数据，才能截掉后面的随机数据。
		此时，可以指定输入长度 len 为 17，但是实际的长度是32，解密后，output自动只保存
		17字节的有效数据，而且output的大小可以是17字节，不会访问越界。
		@param input -- const void*
		@text 要解密的数据，实际有效长度必须是16的整数倍。
		@param len -- int
		@text 输入数据大小，此值可以是原始数据的长度
		@param output -- void*
		@text 输出数据缓存，需要的大小为len，即输入指定的长度。
		@return int
		@text 返回输出的长度，即输入的 len 参数，如果失败，返回 0 .*/
		int DecryptEcb(void* output,int buflen,const void* input,int len);
		/*@function int Encrypt(const void* input,int len,Memory<uchar>& outbuf)
		@text 加密数据，输出保存在一个Memory对象里面。
		@param input -- cosnt void*
		@text 输入数据。
		@param len -- int
		@text 输入数据的长度。
		@param outbuf -- Memory<uchar>&
		@text 用于存储输出数据。
		@return int
		@text 返回输出数据的长度，失败返回 0 .*/
		//返回值是加密数据的长度，outbuf会自动设置大小，但是不一定是数据长度，可能大于它。
		int EncryptEcb(Memory<char>& outbuf,const void* input,int len);
		/*@function int Decrypt(const void* input,int len,Memory<uchar>& outbuf)
		@text 解密数据，输入数据的实际长度必须是16的倍数，但是可以指定 len
		为解密的数据长度。因为Aes加密总是处理16字节，非16字节的整数倍的尾部数据是用
		随机数据补足的，解密的时候需要截掉这部分数据。但是，Aes的加密数据本身不记录
		数据长度，它总是一次16字节，必须由用户自己记录原始数据的实际长度。
		@text 比如17字节的原始数据，加密后得到32字节的加密数据，这32字节是不能截断的，
		否则将无法解密。只有用这32字节数据解密后的32字节数据，才能截掉后面的随机数据。
		此时，可以指定输入长度 len 为 17，但是实际的长度是32，解密后，output自动只保存
		17字节的有效数据。
		@param input -- const void*
		@text 要解密的数据，实际有效长度必须是16的整数倍。
		@param len -- int
		@text 输入数据大小，此值可以是原始数据的长度
		@param output -- void*
		@text 输出数据缓存，需要的大小为len，即输入指定的长度。
		@return int
		@text 返回输出的长度，即输入的 len 参数，如果失败，返回 0 .*/
		int DecryptEcb(Memory<char>& outbuf,const void* input,int len);
		uint GetOutputLen(uint srclen){
			int res = srclen%16;
			if(res) return srclen - res + 16;
			else return srclen;
		}
	};
	class CONCISE_API Base64{
	public:
		static int Base64::Encode(Memory<char>& output,const void* src,int len);
		//源数据的长度 len 可以是 -1，长度是 src 的字符长度。
		static int Base64::Decode(Memory<char>& output,const char* src,int len);
		static int Base64::Encode(char *dst, int *dlen,const void* src, int slen);
		//源数据的长度 len 可以是 -1，长度是 src 的字符长度。
		static int Base64::Decode(void *dst, int *dlen,const char *src, int slen);
	};
	//void sha512(const void* key,int keylen,const void* data,int datalen,void* buf64);
}