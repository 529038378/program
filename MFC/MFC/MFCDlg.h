
// MFCDlg.h : ͷ�ļ�
//


#pragma once

//#include "customLib.h"

struct Argument
{
	char*	pcFileName;
	int		iIters;
	int		iKDTreeDepth;
	int		iInitTemp;
	int		iSampleTimes;
	int		iDesSpeed;	
	bool	bRealTime;
};

// CMFCDlg �Ի���
class CMFCDlg : public CDialogEx
{
// ����
public:
	CMFCDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MFC_DIALOG };
	Argument sArgu;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton2();
};

CMFCDlg& GetMFCDlg();