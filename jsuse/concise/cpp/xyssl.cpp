#include "../stdafx.h"
#include "../../concise.h"
#include "../xyssl/sha2.h"
#include "../xyssl/aes.h"
#include "../xyssl/sha1.h"
#include "../xyssl/md5.h"
#include "../xyssl/base64.h"
#include "../xyssl/sha4.h"
namespace cs{
	template<typename T> inline T* tvcast(void* ctx){
		return (T*)ctx;
	}
	void Md5(const void* input,int len,void* output16){
		md5((uchar*)input,len,(uchar*)output16);
	}
	Md5::Md5(){
		_md5_ctx = new md5_context;
		Reset();
	}
	Md5::~Md5(){
		delete tvcast<md5_context>(_md5_ctx);
	}
	void Md5::Reset(){
		md5_starts(tvcast<md5_context>(_md5_ctx));
	}
	void Md5::Update(const void* data,uint len){
		md5_update(tvcast<md5_context>(_md5_ctx),(uchar*)data,len);
	}
	uint Md5::Finish(void* buf){
		md5_finish(tvcast<md5_context>(_md5_ctx),(uchar*)buf);
		return 32;
	}
	uint Md5::GetDigest(void* digest16,const void* data,uint len){
		md5((byte*)data,len,(byte*)digest16);
		return 16;
	}
	uint Md5::GetFileDigest(void* digest16,LPCWSTR fn){
		cs::String str = fn;
		char buf[MAX_PATH];
		int len = str.ToMultiByte(buf,MAX_PATH);
		buf[len] = 0;
		return md5_file(buf,(byte*)digest16)?0:16;
	}
	Md5Hmac::Md5Hmac(){
		_md5_ctx = new md5_context;
	}
	Md5Hmac::~Md5Hmac(){
		delete tvcast<md5_context>(_md5_ctx);
	}
	void Md5Hmac::Create(const void* key,uint keylen){
		md5_hmac_starts(tvcast<md5_context>(_md5_ctx),(uchar*)key,keylen);
	}
	void Md5Hmac::Update(const void* data,uint len){
		md5_hmac_update(tvcast<md5_context>(_md5_ctx),(uchar*)data,len);
	}
	uint Md5Hmac::Finish(void* buf){
		md5_hmac_finish(tvcast<md5_context>(_md5_ctx),(uchar*)buf);
		return 32;
	}
	uint Md5Hmac::GetDigest(void* digest16,const void* data,uint len,const void* key,uint klen){
		md5_hmac((byte*)key,klen,(byte*)data,len,(byte*)digest16);
		return 16;
	}
	Sha::Sha(SHA_BITS bits){
		_bits = bits;
		if(_bits==sha_160){
			_sha_ctx = new sha1_context;
		}else if(_bits==sha_224||_bits==sha_256){
			_sha_ctx = new sha2_context;
		}else if(_bits==sha_384||_bits==sha_512){
			_sha_ctx = new sha4_context;
		}else{
			_sha_ctx = 0;
			_ASSERT(0);
		} 
		Reset();
	}
	void Sha::Reset(){
		if(_bits==sha_160){
			sha1_starts(tvcast<sha1_context>(_sha_ctx));
		}else if(_bits==sha_224){
			sha2_starts(tvcast<sha2_context>(_sha_ctx),1);
		}else if(_bits==sha_256){
			sha2_starts(tvcast<sha2_context>(_sha_ctx),0);
		}else if(_bits==sha_384){
			sha4_starts(tvcast<sha4_context>(_sha_ctx),1);
		}else if(_bits==sha_512){
			sha4_starts(tvcast<sha4_context>(_sha_ctx),0);
		}else{
			_ASSERT(0);
		} 
	}
	uint Sha::Finish(void *digest){
		if(_bits==sha_160){
			sha1_finish(tvcast<sha1_context>(_sha_ctx),(byte*)digest);
		}else if(_bits==sha_224||_bits==sha_256){
			sha2_finish(tvcast<sha2_context>(_sha_ctx),(byte*)digest);
		}else if(_bits==sha_384||_bits==sha_512){
			sha4_finish(tvcast<sha4_context>(_sha_ctx),(byte*)digest);
		}else{
			_ASSERT(0);
			return 0;
		} 
		return _bits;
	}
	void Sha::Update(const void* data,uint len){
		if(_bits==sha_160){
			sha1_update(tvcast<sha1_context>(_sha_ctx),(byte*)data,len);
		}else if(_bits==sha_224||_bits==sha_256){
			sha2_update(tvcast<sha2_context>(_sha_ctx),(byte*)data,len);
		}else if(_bits==sha_384||_bits==sha_512){
			sha4_update(tvcast<sha4_context>(_sha_ctx),(byte*)data,len);
		}else{
			_ASSERT(0);
		} 
	}
	Sha::~Sha(){
		if(_bits==sha_160){
			delete tvcast<sha1_context>(_sha_ctx);
		}else if(_bits==sha_224||_bits==sha_256){
			delete tvcast<sha2_context>(_sha_ctx);
		}else if(_bits==sha_384||_bits==sha_512){
			delete tvcast<sha4_context>(_sha_ctx);
		}else{
			_ASSERT(0);
		} 
	}
	uint Sha::GetDigest(void* digest,const void* data,uint len,SHA_BITS bits){
		if(bits==sha_160){
			sha1((byte*)data,len,(byte*)digest);
		}else if(bits==sha_224){
			sha2((byte*)data,len,(byte*)digest,1);
		}else if(bits==sha_256){
			sha2((byte*)data,len,(byte*)digest,0);
		}else if(bits==sha_384){
			sha4((byte*)data,len,(byte*)digest,1);
		}else if(bits==sha_512){
			sha4((byte*)data,len,(byte*)digest,0);
		}else{
			_ASSERT(0);
			return 0;
		}
		return bits;
	}
	uint Sha::GetFileDigest(void* digest,LPCWSTR fn,SHA_BITS bits){
		cs::String str = fn;
		char buf[MAX_PATH];
		int len = str.ToMultiByte(buf,MAX_PATH);
		buf[len] = 0;
		int rst;
		if(bits==sha_160){
			rst = sha1_file(buf,(byte*)digest);
		}else if(bits==sha_224){
			rst = sha2_file(buf,(byte*)digest,1);
		}else if(bits==sha_256){
			rst = sha2_file(buf,(byte*)digest,0);
		}else if(bits==sha_384){
			rst = sha4_file(buf,(byte*)digest,1);
		}else if(bits==sha_512){
			rst = sha4_file(buf,(byte*)digest,0);
		}else{
			_ASSERT(0);
			return 0;
		} 
		return rst?0:bits;
	}
	ShaHmac::ShaHmac(SHA_BITS bits){
		_bits = bits;
		if(_bits==sha_160){
			_sha_ctx = new sha1_context;
		}else if(_bits==sha_224||_bits==sha_256){
			_sha_ctx = new sha2_context;
		}else if(_bits==sha_384||_bits==sha_512){
			_sha_ctx = new sha4_context;
		}else{
			_sha_ctx = 0;
			_ASSERT(0);
		} 
	}
	void ShaHmac::Create(const void* key,uint klen){
		if(_bits==sha_160){
			sha1_hmac_starts(tvcast<sha1_context>(_sha_ctx),(byte*)key,klen);
		}else if(_bits==sha_224){
			sha2_hmac_starts(tvcast<sha2_context>(_sha_ctx),(byte*)key,klen,1);
		}else if(_bits==sha_256){
			sha2_hmac_starts(tvcast<sha2_context>(_sha_ctx),(byte*)key,klen,0);
		}else if(_bits==sha_384){
			sha4_hmac_starts(tvcast<sha4_context>(_sha_ctx),(byte*)key,klen,1);
		}else if(_bits==sha_512){
			sha4_hmac_starts(tvcast<sha4_context>(_sha_ctx),(byte*)key,klen,0);
		}else{
			_ASSERT(0);
		} 
	}
	uint ShaHmac::Finish(void *digest){
		if(_bits==sha_160){
			sha1_hmac_finish(tvcast<sha1_context>(_sha_ctx),(byte*)digest);
		}else if(_bits==sha_224||_bits==sha_256){
			sha2_hmac_finish(tvcast<sha2_context>(_sha_ctx),(byte*)digest);
		}else if(_bits==sha_384||_bits==sha_512){
			sha4_hmac_finish(tvcast<sha4_context>(_sha_ctx),(byte*)digest);
		}else{
			_ASSERT(0);
			return 0;
		} 
		return _bits;
	}
	void ShaHmac::Update(const void* data,uint len){
		if(_bits==sha_160){
			sha1_hmac_update(tvcast<sha1_context>(_sha_ctx),(byte*)data,len);
		}else if(_bits==sha_224||_bits==sha_256){
			sha2_hmac_update(tvcast<sha2_context>(_sha_ctx),(byte*)data,len);
		}else if(_bits==sha_384||_bits==sha_512){
			sha4_hmac_update(tvcast<sha4_context>(_sha_ctx),(byte*)data,len);
		}else{
			_ASSERT(0);
		} 
	}
	ShaHmac::~ShaHmac(){
		if(_bits==sha_160){
			delete tvcast<sha1_context>(_sha_ctx);
		}else if(_bits==sha_224||_bits==sha_256){
			delete tvcast<sha2_context>(_sha_ctx);
		}else if(_bits==sha_384||_bits==sha_512){
			delete tvcast<sha4_context>(_sha_ctx);
		}else{
			_ASSERT(0);
		} 
	}
	uint ShaHmac::GetDigest(void* digest,const void* data,uint len,const void* key,uint klen,SHA_BITS bits){
		if(bits==sha_160){
			sha1_hmac((byte*)key,klen,(byte*)data,len,(byte*)digest);
		}else if(bits==sha_224){
			sha2_hmac((byte*)key,klen,(byte*)data,len,(byte*)digest,1);
		}else if(bits==sha_256){
			sha2_hmac((byte*)key,klen,(byte*)data,len,(byte*)digest,0);
		}else if(bits==sha_384){
			sha4_hmac((byte*)key,klen,(byte*)data,len,(byte*)digest,1);
		}else if(bits==sha_512){
			sha4_hmac((byte*)key,klen,(byte*)data,len,(byte*)digest,0);
		}else{
			_ASSERT(0);
			return 0;
		}
		return bits;
	}
#define aes_ctx (aes_context*)_aes_ctx
	Aes::Aes(){
		_aes_ctx = new aes_context;
		::ZeroMemory(_iv,sizeof(_iv));
	}
	Aes::~Aes(){
		delete aes_ctx;
	}
	void _setAesKey(aes_context* ctx,const void* userkey,int len,AES_BITS key_type,bool encrypt){
		int keysize,bytes;
		if(key_type==aes_128){
			keysize = 128;
		}else if(key_type==aes_192){
			keysize = 192;
		}else if(key_type==aes_256){
			keysize = 256;
		}else{
			return;
		}
		bytes = keysize/8;
		if(len<=0){
			len = strlen((const char*)userkey);
		}
		if(len>bytes) len = bytes;
		uchar key[32];
		memcpy(key,userkey,len);
		memset(key+len,0,bytes-len);

		if(encrypt)
			aes_setkey_enc(ctx,key,keysize);//16,24,32
		else
			aes_setkey_dec(ctx,key,keysize);//16,24,32
	}
	void Aes::SetEncKey(const void* userkey,int len,AES_BITS key_type){
		_setAesKey(aes_ctx,userkey,len,key_type,true);
	}
	void Aes::SetDecKey(const void* userkey,int len,AES_BITS key_type){
		_setAesKey(aes_ctx,userkey,len,key_type,false);
	}
	void Aes::SetIV(const void* iv16,int len){
		if(iv16==NULL) len = 0;
		if((uint)len>16) len = 16;
		memcpy(_iv,iv16,len);
		memset(_iv+len,0,16-len);
	}

	//加密数据，加密是以16字节为单位的，输入和输出缓存都不能小于16字节，输入和输出字节数是相同的。
	void Aes::EncryptEcb(const void* in16,void* out16){
		aes_crypt_ecb(aes_ctx,AES_ENCRYPT,(uchar*)in16,(uchar*)out16);
	}
	//解密密数据，解密是以16字节为单位的，输入和输出缓存都不能小于16字节，输入和输出字节数是相同的。
	void Aes::DecryptEcb(const void* in16,void* out16){
		aes_crypt_ecb(aes_ctx,AES_DECRYPT,(uchar*)in16,(uchar*)out16);
	}
	//加密指定长度的数据，因为加密以16字节为单位，输出总是16字节整数倍。比如如果加密长度是17字节，输出就需要32字节。
	int Aes::EncryptEcb(void* output,int buflen,const void* input,int len){
		if(len<=0||buflen<=0) return 0;
		int lastn = len & 0x0F;
		len -= lastn;
		int len0 = len;
		if(lastn) buflen -= 16;
		if(len0>buflen) return 0;

		uchar* in16 = (uchar*)input;
		uchar* out16 = (uchar*)output;
		while(len>0){
			aes_crypt_ecb(aes_ctx,AES_ENCRYPT,in16,out16);
			len -= 16;
			in16 += 16;
			out16 += 16;
		}
		if(lastn){
			uchar buf[16];
			memcpy(buf,in16,lastn);
			memset(buf+lastn,0,16-lastn);
			aes_crypt_ecb(aes_ctx,AES_ENCRYPT,buf,out16);
		}
		return len0 + 16;
	}
	//解密数据，已经加密的数据长度必须是16的整数倍，如果不是尾部的数据会被丢弃。因为Aes本身不能处理非16整数倍的数据，需要自己记录原始数据的长度，如果不是16的整数倍，需要自己截断尾部的字节，Aes本身不进行相关的任何操作。
	int Aes::DecryptEcb(void* output,int buflen,const void* input,int len){
		if(len<=0||buflen<=0) return 0;
		int len0 = len;
		int lastn = len&0xF;
		len -= lastn;
		if(lastn) buflen -= 16;
		if(len0>buflen) return 0;

		uchar* in16 = (uchar*)input;
		uchar* out16 = (uchar*)output;
		while(len>=16){
			aes_crypt_ecb(aes_ctx,AES_DECRYPT,in16,out16);
			len -= 16;
			in16 += 16;
			out16 += 16;
		}
		if(lastn){
			uchar buf[16];
			aes_crypt_ecb(aes_ctx,AES_DECRYPT,in16,buf);
			memcpy(out16,buf,lastn);
		}
		return len0;
	}
	//这个函数的特点是可以指定输入数据的长度，注意，input的有效数据长度必须是16的整数倍，input的大小必须大于等于len。
	//但是outbuf仅仅存储，有效的输出长度。
	int Aes::DecryptEcb(Memory<char>& outbuf,const void* input,int len){
		if(len<=0) return 0;
		if(outbuf.Length()<(uint)len){
			if(!outbuf.SetLength(len)) return 0;
		}
		return DecryptEcb(outbuf.Handle(),len,input,len);
	}
	//返回值是加密数据的长度，outbuf会自动设置大小，但是不一定是数据长度，可能大于它。
	int Aes::EncryptEcb(Memory<char>& outbuf,const void* input,int len){
		if(len<=0) return 0;
		int lastn = len&0xF;
		int outLen;
		if(lastn) outLen = len - lastn + 16;
		else outLen = len;
		if(outbuf.Length()<(uint)outLen){
			if(!outbuf.SetLength(outLen)) return 0;
		}
		return EncryptEcb(outbuf.Handle(),outLen,input,len);
	}
	void Aes::EncryptCbc(void *output,const void *input,int len){
		int lastn = len & 0x0F;
		len -= lastn;
		aes_crypt_cbc(aes_ctx,AES_ENCRYPT,len,(byte*)_iv,(byte*)input,(byte*)output);
		if(lastn){
			uchar buf[16];
			memcpy(buf,(byte*)input+len,lastn);
			memset(buf+lastn,0,16-lastn);
			aes_crypt_cbc(aes_ctx,AES_ENCRYPT,16,(byte*)_iv,buf,(byte*)output+len);
		}
	}
	void Aes::DecryptCbc(void* output,const void* input,int len){
		aes_crypt_cbc(aes_ctx,AES_DECRYPT,len,(byte*)_iv,(byte*)input,(byte*)output);
	}
	void Aes::EncryptCfb(void* output,const void* input,int len,void* iv16,int* ivoffset){
		aes_crypt_cfb(aes_ctx,AES_ENCRYPT,len,ivoffset,(byte*)iv16,(byte*)input,(byte*)output);
	}
	void Aes::DecryptCfb(void* output,const void* input,int len,void* iv16,int* ivoffset){
		aes_crypt_cfb(aes_ctx,AES_DECRYPT,len,ivoffset,(byte*)iv16,(byte*)input,(byte*)output);
	}
	int Base64::Encode(Memory<char>& output,const void* data,int len){
		int dlen = 0;
		base64_encode(0,&dlen,(uchar*)data,len);
		if(dlen==0) return 0;
		if(!output.SetLength(dlen)) return 0;
		base64_encode((byte*)output.Handle(),&dlen,(uchar*)data,len);
		return dlen;
	}
	int Base64::Decode(Memory<char>& output,const char* data,int len){
		if(len==-1) len = strlen(data);
		int dlen = 0;
		base64_decode(0,&dlen,(uchar*)data,len);
		if(dlen==0) return 0;
		if(!output.SetLength(dlen)) return 0;
		base64_decode((byte*)output.Handle(),&dlen,(uchar*)data,len);
		return dlen;
	}
	int Base64::Encode(char *dst, int *dlen,const void* src, int slen ){
		return base64_encode((unsigned char*)dst,dlen,(unsigned char*)src,slen);
	}
	int Base64::Decode(void *dst, int *dlen,const char *src, int slen ){
		if(slen==-1) slen = strlen(src);		
		return base64_decode((unsigned char*)dst,dlen,(unsigned char*)src,slen);
	}
	/*void sha512(const void* key,int keylen,const void* data,int datalen,void* buf64){
		sha4_hmac((uchar*)key,keylen,(uchar*)data,datalen,(uchar*)buf64,0);
	}
	*/
};
