// QR_Encode.h : CQR_Encode ���饹���Ԥ���ӥ��󥿩`�ե��������x
// Date 2006/05/17	Ver. 1.22	Psytec Inc.

#if !defined(AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD37__INCLUDED_)
#define AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD37__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// ����

// �`��ӆ����٥�
#define QR_LEVEL_L	0
#define QR_LEVEL_M	1
#define QR_LEVEL_Q	2
#define QR_LEVEL_H	3

// �ǩ`����`��
#define QR_MODE_NUMERAL		0
#define QR_MODE_ALPHABET	1
#define QR_MODE_8BIT		2
#define QR_MODE_KANJI		3

// �Щ`�����(�ͷ�)����`��
#define QR_VRESION_S	0 // 1 �� 9
#define QR_VRESION_M	1 // 10 �� 26
#define QR_VRESION_L	2 // 27 �� 40

#define MAX_ALLCODEWORD	 3706 // �t���`�ɥ�`�������
#define MAX_DATACODEWORD 2956 // �ǩ`�����`�ɥ�`�����(�Щ`�����40-L)
#define MAX_CODEBLOCK	  153 // �֥�å��ǩ`�����`�ɥ�`�������(�ңӥ��`�ɥ�`�ɤ򺬤�)
#define MAX_MODULESIZE	  177 // һ�x�⥸��`�������

// �ӥåȥޥå��軭�r�ީ`����
#define QR_MARGIN	4


/////////////////////////////////////////////////////////////////////////////
typedef struct tagRS_BLOCKINFO {
	int ncRSBlock;		// �ңӥ֥�å���
	int ncAllCodeWord;	// �֥�å��ڥ��`�ɥ�`����
	int ncDataCodeWord;	// �ǩ`�����`�ɥ�`����(���`�ɥ�`���� - �ңӥ��`�ɥ�`����)

} RS_BLOCKINFO, *LPRS_BLOCKINFO;


/////////////////////////////////////////////////////////////////////////////
// QR���`�ɥЩ`�����(�ͷ�)�v�B���

typedef struct tagQR_VERSIONINFO {
	int nVersionNo;	   // �Щ`�����(�ͷ�)����(1��40)
	int ncAllCodeWord; // �t���`�ɥ�`����

	// �����������֤��`��ӆ����(0 = L, 1 = M, 2 = Q, 3 = H) 
	int ncDataCodeWord[4];	// �ǩ`�����`�ɥ�`����(�t���`�ɥ�`���� - �ңӥ��`�ɥ�`����)

	int ncAlignPoint;	// ���饤���ȥѥ��`��������
	int nAlignPoint[6];	// ���饤���ȥѥ��`����������

	RS_BLOCKINFO RS_BlockInfo1[4]; // �ңӥ֥�å����(1)
	RS_BLOCKINFO RS_BlockInfo2[4]; // �ңӥ֥�å����(2)

} QR_VERSIONINFO, *LPQR_VERSIONINFO;


/////////////////////////////////////////////////////////////////////////////
// CQR_Encode ���饹

class CQR_Encode {
	// ���B/����
public:
	CQR_Encode();
	~CQR_Encode();

public:
	int m_nLevel;		// �`��ӆ����٥�
	int m_nVersion;		// �Щ`�����(�ͷ�)
	BOOL m_bAutoExtent;	// �Щ`�����(�ͷ�)�ԄӒ���ָ���ե饰
	int m_nMaskingNo;	// �ޥ����󥰥ѥ��`�󷬺�

public:
	int m_nSymbleSize;
	BYTE m_byModuleData[MAX_MODULESIZE][MAX_MODULESIZE]; // [x][y]
	// bit5:�C�ܥ⥸��`�루�ޥ����󥰌����⣩�ե饰
	// bit4:�C�ܥ⥸��`���軭�ǩ`��
	// bit1:���󥳩`�ɥǩ`��
	// bit0:�ޥ����ᥨ�󥳩`���軭�ǩ`��
	// 20h�Ȥ�Փ��ͤˤ��C�ܥ⥸��`���ж���11h�Ȥ�Փ��ͤˤ���軭����K�Ĥˤ�BOOL������

private:
	int m_ncDataCodeWordBit; // �ǩ`�����`�ɥ�`�ɥӥå��L
	BYTE m_byDataCodeWord[MAX_DATACODEWORD]; // �����ǩ`�����󥳩`�ɥ��ꥢ

	int m_ncDataBlock;
	BYTE m_byBlockMode[MAX_DATACODEWORD];
	int m_nBlockLength[MAX_DATACODEWORD];

	int m_ncAllCodeWord; // �t���`�ɥ�`����(�ң��`��ӆ���ǩ`���򺬤�)
	BYTE m_byAllCodeWord[MAX_ALLCODEWORD]; // �t���`�ɥ�`��������ꥢ
	BYTE m_byRSWork[MAX_CODEBLOCK]; // �ңӥ��`�ɥ�`�������`��

	// �ǩ`�����󥳩`���v�B�ե��󥯥����
public:
	//nLevel:������0-3
	//nVersion:��ά��汾��1-40����ά��ֱ��ʹ�ʽ��(V-1)*4 + 21��
	//bAutoExtent:��Ϊ�Ͱ�Խ�ͱ��ܹ�д����ַ����٣��������Դ�����ַ������࣬�˲�����ΪTRUE�����Զ���߰汾����ΪFALSE���Զ���߰汾�����Ǻ�����ʧ�ܡ�
	//nMaskingNo:��Ϊ 1.
	//lpsSource:Ҫ����������ı���
	BOOL EncodeData(int nLevel, int nVersion, BOOL bAutoExtent, int nMaskingNo, LPCSTR lpsSource, int ncSource = 0);

private:
	int GetEncodeVersion(int nVersion, LPCSTR lpsSource, int ncLength);
	BOOL EncodeSourceData(LPCSTR lpsSource, int ncLength, int nVerGroup);

	int GetBitLength(BYTE nMode, int ncData, int nVerGroup);

	int SetBitStream(int nIndex, WORD wData, int ncData);

	BOOL IsNumeralData(unsigned char c);
	BOOL IsAlphabetData(unsigned char c);
	BOOL IsKanjiData(unsigned char c1, unsigned char c2);

	BYTE AlphabetToBinaly(unsigned char c);
	WORD KanjiToBinaly(WORD wc);

	void GetRSCodeWord(LPBYTE lpbyRSWork, int ncDataCodeWord, int ncRSCodeWord);

	// �⥸��`�������v�B�ե��󥯥����
private:
	void FormatModule();

	void SetFunctionModule();
	void SetFinderPattern(int x, int y);
	void SetAlignmentPattern(int x, int y);
	void SetVersionPattern();
	void SetCodeWordPattern();
	void SetMaskingPattern(int nPatternNo);
	void SetFormatInfoPattern(int nPatternNo);
	int CountPenalty();
};

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_QR_ENCODE_H__AC886DF7_C0AE_4C9F_AC7A_FCDA8CB1DD37__INCLUDED_)
