#if !defined(AFX_HYDROPREDICTMODELAUTOPARAPAGE_H__5EBCBCAF_25C6_4901_9933_DF5022348B79__INCLUDED_)
#define AFX_HYDROPREDICTMODELAUTOPARAPAGE_H__5EBCBCAF_25C6_4901_9933_DF5022348B79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HydroPredictModelAutoParaPage.h : header file
//
#include "BasicModel.h"					//y D:\BDMDSS_NEWSYSTEM(X64)\BDMDSS\SourceCode  
#include "DataOutInfo\GraphWnd.h"		//y D:\BDMDSS_NEWSYSTEM(X64)\BDMDSS\SourceCode\DataOutInfo
#include "DataOutInfo\DataOutManage.h"	//y D:\BDMDSS_NEWSYSTEM(X64)\BDMDSS\SourceCode\DataOutInfo

#include "ModelParameterOptimizePage.h"
#include "ModelParameterOptimize.h"		//yll 2018.11.08  SCE-UA�㷨
#include "SequencePartitioningandSelectionPage.h"
#include "DlgSaveSpareParameters.h"
#include "HydroPredictModelParaChoosePage.h"
/////////////////////////////////////////////////////////////////////////////

//��ּ��һ��ģ��ʹ��ͬһ���������ͬ����ģ��ʹ�ò�ͬ�Ĳ��������磬�°���ģ�;�ʹ��һ�������ˮ�����ģ��ʹ����һ���������ͬ����ģ�Ͳ��������Ͳ������嶼����ͬ��

// CHydroPredictModelAutoParaPage dialog
class CHydroPredictModelAutoParaPage : public CDialog, public CVirtualOptimizeObject1	//ˮ��ģ�Ͳ����Զ��ʶ�����
{
// Construction
// CDialog:a modal or modeless dialog ģʽ����ģʽ�Ի���
public:
	CHydroPredictModelAutoParaPage(CBasicModel * BasicModel,CWnd* pParent = NULL);		// standard constructor
	//CBasicModel * BasicModel 
	// Dialog Data BasicModel.h
	//{{AFX_DATA(CHydroPredictModelAutoParaPage)
	friend class CHydroPredictModelParaChoosePage;
	enum {			//ö������
		IDD = IDD_MODEL_HYDRO_AUTOPARADATA
	};
	CComboBox	m_HydroModelCombo;		//ˮ��ģ����Ͽ� ˮ�ķ����б�
	CComboBox	m_HydroDataCombo;		//�ʶ�����������
	CComboBox	m_HohaoCombo;			//�����Ͽ�	CComboBox MFC �༭�ʶ�����ˮ������
	CString	m_OutInfo;					//afxstr.h	MFC �������
	//}}AFX_DATA
	GUID m_CurClassGuid;				//��ǰģ�͵�GUID��mtf201801515
	CUGCtrlEx m_Grid,m_GridReport;		//������
	CGraphWnd	m_Graph;				//��ͼ����
	CBasicModel * m_BasicModel;			//����ģ�Ͷ���
	CDataOutManage *m_DataOutManage,m_LocalDataOutManage;		//��������������
	CDataOutItem *m_HydroDataOut,*m_ModelDataOut,m_Rain0DataOut,m_RainDataOut;		//������������
	/*
	*m_HydroDataOut:ˮ�������������
	*m_ModelDataOut:ģ�������������
	m_Rain0DataOut:����0�����������	?�����к�����?
	m_RainDataOut:���������������
	*/
	CHydroDataManage * m_HydroDataManage;			//��ǰ���к� ��ˮ�������б��������б�ѡ�е�ˮ�����У�����ǰ��Ҫ�ʶ���ˮ������
	CHydroDataEmendation * m_HydroDataEmendation;	//��ǰ�ʶ�����  ˮ������У��/�޸�/��ɾ
	CHydroPredictModel * m_HydroPredictModel,*m_ComputePredictModel;//ˮ�ķ����б����汻ѡ�е�ˮ��ģ�ͣ��������ı�  ��������ˮ��Ԥ��ģ�ͣ���ˮ�ķ����б����������ˮ��ģ�Ͷ���  m_ComputePredictModel �ӵ�����ģ�� ��
	CHydroModelDriverForItem * m_HydroModelItem;	//ˮ��ģ����������	��ѡ�е�ˮ��ģ���е�һ��ˮ��ģ�Ͷ���,m_HydroPredictModel�п��ܰ�������ˮ��ģ�ͣ����°�������˹����
	CHydroDataArray m_HydroDataArray;				//ˮ����������
	double * m_AO;			//���ˮ������
	UINT m_Number;			//ˮ�����еĳ���
	BOOL m_bIsRun;			//typedef int �Ƿ�������
	BOOL m_bIsReport;		//typedef int �Ƿ񷵻ر���
	BOOL m_bIsSetSceuaparam;//�Ƿ�����sceua�㷨����
	int m_beSelectedParam;	//��ѡ�еĲ�����
	CPredictModelArray *m_InputPredictModelArray;//���뵽��ģ���е�����Ԥ��ģ��Ҫ�ؼ���
	vector<CVirtHydroModelDriver *> m_PredicModelTypeArray;//Ԥ��ģ�����ͼ��� yll
	CHydroManageArray  *m_HydroManageArray,m_LocalHydroManageArray;//ˮ�Ĺ������� �ĵ��е�����ˮ�����С��μӼ����ˮ������
//	typedef CArray<CObject*, CHydroDataEmendation*> CHydroDataEmendationArray;
//	typedef std::vector<CHydroDataEmendation*> CHydroDataEmendationArray;
	CHydroDataEmendationArray * m_HydroDataEmendationArray;//�����ʶ�������󼯺�

	void UpdateMenuUI();	//UI user interface,�û�����	���²˵��û�����

	//��Բ����������ֵ
	BOOL GetDataFromMinMaxData(int l,double minData,double maxData,double DData,double & aData,double &Data,double &bData);//��ȡ����������
	BOOL ModelRun(CHydroDataManage * HydroDataManage,double &m_minSX,double &maxMax,double &maxAdev,double &DTime, vector<int> * vecPurposeofHonghao = NULL);//����ˮ��ģ��
	void ModelRun(vector<vector<int>> & PurposeofHonghao, double &m_minSX, double &maxMax, double &maxAdev, double &DTime);//���㵱ǰ�ʶ���������ˮ�����еĽ�������������ʶ�����
	//���㵱ǰ�ʶ�������ˮ������HydroDataManage�Ľ��
	void ModelRunAll(double &m_minSX,double &maxMax,double &maxAdev,double &DTime);		//��������ģ�ͣ���������ˮ�����У��������к�ţ�
	void ModelRun(double &m_minSX,double &maxMax,double &maxAdev,double &DTime);		//���㵱ǰ�ʶ���������ˮ�����еĽ�������������ʶ�����
	void GetStatDataInfo(double &m_minSX,double &maxMax,double &maxAdev,double &DTime);	//��ȡ״̬������Ϣ
	BOOL GetStatDataInfo(COleDateTime &st,COleDateTime &et,double & mRain0,double & Rain,double &SX,
						double &Rmin,double &Cmin,COleDateTime &RminTim,COleDateTime &CminTim,
						double &Rmax,double &Cmax,COleDateTime &Rtim,COleDateTime &Ctim,
						double &RSum,double &CSum,double &RAdvData,double &CAdvData);	//��ȡ״̬������Ϣ
	double GetResult(int LuDiInfo,double SX,double Cmax,double CAdvData);		//��ȡ�ʶ���� ȷ����ϵ��������������
	void ResetGridData(BOOL WR);				//�������ñ������
	void ResetGridData(vector<double> ParaData);//�����ñ�񡰲���ֵ����
	void ResetControlState();					//�������״̬
	void OnUpdateEditBegData(CCmdUI* pCmdUI);	//���߸��±༭��������
	void OnUpdateAutoCompute(CCmdUI* pCmdUI);	//���߸����Զ�����
	void OnUpdateStopCompute(CCmdUI* pCmdUI);	//���߸���ֹͣ����
	void OnUpdateCompute(CCmdUI* pCmdUI);		//���߸��¼���
	void OnUpdateIDOK(CCmdUI* pCmdUI);			//���߸���IDOK?

	LRESULT OnShowInfoToStateBar(WPARAM wParam, LPARAM lParam)/*����Ϣ��ʾ��״̬����*/;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHydroPredictModelAutoParaPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);	//Ԥ������Ϣ
	sceparam sceparam1;
	bool m_IsCalibrated;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	�������ݽ���
	//}}AFX_VIRTUAL

	//yll SCE-UA�㷨 2018.11.08
	double CaculateTargetValue(double * ParaS);
	void GetBasicMassageofModel();//��ȡˮ��ģ������������
	void CalculateLJ();//����AK
	vector<double> m_LJ;// L/(J^0.5)�ӵ�����ʱ��    K=A*L/(J^0.5)
	double m_A;//�ۺ�ϵ�� A(n,R)
	CModelParameterOptimize2 m_sceua;	//�����㷨���󣬵����㷨
	//double TargetValue(double &m_minSX, double &maxMax, double &maxAdev, double &DTime);
	ofstream OutPara;

	//�������� ����A��AX
	CStringArray m_ParaInfo;//�洢�������� �ַ���������� CStringArray afxcoll.h MFC ��ʹ��Collections Classes�����Դ������ݽṹ�����顢���У�֮�������������ļ���
	vector<double>m_ParaData;//������ֵ ��ʼ�� ����A��AX
	vector<double>m_minData;//��������
	vector<double>m_maxData;//��������
	vector<GUID>m_ModelDriverGuidPrePara;//ģ�����  ��¼ÿ������������ģ������
	CArray<int, int> m_LuDiInfo;//�ɱ�����  �ʶ���Ϣ  ���Ƿ�����ʶ�|�Ƽ���Ŀ�꺯����
	vector<int> m_RiverClass;//��¼ģ���еĺӵ�����

	vector<CString> m_UsesOfHydroScequence;//ˮ�����е���;
	vector<vector<int>> m_PurposeofHonghao;//ˮ�����е���;ѡ��

	//�Զ��ʶ�ʹ��
	CStringArray m_ParaName;//
	vector<double>m_bl;//���ʶ�����������
	vector<double>m_bu;//���ʶ�����������
	vector<double>m_StartPoint;//��ʼ�� ��ʼ����
	vector<int>m_ParaIndex;//���ʶ��������±�
	vector<int>m_ModelDriverIndex;//ģ�����  ��¼ÿ������������ģ������

// Implementation
protected:

	// Generated message map functions		������Ϣӳ�亯��
	//{{AFX_MSG(CHydroPredictModelAutoParaPage)
	virtual BOOL OnInitDialog();			//���߳�ʼ���Ի���
	afx_msg void OnPaint();					//afx_msg: intentional placeholder ��Ϣ��־������ϵͳ����������Ϣӳ�䵽����ʵ���壻��ͼ��
	//����ˮ���Զ������ʶ�Ļ�� ��������		����һϵ�к�����ɸý���Ļ�ͼ
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	//
	afx_msg void OnSelchangeHohao();		//ˮ�����иı䣬�༭��ǰˮ���������Ӻ�ź���  ˮ�������б�ť
	afx_msg void OnSelchangeHydroData();	//ѡ��ı�ˮ������	�ʶ����水ť
	afx_msg void OnSelchangeHydroModel();	//ѡ��ı�ˮ��ģ��  ˮ�ķ����б�ť
	afx_msg void OnButtonCalibrationCalculate();		//���߰�ť��������  �ʶ����㰴ť
	afx_msg void OnDestroy();				//���ߴݻ�  ��ͼ��
	afx_msg void OnSize(UINT nType, int cx, int cy);		//���ÿ�ͼ��С��
	afx_msg void OnButtonChang();			//��ť�ı�	����ˮ�����г�ʼ״̬��ť
	afx_msg void OnButtonAutomaticCalibration();				//��ť���	�Զ��ʶ����� ��ť
	//afx_msg void OnButtonDelete();			//��ťɾ��	ֹͣ�ʶ� ��ť
	afx_msg void OnAutohydroparaEditchildHonghao();		//�༭��ǰˮ���������Ӻ�� �˵�
	afx_msg void OnAutohydroparaEditComputehonghao();	//�Զ������༭������
	afx_msg void OnGraphResetminmaxtime();				//������ʾʱ������
	afx_msg void OnAutohydroparaOutHonghaojg();			//�����ǰˮ�����еļ���ɹ� ���
	afx_msg void OnAutohydroparaShowHonghaojg();		//��ʾ��ǰˮ�����еļ���ɹ� �˵�
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);//��ťDblClk
	afx_msg void OnClose();								//�����ʶ�ҳ�رհ�ť
	afx_msg void OnAutohydroparaEditHonghaoreport();	//����Ӻ�ű��� �˵�
	afx_msg void OnAutohydroparaEditHydroparaReport();	//�Զ�ˮ�Ĳ����༭��������
	afx_msg void OnAutoparaMax();		//�Զ��������
	afx_msg void OnAutoparaAdv();		//�Զ�����Adv
	afx_msg void OnAutoparaStat();		//�Զ�����ͳ�ƣ�
	afx_msg void OnAutoparaStaq();		//�Զ�����staq
	afx_msg void OnAutoparaDefault();	//�Զ�����Ĭ��
	afx_msg void OnAutoparaMaxDat();	//�Զ��������ֵ
	afx_msg void OnAutoparaAdvData();	//�Զ�������ֵ
	afx_msg void OnAutoparaSxData();	//�Զ����� ȷ����ϵ��
	afx_msg void OnAutohydroparaEditHydroparaItemreport();		//�Զ�ˮ�Ĳ����༭ˮ�Ĳ������
	afx_msg void OnAutohydroparaQureBasinInitidata();			//�Զ�ˮ�Ĳ��� �����ʼ����

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HYDROPREDICTMODELAUTOPARAPAGE_H__5EBCBCAF_25C6_4901_9933_DF5022348B79__INCLUDED_)
