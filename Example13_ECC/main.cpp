
#include "../juwnd.h"

void printHex(void* data) {
	ju::String hex;
	hex.HexBinary(data, 256);
	ju::Print(L"%s", hex.Handle());
}
void eccTest(ju::EccCurve curve) {
	ju::Ecies ies;
	ies.CreateKey(curve);
	char key[256];
	memset(key, 0, 256);
	ies.GetPrivateKey(key);
	printHex(key);
	memset(key, 0, 256);
	char pu1[256], pu2[256];
	memset(pu1, 0, 256);
	memset(pu2, 0, 256);
	ies.GetPublicKey(pu1, pu2);
	printHex(pu1);
	printHex(pu2);
	ju::Print(L"\n");

	ju::Memory<char> enc, dec;
	char* src = "3����ѮIntel";// ��˾��������153����Ԫ�չ�����ɫ�����˼�ʻ������˾Mobileye��֮ǰ��ҹ�˾����˹���綯�������Զ���ʻ��Ӧ��֮һ��";
	int srclen = strlen(src);
	int enclen = ies.Encrypt(enc, src, srclen);
	int declen = ies.Decrypt(dec, enc.Handle(), enclen);
	ju::Print(L"srclen: %d,declen: %d,enclen: %d", srclen, declen,enclen);
	dec.SetLength(declen + 1);
	dec[declen] = 0;
	ju::Print("dec: %s", dec.Handle());
}
WINMAIN{
	for(int i = 0; i < 5; i++) {
		eccTest(ju::ecc_curve_192);
	}
	for(int i = 0; i < 5; i++) {
		eccTest(ju::ecc_curve_224);
	}
	for(int i = 0; i < 5; i++) {
		eccTest(ju::ecc_curve_256);
	}
	for(int i = 0; i < 5; i++) {
		eccTest(ju::ecc_curve_384);
	}
	for(int i = 0; i < 5; i++) {
		eccTest(ju::ecc_curve_521);
	}
	return 0;
}
