#pragma once


// CSimulatedAnnealDlg �Ի���

class CSimulatedAnnealDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSimulatedAnnealDlg)

public:
	CSimulatedAnnealDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSimulatedAnnealDlg();
	BOOL OnInitDialog();
// �Ի�������
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};

CSimulatedAnnealDlg& GetSimulatedAnnealAlgoDlg();
