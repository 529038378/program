#pragma once


// CCommonAlgoDlg �Ի���

class CCommonAlgoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCommonAlgoDlg)

public:
	CCommonAlgoDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CCommonAlgoDlg();
	BOOL OnInitDialog();
// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};
CCommonAlgoDlg& GetCommonAlgoDlg();
