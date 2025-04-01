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
#include "ModelParameterOptimize.h"		//yll 2018.11.08  SCE-UA算法
#include "SequencePartitioningandSelectionPage.h"
#include "DlgSaveSpareParameters.h"
#include "HydroPredictModelParaChoosePage.h"
/////////////////////////////////////////////////////////////////////////////

//宗旨：一类模型使用同一组参数，不同类别的模型使用不同的参数。例如，新安江模型均使用一组参数，水面产流模型使用另一组参数。不同类别的模型参数个数和参数含义都不相同。

// CHydroPredictModelAutoParaPage dialog
class CHydroPredictModelAutoParaPage : public CDialog, public CVirtualOptimizeObject1	//水文模型参数自动率定界面
{
// Construction
// CDialog:a modal or modeless dialog 模式或无模式对话框
public:
	CHydroPredictModelAutoParaPage(CBasicModel * BasicModel,CWnd* pParent = NULL);		// standard constructor
	//CBasicModel * BasicModel 
	// Dialog Data BasicModel.h
	//{{AFX_DATA(CHydroPredictModelAutoParaPage)
	friend class CHydroPredictModelParaChoosePage;
	enum {			//枚举类型
		IDD = IDD_MODEL_HYDRO_AUTOPARADATA
	};
	CComboBox	m_HydroModelCombo;		//水文模型组合框 水文分区列表
	CComboBox	m_HydroDataCombo;		//率定断面下拉框
	CComboBox	m_HohaoCombo;			//洪号组合框	CComboBox MFC 编辑率定计算水文序列
	CString	m_OutInfo;					//afxstr.h	MFC 输出对象
	//}}AFX_DATA
	GUID m_CurClassGuid;				//当前模型的GUID，mtf201801515
	CUGCtrlEx m_Grid,m_GridReport;		//表格对象
	CGraphWnd	m_Graph;				//绘图对象
	CBasicModel * m_BasicModel;			//基础模型对象
	CDataOutManage *m_DataOutManage,m_LocalDataOutManage;		//数据输出管理对象
	CDataOutItem *m_HydroDataOut,*m_ModelDataOut,m_Rain0DataOut,m_RainDataOut;		//数据输出项对象
	/*
	*m_HydroDataOut:水文数据输出对象
	*m_ModelDataOut:模型数据输出对象
	m_Rain0DataOut:降雨0数据输出对象	?二者有何区别?
	m_RainDataOut:降雨数据输出对象
	*/
	CHydroDataManage * m_HydroDataManage;			//当前运行号 从水文序列列表下拉框中被选中的水文序列，即当前需要率定的水文序列
	CHydroDataEmendation * m_HydroDataEmendation;	//当前率定断面  水文数据校正/修改/增删
	CHydroPredictModel * m_HydroPredictModel,*m_ComputePredictModel;//水文分区列表里面被选中的水文模型，参数被改变  参与计算的水文预报模型，即水文分区列表里面的所有水文模型对象  m_ComputePredictModel 河道汇流模型 马法
	CHydroModelDriverForItem * m_HydroModelItem;	//水文模型驱动对象	被选中的水文模型中第一个水文模型对象,m_HydroPredictModel中可能包含两个水文模型，如新安江、马斯京根
	CHydroDataArray m_HydroDataArray;				//水文数据数组
	double * m_AO;			//存放水文序列
	UINT m_Number;			//水文序列的长度
	BOOL m_bIsRun;			//typedef int 是否在运行
	BOOL m_bIsReport;		//typedef int 是否返回报告
	BOOL m_bIsSetSceuaparam;//是否设置sceua算法参数
	int m_beSelectedParam;	//被选中的参数组
	CPredictModelArray *m_InputPredictModelArray;//流入到该模型中的所有预报模型要素集合
	vector<CVirtHydroModelDriver *> m_PredicModelTypeArray;//预报模型类型集合 yll
	CHydroManageArray  *m_HydroManageArray,m_LocalHydroManageArray;//水文管理数组 文档中的所有水文序列、参加计算的水文序列
//	typedef CArray<CObject*, CHydroDataEmendation*> CHydroDataEmendationArray;
//	typedef std::vector<CHydroDataEmendation*> CHydroDataEmendationArray;
	CHydroDataEmendationArray * m_HydroDataEmendationArray;//所有率定断面对象集合

	void UpdateMenuUI();	//UI user interface,用户界面	更新菜单用户界面

	//针对参数初步最佳值
	BOOL GetDataFromMinMaxData(int l,double minData,double maxData,double DData,double & aData,double &Data,double &bData);//获取参数上下限
	BOOL ModelRun(CHydroDataManage * HydroDataManage,double &m_minSX,double &maxMax,double &maxAdev,double &DTime, vector<int> * vecPurposeofHonghao = NULL);//运行水文模型
	void ModelRun(vector<vector<int>> & PurposeofHonghao, double &m_minSX, double &maxMax, double &maxAdev, double &DTime);//计算当前率定对象所有水文序列的结果，可以生成率定报表
	//计算当前率定对象在水文序列HydroDataManage的结果
	void ModelRunAll(double &m_minSX,double &maxMax,double &maxAdev,double &DTime);		//计算所有模型？计算所有水文序列？计算所有洪号？
	void ModelRun(double &m_minSX,double &maxMax,double &maxAdev,double &DTime);		//计算当前率定对象所有水文序列的结果，可以生成率定报表
	void GetStatDataInfo(double &m_minSX,double &maxMax,double &maxAdev,double &DTime);	//获取状态数据信息
	BOOL GetStatDataInfo(COleDateTime &st,COleDateTime &et,double & mRain0,double & Rain,double &SX,
						double &Rmin,double &Cmin,COleDateTime &RminTim,COleDateTime &CminTim,
						double &Rmax,double &Cmax,COleDateTime &Rtim,COleDateTime &Ctim,
						double &RSum,double &CSum,double &RAdvData,double &CAdvData);	//获取状态数据信息
	double GetResult(int LuDiInfo,double SX,double Cmax,double CAdvData);		//获取率定结果 确定性系数、最大洪峰相对误差、
	void ResetGridData(BOOL WR);				//重新设置表格数据
	void ResetGridData(vector<double> ParaData);//仅重置表格“参数值”列
	void ResetControlState();					//重设控制状态
	void OnUpdateEditBegData(CCmdUI* pCmdUI);	//在线更新编辑请求数据
	void OnUpdateAutoCompute(CCmdUI* pCmdUI);	//在线更新自动计算
	void OnUpdateStopCompute(CCmdUI* pCmdUI);	//在线更新停止计算
	void OnUpdateCompute(CCmdUI* pCmdUI);		//在线更新计算
	void OnUpdateIDOK(CCmdUI* pCmdUI);			//在线更新IDOK?

	LRESULT OnShowInfoToStateBar(WPARAM wParam, LPARAM lParam)/*将信息显示到状态条上*/;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHydroPredictModelAutoParaPage)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);	//预翻译信息
	sceparam sceparam1;
	bool m_IsCalibrated;
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	处理数据交换
	//}}AFX_VIRTUAL

	//yll SCE-UA算法 2018.11.08
	double CaculateTargetValue(double * ParaS);
	void GetBasicMassageofModel();//获取水文模型驱动种类数
	void CalculateLJ();//处理AK
	vector<double> m_LJ;// L/(J^0.5)河道汇流时间    K=A*L/(J^0.5)
	double m_A;//综合系数 A(n,R)
	CModelParameterOptimize2 m_sceua;	//声明算法对象，调用算法
	//double TargetValue(double &m_minSX, double &maxMax, double &maxAdev, double &DTime);
	ofstream OutPara;

	//用于填表格 包括A、AX
	CStringArray m_ParaInfo;//存储参数名称 字符串数组对象 CStringArray afxcoll.h MFC 凡使用Collections Classes（用以处理数据结构如数组、串行）之程序必须包含此文件。
	vector<double>m_ParaData;//参数初值 起始点 包括A、AX
	vector<double>m_minData;//参数下限
	vector<double>m_maxData;//参数上限
	vector<GUID>m_ModelDriverGuidPrePara;//模型序号  记录每个参数所属的模型类型
	CArray<int, int> m_LuDiInfo;//可变数组  率定信息  （是否参与率定|推荐的目标函数）
	vector<int> m_RiverClass;//记录模型中的河道级数

	vector<CString> m_UsesOfHydroScequence;//水文序列的用途
	vector<vector<int>> m_PurposeofHonghao;//水文序列的用途选择

	//自动率定使用
	CStringArray m_ParaName;//
	vector<double>m_bl;//待率定参数的下限
	vector<double>m_bu;//待率定参数的上限
	vector<double>m_StartPoint;//起始点 初始参数
	vector<int>m_ParaIndex;//待率定参数的下标
	vector<int>m_ModelDriverIndex;//模型序号  记录每个参数所属的模型类型

// Implementation
protected:

	// Generated message map functions		生成消息映射函数
	//{{AFX_MSG(CHydroPredictModelAutoParaPage)
	virtual BOOL OnInitDialog();			//在线初始化对话框
	afx_msg void OnPaint();					//afx_msg: intentional placeholder 消息标志，它向系统声明：有消息映射到函数实现体；绘图？
	//绘制水文自动参数率定幕布 即主界面		下面一系列函数完成该界面的绘图
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	//
	afx_msg void OnSelchangeHohao();		//水文序列改变，编辑当前水文序列中子洪号函数  水文序列列表按钮
	afx_msg void OnSelchangeHydroData();	//选择改变水文数据	率定断面按钮
	afx_msg void OnSelchangeHydroModel();	//选择改变水文模型  水文分区列表按钮
	afx_msg void OnButtonCalibrationCalculate();		//在线按钮附加数据  率定计算按钮
	afx_msg void OnDestroy();				//在线摧毁  框图？
	afx_msg void OnSize(UINT nType, int cx, int cy);		//设置框图大小？
	afx_msg void OnButtonChang();			//按钮改变	设置水文序列初始状态按钮
	afx_msg void OnButtonAutomaticCalibration();				//按钮添加	自动率定计算 按钮
	//afx_msg void OnButtonDelete();			//按钮删除	停止率定 按钮
	afx_msg void OnAutohydroparaEditchildHonghao();		//编辑当前水文序列中子洪号 菜单
	afx_msg void OnAutohydroparaEditComputehonghao();	//自动参数编辑计算洪号
	afx_msg void OnGraphResetminmaxtime();				//设置显示时间区间
	afx_msg void OnAutohydroparaOutHonghaojg();			//输出当前水文序列的计算成果 标记
	afx_msg void OnAutohydroparaShowHonghaojg();		//显示当前水文序列的计算成果 菜单
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);//按钮DblClk
	afx_msg void OnClose();								//参数率定页关闭按钮
	afx_msg void OnAutohydroparaEditHonghaoreport();	//输出子洪号报表 菜单
	afx_msg void OnAutohydroparaEditHydroparaReport();	//自动水文参数编辑参数报告
	afx_msg void OnAutoparaMax();		//自动参数最大
	afx_msg void OnAutoparaAdv();		//自动参数Adv
	afx_msg void OnAutoparaStat();		//自动参数统计？
	afx_msg void OnAutoparaStaq();		//自动参数staq
	afx_msg void OnAutoparaDefault();	//自动参数默认
	afx_msg void OnAutoparaMaxDat();	//自动参数最大值
	afx_msg void OnAutoparaAdvData();	//自动参数均值
	afx_msg void OnAutoparaSxData();	//自动参数 确定性系数
	afx_msg void OnAutohydroparaEditHydroparaItemreport();		//自动水文参数编辑水文参数项报告
	afx_msg void OnAutohydroparaQureBasinInitidata();			//自动水文参数 流域初始数据

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HYDROPREDICTMODELAUTOPARAPAGE_H__5EBCBCAF_25C6_4901_9933_DF5022348B79__INCLUDED_)
