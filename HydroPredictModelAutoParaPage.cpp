// HydroPredictModelAutoParaPage.cpp : implementation file

#include "stdafx.h"
#include "resource.h"
#include "HydroPredictModelAutoParaPage.h"
#include "EditDataTimeInterzone.h"
#include "EditLocalHongHaoPage.h"
#include "OutHongHaoChildReport.h"

#include "ModelParameterOptimize.h"		//yll 2018.11.08  SCE-UA算法
#include <vector>
#include <iostream>
#include <algorithm>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHydroPredictModelAutoParaPage dialog

int NUM = 100;		//最大迭代次数	？
int mID = 2;		//0-- 最大值 1---平均值  2---均方值  3---均方值，但以实测洪峰为权重		？目标值?
int m_DatType = 0;	//=0 综合系数  =1 为洪峰 =2 为均值 =3 为确定性系数 目标值
int maxNumber = 200;//区间分段数

//构造函数
CHydroPredictModelAutoParaPage::CHydroPredictModelAutoParaPage(CBasicModel * BasicModel, CWnd* pParent /*=NULL*/)
	: CDialog(CHydroPredictModelAutoParaPage::IDD, pParent)		//参数率定页面的构造函数
{
	/*
	CBasicModel * BasicModel：基本模型
	CWnd* pParent：微软基础类库
	CDialog：afxwin.h 微软基础类库
	*/
	//{{AFX_DATA_INIT(CHydroPredictModelAutoParaPage)
	m_OutInfo = _T("");
	//}}AFX_DATA_INIT
	m_BasicModel = BasicModel;
	m_HydroManageArray = &m_BasicModel->m_Parent->m_HydroManageArray;//水文序列管理数组 包含了文档中的所有水文序列
	m_HydroDataEmendationArray = &m_BasicModel->m_LocalHydroDataEmendationArray;//获取模型的率定断面数据
	m_AO = NULL;
	m_Number = 0;
	m_HydroDataManage = NULL;//当前运行号 
	m_HydroDataEmendation = NULL;//当前率定对象
	m_HydroPredictModel = NULL;//当前选择水文模型
	m_ComputePredictModel = NULL;//计算预报模型
	m_DataOutManage = NULL;//数据输出管理
	m_HydroDataOut = m_ModelDataOut = NULL;//水文数据输出
	m_HydroModelItem = NULL;//水文模型驱动对象
	m_bIsRun = FALSE;//是否在运行
	m_LocalHydroManageArray.SetSize(0);//本地水文管理数组
	m_bIsReport = FALSE;//是否返回报告上

	m_IsCalibrated = false;
}
//完成数据交换
void CHydroPredictModelAutoParaPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);		//默认什么都不做
	//{{AFX_DATA_MAP(CHydroPredictModelAutoParaPage)
	DDX_Control(pDX, IDC_COMBO_RIVER, m_HydroModelCombo);		//void AFXAPI DDX_Control(CDataExchange* pDX, int nIDC, CWnd& rControl);for getting access to the actual controls
	DDX_Control(pDX, IDC_COMBO_ADDRES_A, m_HydroDataCombo);		//
	DDX_Control(pDX, IDC_COMBO_ADDRES_B, m_HohaoCombo);			//
	DDX_Text(pDX, IDC_EDIT_ALF, m_OutInfo);						//
	//}}AFX_DATA_MAP
}
//消息映射机制
BEGIN_MESSAGE_MAP(CHydroPredictModelAutoParaPage, CDialog)		//开始消息映射机制
	ON_MESSAGE(WM_USER_SHOWINFOTOSTATEBAR, OnShowInfoToStateBar)//将信息显示到状态条上
//{{AFX_MSG_MAP(CHydroPredictModelAutoParaPage)
ON_WM_PAINT()
ON_WM_CREATE()
ON_CBN_SELCHANGE(IDC_COMBO_ADDRES_B, OnSelchangeHohao)		//水文序列列表按钮
ON_CBN_SELCHANGE(IDC_COMBO_ADDRES_A, OnSelchangeHydroData)	//率定断面按钮
ON_CBN_SELCHANGE(IDC_COMBO_RIVER, OnSelchangeHydroModel)	//水文分区列表按钮
ON_BN_CLICKED(IDC_BUTTON_APPENDDATA, OnButtonCalibrationCalculate)	//率定计算按钮
ON_WM_DESTROY()
ON_WM_SIZE()
ON_BN_CLICKED(IDC_BUTTON_CHANG, OnButtonChang)		//设置水文序列初始状态按钮
ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAutomaticCalibration)			//自动率定计算按钮
ON_COMMAND(IDM_AUTOHYDROPARA_EDITCHILD_HONGHAO, OnAutohydroparaEditchildHonghao)	//编辑当前水文序列中子洪号 菜单
ON_COMMAND(IDM_AUTOHYDROPARA_EDIT_COMPUTEHONGHAO, OnAutohydroparaEditComputehonghao)
ON_COMMAND(IDM_GRAPH_RESETMINMAXTIME, OnGraphResetminmaxtime)			//设置显示时间区间
ON_COMMAND(IDM_AUTOHYDROPARA_OUT_HONGHAOJG, OnAutohydroparaOutHonghaojg)
ON_COMMAND(IDM_AUTOHYDROPARA_SHOW_HONGHAOJG, OnAutohydroparaShowHonghaojg)
ON_WM_LBUTTONDBLCLK()
ON_WM_CLOSE()
ON_COMMAND(IDM_AUTOHYDROPARA_EDIT_HONGHAOREPORT, OnAutohydroparaEditHonghaoreport)
ON_COMMAND(IDM_AUTOHYDROPARA_EDIT_HYDROPARA_REPORT, OnAutohydroparaEditHydroparaReport)
ON_COMMAND(IDM_AUTOPARA_MAX, OnAutoparaMax)
ON_COMMAND(IDM_AUTOPARA_ADV, OnAutoparaAdv)
ON_COMMAND(IDM_AUTOPARA_STAT, OnAutoparaStat)
ON_COMMAND(IDM_AUTOPARA_STAQ, OnAutoparaStaq)
ON_COMMAND(IDM_AUTOPARA_DEFAULT, OnAutoparaDefault)
ON_COMMAND(IDM_AUTOPARA_MAX_DAT, OnAutoparaMaxDat)
ON_COMMAND(IDM_AUTOPARA_ADV_DATA, OnAutoparaAdvData)
ON_COMMAND(IDM_AUTOPARA_SX_DATA, OnAutoparaSxData)
ON_COMMAND(IDM_AUTOHYDROPARA_EDIT_HYDROPARA_ITEMREPORT, OnAutohydroparaEditHydroparaItemreport)
ON_COMMAND(IDM_AUTOHYDROPARA_QURE_BASIN_INITIDATA, OnAutohydroparaQureBasinInitidata)

//}}AFX_MSG_MAP
ON_UPDATE_COMMAND_UI(IDC_BUTTON_CHANG, OnUpdateEditBegData)
ON_UPDATE_COMMAND_UI(IDC_BUTTON_ADD, OnUpdateAutoCompute)
ON_UPDATE_COMMAND_UI(IDC_BUTTON_DELETE, OnUpdateStopCompute)
ON_UPDATE_COMMAND_UI(IDC_BUTTON_APPENDDATA, OnUpdateCompute)
ON_UPDATE_COMMAND_UI(IDOK, OnUpdateIDOK)
//ON_COMMAND(ID_SHOWRESULT, &CHydroPredictModelAutoParaPage::OnShowResult)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CHydroPredictModelAutoParaPage message handlers	消息处理程序
//将信息显示到状态条上
LRESULT CHydroPredictModelAutoParaPage::OnShowInfoToStateBar(WPARAM wParam, LPARAM lParam)
{
	/*
	将鼠标处的信息显示到状态条上
	WPARAM wParam：
	LPARAM lParam：鼠标处的时间和流量
	*/
	int id = int(wParam);			//？
	CString s = *(CString *)lParam;
	s = "水文自动参数率定界面____" + s;
	SetWindowText(s);			//winuser.h宏 将数据显示到界面
	return 1;
}
//更新
void CHydroPredictModelAutoParaPage::OnUpdateIDOK(CCmdUI* pCmdUI)
{
	UpdateData();					//MFC
	pCmdUI->Enable(!m_bIsRun);		//
}
//更新编辑Beg数据
void CHydroPredictModelAutoParaPage::OnUpdateEditBegData(CCmdUI* pCmdUI)
{
	UpdateData();
	int is = m_HydroModelCombo.GetCurSel();
	pCmdUI->Enable(is > -1 && !m_bIsRun);
}
//更新自动计算
void CHydroPredictModelAutoParaPage::OnUpdateAutoCompute(CCmdUI* pCmdUI)
{
	UpdateData();
	pCmdUI->Enable(m_ComputePredictModel != NULL&&m_HydroDataEmendation != NULL && !m_bIsRun);
}
//更新停止计算
void CHydroPredictModelAutoParaPage::OnUpdateStopCompute(CCmdUI* pCmdUI)
{
	UpdateData();
	pCmdUI->Enable(m_bIsRun);
}
//更新计算
void CHydroPredictModelAutoParaPage::OnUpdateCompute(CCmdUI* pCmdUI)
{
	UpdateData();
	pCmdUI->Enable(m_ComputePredictModel != NULL&&m_HydroDataEmendation != NULL && !m_bIsRun);
}
//初始化对话框
BOOL CHydroPredictModelAutoParaPage::OnInitDialog()
{
	CDialog::OnInitDialog();		//初始化对话框
	ShowWindow(SW_MAXIMIZE);		//最大化窗口
	// TODO: Add extra initialization here
	m_LocalDataOutManage.ResetGraphWnd(&m_Graph);		//对一幅图形的输入输出进行管理------对应着一个CGraphWnd, 排除相同的DataOutItem
	m_LocalDataOutManage.ResetBasicModel(NULL);			//m_BasicModel
	m_LocalDataOutManage.AssembleGraphWnd(&m_Graph);	//组装绘图窗口
	m_Grid.SetModifiedFlag(FALSE);						//设置修改标志
	m_GridReport.SetModifiedFlag(FALSE);				//设置修改标志

	m_HydroDataEmendation = NULL;		//水文数据校正
	m_HydroDataManage = NULL;			//水文数据管理
	m_HohaoCombo.ResetContent();		//洪号组合框，重新设置内容
	int i, j, n, DatTypeIndex;			//数据类型索引
	n = m_HydroManageArray->GetSize();	//获取洪号管理数组的维数
	for (i = 0; i < n; i++)
	{
		m_HohaoCombo.AddString(m_HydroManageArray->GetAt(i)->m_Text);	//向洪号组合框添加字符串
		m_LocalHydroManageArray.Add(m_HydroManageArray->GetAt(i));		//向本地水文管理数组添加水文管理数组中的第i个元素 将m_HydroManageArray里面的所有元素添加到m_LocalHydroManageArray，这样做有什么意义呢？为什么不直接使用m_HydroManageArray呢？
	}
	m_HydroDataCombo.ResetContent();					//重设率定断面下拉框的内容
	n = m_HydroDataEmendationArray->GetSize();			//获取率定断面的个数
	CHydroDataEmendation * HydroDataEmendation = NULL;	//定义水文数据校准数组对象
	BOOL bl;
	for (i = 0; i < n; i++)		//n 率定断面个数
	{
		HydroDataEmendation = m_HydroDataEmendationArray->GetAt(i);			//将水文数据校准数组的第i个对象赋给局部水文数据校准对象
		CModelElement * ModelElement = HydroDataEmendation->m_ModelElement;	//定义局部模型元素对象，将水文数据校准对象的模型元素赋给局部，模型元素对象
		bl = (ModelElement->GetCode() == ePredictHydro);					//获取当前模型类型为水文预报模型 判断是否为水文模型
		HydroDataEmendation->GetEmendationFlowDataTypeInfo(DatTypeIndex);	//获取校正流量数据类型信息	判断是否有流量数据
		if (bl&&DatTypeIndex != -1)	//如果是水文模且有流量数据
		{
			j = m_HydroDataCombo.AddString(HydroDataEmendation->m_Name);	//int CComboBox::AddString(LPCTSTR lpszString)
			m_HydroDataCombo.SetItemData(j, i);		//水文数据组合 设置项目数据
		}
		HydroDataEmendation->ResetChildHangHaoInfoArray(m_HydroManageArray);	//重置子洪号信息数组
	}
	UpdateMenuUI();	//更新菜单UI
	return TRUE;
}
//预翻译消息	什么意思？？  处理键盘、鼠标消息
BOOL CHydroPredictModelAutoParaPage::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	//MSG* pMsg：Winuser.h	结构体  winuser.h -- USER procedure declarations, constant definitions and macros  用户程序声明，常量定义和宏
	UpdateDialogControls(this, TRUE);	//更新对话框控制
	return CDialog::PreTranslateMessage(pMsg);
}
//绘制 水文模型参数自动率定界面 
void CHydroPredictModelAutoParaPage::OnPaint()		//该函数专用绘图
{
	CPaintDC dc(this); // device context for painting	设备环境类

	// TODO: Add your message handler code here 将消息处理程序添加到此处
	if (IsIconic())		//_AFXWIN_INLINE BOOL CWnd::IsIconic() const{ASSERT(::IsWindow(m_hWnd)); return ::IsIconic(m_hWnd);}
	{					//IsIconic() Nonzero if CWnd is minimized; otherwise 0. 如果CWnd最小化返回非零，否则返回0
		HICON hicon = AfxGetApp()->LoadIcon(IDI_RESETZD);
		dc.DrawIcon(2, 2, hicon);
		DestroyIcon(hicon);
		SetWindowText("水文自动参数率定界面");		//设置菜单栏  可能是
	}
	else
	{
		CRect rect, recta, rectc, recto;	//CRect 矩形类 
		GetClientRect(&recta);				//获得窗口的当前的客户区
		rectc = recta;
		GetDlgItem(IDOK)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		int dx, dy;
		dx = recta.right - rect.right - 5;
		dy = recta.bottom - rect.bottom - 5;
		rect.right = recta.right - 8;
		rect.bottom = rect.bottom + dy;
		rect.top = rect.top + dy;
		GetDlgItem(IDOK)->MoveWindow(rect, TRUE);
		GetDlgItem(IDC_EDIT_ALF)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.right = recta.right;
		rect.bottom = rect.bottom + dy;
		rect.top = rect.top + dy;
		GetDlgItem(IDC_EDIT_ALF)->MoveWindow(rect, TRUE);

		GetDlgItem(IDC_STATIC_ADDRES_A)->GetWindowRect(&recto);
		ScreenToClient(&recto);
		recto.OffsetRect(CPoint(0, dy));
		GetDlgItem(IDC_STATIC_ADDRES_A)->MoveWindow(recto, TRUE);
		GetDlgItem(IDC_STATIC_ADDRES_B)->GetWindowRect(&recto);
		ScreenToClient(&recto);
		recto.OffsetRect(CPoint(0, dy));
		GetDlgItem(IDC_STATIC_ADDRES_B)->MoveWindow(recto, TRUE);
		GetDlgItem(IDC_COMBO_RIVER)->GetWindowRect(&recto);
		ScreenToClient(&recto);
		recto.OffsetRect(CPoint(0, dy));
		GetDlgItem(IDC_COMBO_RIVER)->MoveWindow(recto, TRUE);
		GetDlgItem(IDC_COMBO_ADDRES_A)->GetWindowRect(&recto);
		ScreenToClient(&recto);
		recto.OffsetRect(CPoint(0, dy));
		GetDlgItem(IDC_COMBO_ADDRES_A)->MoveWindow(recto, TRUE);
		GetDlgItem(IDC_COMBO_ADDRES_B)->GetWindowRect(&recto);
		ScreenToClient(&recto);
		recto.OffsetRect(CPoint(0, dy));
		GetDlgItem(IDC_COMBO_ADDRES_B)->MoveWindow(recto, TRUE);
		GetDlgItem(IDC_STATIC_BKCOLOR)->GetWindowRect(&recto);
		ScreenToClient(&recto);
		recto.OffsetRect(CPoint(0, dy));
		GetDlgItem(IDC_STATIC_BKCOLOR)->MoveWindow(recto, TRUE);
		GetDlgItem(IDC_BUTTON_CHANG)->GetWindowRect(&recto);
		ScreenToClient(&recto);
		recto.OffsetRect(CPoint(0, dy));
		GetDlgItem(IDC_BUTTON_CHANG)->MoveWindow(recto, TRUE);
		GetDlgItem(IDC_BUTTON_ADD)->GetWindowRect(&recto);
		ScreenToClient(&recto);
		recto.OffsetRect(CPoint(0, dy));
		GetDlgItem(IDC_BUTTON_ADD)->MoveWindow(recto, TRUE);
		//GetDlgItem(IDC_BUTTON_DELETE)->GetWindowRect(&recto);
		ScreenToClient(&recto);
		recto.OffsetRect(CPoint(0, dy));
		//GetDlgItem(IDC_BUTTON_DELETE)->MoveWindow(recto, TRUE);
		GetDlgItem(IDC_BUTTON_APPENDDATA)->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.OffsetRect(CPoint(0, dy));
		GetDlgItem(IDC_BUTTON_APPENDDATA)->MoveWindow(rect, TRUE);
		recta.bottom = rect.top - 2;
		m_GridReport.MoveWindow(recta, TRUE);
		recta.right = rect.right;
		m_Grid.MoveWindow(recta, TRUE);
		rectc.left = rect.right + 2;
		rectc.bottom = rect.top - 2;
		m_Graph.MoveWindow(rectc, TRUE);
		if (m_bIsReport)
		{
			m_GridReport.ShowWindow(SW_SHOW);
			m_Grid.ShowWindow(SW_HIDE);
			m_Graph.ShowWindow(SW_HIDE);
		}
		else
		{
			m_GridReport.ShowWindow(SW_HIDE);
			m_Grid.ShowWindow(SW_SHOW);
			m_Graph.ShowWindow(SW_SHOW);
		}
	}
	// Do not call CDialog::OnPaint() for painting messages
	//不要调用CDialog::OnPaint()函数绘制消息
}
//创造模型参数表格、报表表格、绘图
int CHydroPredictModelAutoParaPage::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_Grid.CreateGrid(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(0, 0, 0, 0), this, IDC_CUSTOM_GRIDDLL);//创建模型参数表格	左、上、右、下  CRect(5, 18, 125, 131)护眼绿
	m_GridReport.CreateGrid(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(0, 0, 0, 0), this, IDC_BUILDSYMBOL_GUI);//创建报表
	m_Graph.Create(NULL, "GraphWindows", WS_VISIBLE | WS_CHILD | WS_BORDER, CRect(0, 0, 0, 0), this, IDC_STATIC_GRAPH);//创建绘图区
	m_Graph.ModifyStyleEx(0, WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE, 0);//修改绘图区的风格
	return 0;
}
//水文序列改变  水文序列列表下拉框
void CHydroPredictModelAutoParaPage::OnSelchangeHohao() //水文序列改变   
{
	// TODO: Add your control notification handler code here 在此处添加控制通知处理程序
	UpdateData();	//
	int is = m_HohaoCombo.GetCurSel();	//被选中的水文序列GetCurSel():MFC m_HohaoCombo，CComboBox对象  GetCurSel():MFC
	if (is < 0) return;
	//这是什么意思
	m_HydroDataManage = m_HydroManageArray->GetAt(is);//被选中的水文序列
	//if (m_HydroDataManage != NULL)
	//	if (m_HydroDataManage->m_HongHao == m_LocalHydroManageArray[is]->m_HongHao)		//这是什么意思？ 如果被选中的水文序列与
	//		return;
	if (m_HydroDataManage != NULL)
		m_HydroDataManage->SaveHeadInfo();
	m_HydroDataManage = m_LocalHydroManageArray[is];//被选中的水文序列为本地水文序列的第i个
	BeginWaitCursor();
	m_HydroDataManage->StartRead();
	m_HydroDataManage->AssembleHydryData();

	UINT star = m_HydroDataManage->ResetStarOffset();
	if (star == 0)//判断选中水文序列是否引入数据
	{
		AfxMessageBox(_T("没有水文数据资料\n请重新从数据库、ASCI文件中引入数据"));
		return;
	}
	if (m_Number != star || !m_AO)
	{
		m_Number = star;
		if (m_AO != NULL)
		{
			delete m_AO;
			m_AO = NULL;
		}
		m_AO = new double[m_Number];
		memset(m_AO, 0, sizeof(double)*m_Number);
	}

	EndWaitCursor();

	//实测图形数据引入绘制		绘制实测流量、降水到界面
	if (m_DataOutManage != NULL)
	{
		m_DataOutManage->ResetNullData(m_HydroDataManage);
		m_DataOutManage->ResetBasicModel(m_BasicModel);
		m_DataOutManage->ResetChangHangHaoInfo(m_HydroDataManage, TRUE);
		m_Graph.Invalidate();
	}
}
//率定断面改变 率定断面下拉框
void CHydroPredictModelAutoParaPage::OnSelchangeHydroData() //率定断面改变
{
	// TODO: Add your control notification handler code here
	UpdateData();//更新数据函数，在很多地方用到，位于afxwin.h中
	int is = m_HydroDataCombo.GetCurSel();
	if (is < 0) return;
	is = m_HydroDataCombo.GetItemData(is);//
	m_HydroDataEmendation = m_HydroDataEmendationArray->GetAt(is);
	m_ComputePredictModel = (CHydroPredictModel *)m_HydroDataEmendation->m_ModelElement;
	m_InputPredictModelArray = &(m_ComputePredictModel->m_InputPredictModelArray);
	GetBasicMassageofModel();
	m_HydroModelCombo.ResetContent();
	int i, n;
	n = m_InputPredictModelArray->GetSize();//获取文档中的水文预报模型对象？
	for (i = 0; i < n; i++)
	{
		m_HydroModelCombo.AddString(m_InputPredictModelArray->GetAt(i)->m_Name);//在水文分区列表下拉单中设置水文分区名称?  这里的名称是率定断面以上各分区的名称
	}
	if (n > 0)
	{                                                                                                 
		m_HydroModelCombo.SetCurSel(0);
		OnSelchangeHydroModel();
	}
	m_DataOutManage = m_HydroDataEmendation->GetDataOutManage(2, &m_HydroDataOut, &m_ModelDataOut);
	if (m_DataOutManage != NULL)
	{
		m_DataOutManage->ResetGraphWnd(&m_Graph);
		m_DataOutManage->ResetBasicModel(m_BasicModel);//m_BasicModel
		m_DataOutManage->ResetModelElementArray();
		m_DataOutManage->AssembleGraphWnd(&m_Graph);
		if (m_HydroDataManage != NULL)
		{
			m_DataOutManage->ResetNullData(m_HydroDataManage);
			m_DataOutManage->ResetHydroData(m_BasicModel, m_HydroDataManage);
			m_DataOutManage->ResetChangHangHaoInfo(m_HydroDataManage, TRUE);
		}
		m_Graph.Invalidate();
	}
	n = m_LocalHydroManageArray.GetSize();
	UINT HongHao;
	for (i = 0; i < n; i++)
	{
		HongHao = m_LocalHydroManageArray[i]->m_HongHao;
		CChildHangHaoInfo * ChildHangHaoInfo = m_HydroDataEmendation->GetChildHongHaoInfoFromHongHao(HongHao);
		ChildHangHaoInfo->ResetNullDataForChild();
	}
	m_OutInfo.Format("分区流域面积：%.2f(KM^2)\r\n总流域面积：%.2f(KM^2)", m_ComputePredictModel->GetArea(), m_ComputePredictModel->GetSumArea());
	UpdateData(FALSE);
}
//获取水文模型的基本信息，包括模型种类数、参数名称、初值、上下限、是否参加率定等。
void CHydroPredictModelAutoParaPage::GetBasicMassageofModel()
{
	m_PredicModelTypeArray.clear();
	bool IsSave;//是否已经保存了此类水文模型
	bool IsSaveRiverClass;//是否已经保存了该河道级数
	double minPara, maxPara;//参数上下限
	int n = m_InputPredictModelArray->GetSize();//获取文档中的水文预报模型对象的个数 即流域分区个数

	//统计当前率定断面以上中预报模型类型数
	CVirtHydroModelDriver * HydroModelDriver = (((CHydroPredictModel *)m_InputPredictModelArray->GetAt(0))->GetFirstModelItem())->m_HydroModelDriver;
	CHydroPredictModel * HydroPredictModel = (CHydroPredictModel *)m_InputPredictModelArray->GetAt(0);
	m_PredicModelTypeArray.push_back(HydroModelDriver);
	m_RiverClass.push_back(HydroPredictModel->GetRiverClass());
	for (int i = 1; i < n; i++)
	{
		//模型Driver的种类
		IsSave = false;
		for (int j = 0; j < m_PredicModelTypeArray.size(); j++)
		{
			HydroModelDriver = (((CHydroPredictModel *)m_InputPredictModelArray->GetAt(i))->GetFirstModelItem())->m_HydroModelDriver;
			if (HydroModelDriver->m_ClassGuid == m_PredicModelTypeArray[j]->m_ClassGuid)
			{
				IsSave = true;
				break;
			}
		}
		if (!IsSave)
		{
			m_PredicModelTypeArray.push_back(HydroModelDriver);
		}
		//河道级数
		IsSaveRiverClass = false;
		HydroPredictModel = (CHydroPredictModel *)m_InputPredictModelArray->GetAt(i);
		for (int j = 0; j < m_RiverClass.size(); j++)
		{
			int Class = HydroPredictModel->GetRiverClass();
			if (HydroPredictModel->GetRiverClass() == m_RiverClass[j])
			{
				IsSaveRiverClass = true;
				break;
			}
		}
		if (!IsSaveRiverClass)
		{
			m_RiverClass.push_back(HydroPredictModel->GetRiverClass());
		}
	}
	int k = 0;
	int m = m_PredicModelTypeArray.size();//水文模型的种类数
	for (int i = 0; i < m; i++)//获取参数起始点
	{
		CVirtHydroModelDriver * HydroModelDriver = m_PredicModelTypeArray[i];
		int nPara;//参数个数
		nPara = HydroModelDriver->m_ParaNo - HydroModelDriver->m_MidVarNo;
		for (int j = 0; j < nPara; j++)
		{
			m_ParaInfo.Add(HydroModelDriver->GetParaTextTitle(j));//将参数名添加到数组对象中 afxcoll.nil	
			m_ParaData.push_back(HydroModelDriver->m_ParaData[j]);//
			HydroModelDriver->GetMinMaxPara(j, minPara, maxPara);//第j个参数
			m_minData.push_back(minPara);
			m_maxData.push_back(maxPara);
			m_LuDiInfo.Add(HydroModelDriver->GetLuDiInfo(j));//(是否参与率定|适合的目标函数)
			m_ModelDriverGuidPrePara.push_back(HydroModelDriver->m_ClassGuid);//记录该参数所属模型的类型
		}
	}
	//--所有的模型用同一套汇流参数
	m_ParaInfo.Add("传播时间AK[1]");//A
	m_ParaInfo.Add("流量比重系数AX");//AX
	//初始值
	m_ComputePredictModel->GetAKAX(minPara, maxPara);
	//TODO:此处计算A的初始值  在界面上
	HydroModelDriver = (((CHydroPredictModel *)m_InputPredictModelArray->GetAt(0))->GetFirstModelItem())->m_HydroModelDriver;
	minPara = minPara / (HydroModelDriver->m_RiverLength*pow(HydroModelDriver->m_RiverGradient, -0.5));
	m_ParaData.push_back(minPara);//A
	m_ParaData.push_back(maxPara);//AX
	//TODO:此处需确定A的取值范围
	m_minData.push_back(0);//A
	m_maxData.push_back(10 * maxPara);//A
	//流量比重系数AX的取值范围
	m_minData.push_back(-10);//AX
	m_maxData.push_back(0.5);//AX
	//模型序号
	//m_ModelDriverGuidPrePara.push_back(-1);//A
	//m_ModelDriverGuidPrePara.push_back(-1);//AX
	m_LuDiInfo.Add(CHydroPredictModel::m_LuDiInfo[0]);//A	//是否率定  A = A*(L/J^0.5)   
	m_LuDiInfo.Add(CHydroPredictModel::m_LuDiInfo[1]);//AX
}
//水文分区列表按钮 
void CHydroPredictModelAutoParaPage::OnSelchangeHydroModel() //水文模型区域改变  在率定断面下拉框选择后被调用一次
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if (m_Grid.GetModifiedFlag())
	{
		ResetGridData(FALSE);
	}
	int is = m_HydroModelCombo.GetCurSel();//获取下拉列表里面选中的标签id
	if (is < 0) return;
	m_HydroPredictModel = (CHydroPredictModel *)m_InputPredictModelArray->GetAt(is);//水文分区列表中被选中的那个分区模型  赋值，非指向
	m_HydroModelItem = m_HydroPredictModel->GetFirstModelItem();//赋值，非指向，所以操作后需回赋。
	m_OutInfo.Format("分区流域面积：%.2f(KM^2)\r\n总流域面积：%.2f(KM^2)", m_HydroPredictModel->GetArea(), m_HydroPredictModel->GetSumArea());		//将分区信息输出
	ResetGridData(TRUE);		//每次改变水文模型分区，便重新设置参数表格一次，在表格中显示当前分区的参数取值
}
//重新设置表格数据
void CHydroPredictModelAutoParaPage::ResetGridData(BOOL WR)//重新设置表格数据
{
	/*
	BOOL WR：是否重新设置表格数据
	*/
	if (m_HydroPredictModel == NULL) return;
	int i, j, k2, row, col;	//k2:列的循环控制变量  row 行	col 列
	CString strText;//字符串对象，用于存储单元格文本
	k2 = 8;//列
	int n;//流域分区的个数
	n = m_InputPredictModelArray->GetSize();//获取文档中的水文预报模型对象的个数 即流域分区个数
	int nParaPreModel = 0, nParaSum = 0;//参数总个数
	if (WR)//写表
	{
		m_Grid.OpenFile(BDSGetAppPath() + "一般通用表格模板.gcl");
		int nHydroModelType = m_PredicModelTypeArray.size();//模型类型数
		for (int i = 0; i < nHydroModelType; ++i)
		{
			nParaSum += (m_PredicModelTypeArray[i]->m_ParaNo - m_PredicModelTypeArray[i]->m_MidVarNo);
		}
		m_Grid.ResetGridSize(k2, nParaSum + 3);//重新设置表格大小  1行表头2行马法参数
		row = 0;//将行和列置  模型中所有的表格首行、首列都空着
		col = 0;
		//表头
		m_Grid.SetColWidth(col, 80);
		strText.Format(_T("模型算法"));
		m_Grid.QuickSetText(col++, row, strText);
		m_Grid.SetColWidth(col, 40);
		strText.Format(_T("序号"));
		m_Grid.QuickSetText(col++, row, strText);
		m_Grid.SetColWidth(col, 100);
		strText.Format(_T("参数名称"));
		m_Grid.QuickSetText(col++, row, strText);
		m_Grid.SetColWidth(col, 80);
		strText.Format(_T("参数值"));
		m_Grid.QuickSetText(col++, row, strText);
		m_Grid.SetColWidth(col, 60);
		strText.Format(_T("是否率定"));
		m_Grid.QuickSetText(col++, row, strText);
		m_Grid.SetColWidth(col, 60);
		strText.Format(_T("下限"));
		m_Grid.QuickSetText(col++, row, strText);
		m_Grid.SetColWidth(col, 60);
		strText.Format(_T("上限"));
		m_Grid.QuickSetText(col++, row, strText);
		m_Grid.SetColWidth(col, 100);
		strText.Format(_T("推荐目标函数"));
		m_Grid.QuickSetText(col, row++, strText);	//最后一列，行自加
		//
		col = 0;//重起一行
		//合并模型算法列 填入算法名称
		for (i = 0; i < nHydroModelType; i++)
		{
			nParaPreModel = m_PredicModelTypeArray[i]->m_ParaNo - m_PredicModelTypeArray[i]->m_MidVarNo;
			m_Grid.JoinCells0(col, row, col, row + nParaPreModel - 1);
			strText = m_PredicModelTypeArray[i]->GetTitleInfo();//模型算法名称
			m_Grid.QuickSetText(col, row, strText);
			row += nParaPreModel;
		}
		row = 1, col = 1;//模型类型
		int k = 0;
		for (i = 0; i < nHydroModelType; i++)
		{
			nParaPreModel = m_PredicModelTypeArray[i]->m_ParaNo - m_PredicModelTypeArray[i]->m_MidVarNo;
			for (j = 0; j < nParaPreModel; j++)//单个模型的参数个数
			{
				m_Grid.QuickSetNumber(col++, row, k + 1);//序号
				m_Grid.QuickSetText(col++, row, m_ParaInfo[k]);//参数名称
				m_Grid.QuickSetNumber(col++, row, m_ParaData[k]);//参数值
				strText = "N";
				m_LuDiInfo[k] = (m_LuDiInfo[k] & ~0xF) + (0x4&0xF);//
				if (m_DatType == ((m_LuDiInfo.GetAt(k) >> 4) & 0xF) & 0xF)
				{
					strText = "Y";
					m_LuDiInfo[k] = (m_LuDiInfo[k] & ~0xF) + (0x3 & 0xF);
				}
				if ((m_DatType == 0) && ((((m_LuDiInfo.GetAt(k) >> 4) & 0xF) & 0xF) == 4))
				{
					strText = "Y";
					m_LuDiInfo[k] = (m_LuDiInfo[k] & ~0xF) + (0x3 & 0xF);
				}
				m_Grid.QuickSetText(col++, row, strText);//是否参与率定
				m_Grid.QuickSetNumber(col++, row, m_minData[k]);//下限
				m_Grid.QuickSetNumber(col++, row, m_maxData[k]);//上限

				switch (((m_LuDiInfo.GetAt(k) >> 4) & 0xF) & 0xF)  //>>4? 为什么？
				{
					//0x1----为洪峰相对误差(最佳为零可正可负)
					//0x2----为平均值相对误差(最佳为零)  
					//0x3----为确定性系数(为最小)
					//0x4----为综合系数(ABS(洪峰相对误差)+(1.0-确定性系数)*100)
				case 1:
					strText.Format(_T("洪峰相对误差"));
					break;
				case 2:
					strText.Format(_T("均值相对误差"));
					break;
				case 3:
					strText.Format(_T("确定性系数"));
					break;
				case 4:
					strText.Format(_T("综合系数"));
					break;
				}
				m_Grid.QuickSetText(col, row, strText);//推荐目标函数
				col = 1;//重置为起始列
				row++;
				k++;
			}
		}
		//设置马法参数
		col = 0;
		m_Grid.JoinCells0(col, row, col, row + 1);
		m_Grid.QuickSetText(col++, row, "河道汇流模型");
		m_Grid.QuickSetNumber(col++, row, row);//设置参数序号
		strText.Format("传播时间AK[1]"/*"%d", m_HydroPredictModel->m_RiverNumber*/);//这里的河道数是什么？
		m_Grid.QuickSetText(col++, row, strText);
		strText.Format("%.3f", m_ParaData[nParaSum]);
		m_Grid.QuickSetText(col++, row, strText);//参数值
		//if ((CHydroPredictModel::m_LuDiInfo[0]) & 0X1)
		//	strText = "Y";
		//else
		//	strText = "N";
		if (m_DatType == 1)
		{
			strText = "Y";
			CHydroPredictModel::m_LuDiInfo[0] = (CHydroPredictModel::m_LuDiInfo[0]) & 0xFFFFFFFE | 0x1;
		}
		else
		{
			strText = "N";
			CHydroPredictModel::m_LuDiInfo[0] = (CHydroPredictModel::m_LuDiInfo[0]) & 0xFFFFFFFE;
		}
		m_Grid.QuickSetText(col++, row, strText);//是否率定
		m_Grid.QuickSetNumber(col++, row, m_minData[nParaSum]);//下限
		m_Grid.QuickSetNumber(col++, row, m_maxData[nParaSum]);//上限
		strText.Format("洪峰相对误差");
		m_Grid.QuickSetText(col, row++, strText);//推荐目标函数
		//设置AX
		col = 1;
		strText.Format("%d", row);//设置参数序号
		m_Grid.QuickSetText(col++, row, strText);
		strText.Format("流量比重系数AX");
		m_Grid.QuickSetText(col++, row, strText);
		strText.Format("%.3f", m_ParaData[nParaSum + 1]);//设置参数值
		m_Grid.QuickSetText(col++, row, strText);
		//if ((CHydroPredictModel::m_LuDiInfo[1]) & 0X1)
		//	strText = "Y";
		//else
		//	strText = "N";
		if (m_DatType == 1)
		{
			strText = "Y";
			CHydroPredictModel::m_LuDiInfo[1] = (CHydroPredictModel::m_LuDiInfo[1]) & 0xFFFFFFFE | 0x1;
		}
		else
		{
			strText = "N";
			CHydroPredictModel::m_LuDiInfo[1] = (CHydroPredictModel::m_LuDiInfo[1]) & 0xFFFFFFFE;
		}
		m_Grid.QuickSetText(col++, row, strText);//是否率定
		m_Grid.QuickSetNumber(col++, row, m_minData[nParaSum + 1]);//下限
		m_Grid.QuickSetNumber(col++, row, m_maxData[nParaSum + 1]);//上限
		strText.Format("洪峰相对误差");
		m_Grid.QuickSetText(col, row++, strText);//推荐目标函数

		m_Grid.SetModifiedFlag(FALSE);
		m_Grid.RecordRefresh();
		m_Grid.RedrawAll();
		UpdateData(FALSE);
	}
	else//读取表格中设置的参数
	{
		//3、4列
		UpdateData();//刷新数据表
		if (m_Grid.GetModifiedFlag())
		{
			if (m_HydroModelItem != NULL)
			{
				for (i = 0; i < m_ParaInfo.GetSize(); i++)
				{
					//参数值
					strText = m_Grid.QuickGetText(3, i + 1);//获取参数值  //第一行为表头
					if (strText.GetLength() > 0)
					{
						m_ParaData[i] = (atof((const char *)strText));
					}
					//是否参与率定
					strText = m_Grid.QuickGetText(4, i + 1);//获取是否参与率定
					if (strText == "Y" || strText == "y")
						m_LuDiInfo[i] = (m_LuDiInfo[i] & ~0xF) + (0x3 & 0xF);//0x3参与率定
					else
						m_LuDiInfo[i] = (m_LuDiInfo[i] & ~0xF) + (0x1 & 0xF);//0x1不参与率定
					//参数上下限
					m_minData[i] = m_Grid.QuickGetNumber(5, i + 1);
					m_maxData[i] = m_Grid.QuickGetNumber(6, i + 1);
				}
			}
			//将修改后的值赋回各个模型中
			int n = m_PredicModelTypeArray.size();//模型种类数
			int ParaID = 0;//参数id
			j = 0;
			for (i = 0; i < n; i++)
			{
				int nParaPerDriver = m_PredicModelTypeArray[i]->m_ParaNo - m_PredicModelTypeArray[i]->m_MidVarNo;
				for (int k = 0; k < nParaPerDriver; k++)
				{
					int LuDiInfo = m_LuDiInfo.GetAt(j);
					if ((LuDiInfo & 0x3) == 0x3)
					{
						m_ComputePredictModel->ResetParaForAll(m_ModelDriverGuidPrePara[j], k, m_ParaData[j]);
					}
					j++;
				}
			}
			//A
			int LuDiInfo = m_LuDiInfo.GetAt(j);
			if ((LuDiInfo & 0x3) == 0x3)
			{
				//CVirtHydroModelDriver * HydroModelDriver;
				CHydroPredictModel * HydroPredictModel;
				int n = m_InputPredictModelArray->GetSize();//获取文档中的水文预报模型对象的个数 即流域分区个数
				for (int k = 0; k < n; k++)//分别给每个模型设置AK、AX
				{
					HydroPredictModel = ((CHydroPredictModel *)m_InputPredictModelArray->GetAt(k));
					//HydroPredictModel = (CHydroPredictModel *)m_InputPredictModelArray->GetAt(k);
					if (HydroPredictModel->GetRiverGradient() == 0)
					{
						m_ComputePredictModel->ResetAKForAll(m_ParaData[j]);
					}
					else
					{
						for (int i = 0; i < HydroPredictModel->m_RiverNumber; i++)//每一个水文模型中的河段数
						{
							HydroPredictModel->m_AK[i] = m_ParaData[j] * (HydroPredictModel->GetRiverLengh())*pow(HydroPredictModel->GetRiverGradient(), -0.5);//K=A*L/(J^0.5)
							//TRACE("%d\n", HydroPredictModel->GetRiverClass());
							//TRACE("%0.3f\n", HydroPredictModel->m_AK[i]);
							//TRACE("%0.3f\n", HydroPredictModel->GetRiverLengh());
							//TRACE("%0.5f\n", HydroPredictModel->GetRiverGradient());
						}
					}
				}
			}
			//AX
			LuDiInfo = m_LuDiInfo.GetAt(j+1);
			if ((LuDiInfo & 0x3) == 0x3)
			{
				int n = m_InputPredictModelArray->GetSize();//获取文档中的水文预报模型对象的个数 即流域分区个数
				for (int k = 0; k < n; k++)//分别给每个模型设置AK、AX
				{
					m_ComputePredictModel->ResetAXForAll(m_ParaData[j+1]);//所有的模型用同一个AX
				}
			}
		}
	}
	m_Grid.SetModifiedFlag(FALSE);
}

void CHydroPredictModelAutoParaPage::ResetGridData(vector<double> ParaData)
{
	int col = 3, row = 1;
	for (int i = 0; i < m_ParaData.size(); i++)
	{
		m_Grid.QuickSetNumber(col, row++, m_ParaData[i]);
	}
	m_Grid.RecordRefresh();
	m_Grid.RedrawAll();
}
//计算马法参数AK的地形因子L/(J^0.5)
void CHydroPredictModelAutoParaPage::CalculateLJ()
{
	CVirtHydroModelDriver * HydroModelDriver;
	for (int i = 0; i < m_LJ.size(); i++)
	{
		HydroModelDriver = (((CHydroPredictModel *)m_InputPredictModelArray->GetAt(i))->GetFirstModelItem())->m_HydroModelDriver;
		m_LJ[i] = HydroModelDriver->m_RiverLength*pow(HydroModelDriver->m_RiverGradient, -0.5);
	}
}
//率定计算按钮
void CHydroPredictModelAutoParaPage::OnButtonCalibrationCalculate()  //单独计算	//yll 2018.11.12 率定计算按钮
{
	// TODO: Add your control notification handler code here
	UpdateData();//同步前后台数据
	if (m_Grid.GetModifiedFlag())//表格没有被改动时返回false,表格被修改后返回ture
	{
		ResetGridData(FALSE);//读取表格的数据
		m_ComputePredictModel->m_bChange = TRUE;//标识模型被改动了。
	}
	m_ComputePredictModel->ResetAutoLUDIFlag();

	//资料的统一化处理
	if (m_HydroDataManage == NULL)
	{
		if (AfxMessageBox("您是否决定率定所有水文序列！！！", MB_YESNO | MB_ICONWARNING) == IDYES)//率定所有的水文序列
		{
			double m_minSX, maxMax, maxAdev, DTime;
			//	m_ComputePredictModel->WriteRead(m_TempPath+"临时交换文件.TMP",FALSE);//写入初始条件
			ModelRun(m_minSX, maxMax, maxAdev, DTime);
			m_OutInfo.Format("确定性系数:%.3f 洪峰相对误差:%.1f％ 均值相对误差:%.1f％\r\n 峰现时间差:%.2f小时", m_minSX, maxMax, maxAdev, DTime);
			UpdateData(FALSE);
			//	CFile::Remove(m_TempPath+"临时交换文件.TMP");
		}
	}
	else//率定当前的水文序列
	{
		BOOL bla;
		CString info;

		m_BasicModel->m_Parent->m_HydroDataManage = m_HydroDataManage;//将当前选择的率定序列赋给当前文档的水文序列

		m_BasicModel->ResetRainParaForHongHaoRainPlan(m_HydroDataManage->m_CurRainScalePlan, m_HydroDataManage);//
		info = "水文序列：" + m_HydroDataManage->m_Text + "\n";
		bla = m_ComputePredictModel->IsCanRun(info, m_HydroDataManage);
		if (!bla)
		{
			m_OutInfo = info;
			return;
		}
		//		m_ComputePredictModel->WriteRead(m_TempPath+"临时交换文件.TMP",FALSE);//写入初始条件
		m_ComputePredictModel->RunHydroModel(m_DataOutManage, m_HydroDataManage, m_AO, m_ModelDataOut, &m_Rain0DataOut, &m_RainDataOut);
		//		m_ComputePredictModel->WriteRead(m_TempPath+"临时交换文件.TMP",TRUE);//读取初始条件
		//		CFile::Remove(m_TempPath+"临时交换文件.TMP");
		m_GraphInvalidate();//清空绘图区域
		double SX, RSum, CSum, DTime;
		GetStatDataInfo(SX, RSum, CSum, DTime);//获取这下面的4个值
		m_OutInfo.Format("确定性系数:%.3f 洪峰相对误差:%.1f％ 均值相对误差:%.1f％\r\n 峰现时间差：%.2f小时", SX, RSum, CSum, DTime);
		UpdateData(FALSE);
	}
}
//关闭对话框
void CHydroPredictModelAutoParaPage::OnDestroy()
{
	CDialog::OnDestroy();	//onDestory()函数关闭对话框
	if (m_AO != NULL)
	{
		delete m_AO;
		m_AO = NULL;
	}
	// TODO: Add your message handler code here
}
//获取状态数据信息
BOOL CHydroPredictModelAutoParaPage::GetStatDataInfo(COleDateTime &st, COleDateTime &et, double & mRain0, double & mRain, double &SX,
	double &Rmin, double &Cmin, COleDateTime &RminTim, COleDateTime &CminTim,
	double &Rmax, double &Cmax, COleDateTime &Rtim, COleDateTime &Ctim,
	double &RSum, double &CSum, double &RAdvData, double &CAdvData)
{
	/*
	COleDateTime & st  水文序列的开始时间
	COleDateTime & et  水文序列的结束时间
	double & mRain0
	double & mRain
	double &SX  确定性系数
	double &Rmin
	double &Cmin
	COleDateTime &RminTim
	COleDateTime &CminTim
	double &Rmax
	double &Cmax
	COleDateTime &Rtim
	COleDateTime &Ctim
	double &RSum
	double &CSum
	double &RAdvData
	double &CAdvData
	*/

	double area;	//流域面积
	area = m_ComputePredictModel->GetSumArea();		//子流域面积
	CDataOutItem *ItemR, *ItemC;	//数据输出项
	ItemR = m_HydroDataOut;		//水文数据输出对象
	ItemC = m_ModelDataOut;		//模型数据输出对象
	if (ItemR == NULL || ItemC == NULL) return FALSE;
	int m_DTime;
	m_DTime = m_BasicModel->m_HydroDtt;	//m_BasicModel->m_HydroDtt：时间步长(秒)、步长数   一个模型一个时间步长 、降雨预报时间步长
	//	m_DTime=3600;
	double da, DataR, DataC;
	TwoDimArray m_XY = NULL;	//typedef double ( *TwoDimArray)[2]; trans_xy=new double[m][2] 二维数组
	int i;
	m_XY = ItemR->m_Feature->m_XY;	//
	if (m_XY == NULL) return FALSE;	//
	da = m_XY[0][0];	//
	m_XY = ItemC->m_Feature->m_XY;
	if (m_XY == NULL) return FALSE;
	if (da < m_XY[0][0])
		da = m_XY[0][0];
	if (st.m_dt < da) st.m_dt = da;
	i = ItemR->m_Feature->mNumberOfPoint;
	da = ItemR->m_Feature->m_XY[i - 1][0];
	i = ItemC->m_Feature->mNumberOfPoint;
	if (da > ItemC->m_Feature->m_XY[i - 1][0])
		da = ItemC->m_Feature->m_XY[i - 1][0];
	if (et.m_dt > da) et.m_dt = da;
	st = GetModDateTime(st, m_DTime, TRUE);
	et = GetModDateTime(et, m_DTime, FALSE);
	if (st >= et) return FALSE;
	COleDateTime StarTime, EndTime, CurTime;	//开始时间 结束时间 当前时间
	StarTime = st;
	EndTime = et;
	ItemR->GetCharaDataFromTime(StarTime, EndTime, Rmin, RminTim, Rmax, Rtim, RSum, RAdvData);
	if (fabs(EndTime.m_dt - et.m_dt) > 1.0 / 86400.0 || fabs(StarTime.m_dt - st.m_dt) > 1.0 / 86400.0)
	{
		AfxMessageBox("ERROR！\n统计时间不对应!");
	}
	ItemC->GetCharaDataFromTime(StarTime, EndTime, Cmin, CminTim, Cmax, Ctim, CSum, CAdvData);
	if (fabs(EndTime.m_dt - et.m_dt) > 1.0 / 86400.0 || fabs(StarTime.m_dt - st.m_dt) > 1.0 / 86400.0)
	{
		AfxMessageBox("ERROR！\n统计时间不对应!");
	}
	StarTime = st;
	EndTime = et;
	int n, iCurR, iCurC;
	iCurR = iCurC = 0;
	BOOL blr, blc;
	double se = 0.0, sx = 0.0;
	COleDateTimeSpan mDTimeSpan(m_DTime / 86400.0);
	for (CurTime = StarTime; CurTime <= EndTime; CurTime = CurTime + mDTimeSpan)
	{
		blr = ItemR->GetDataFromTime(CurTime, DataR, iCurR);
		blc = ItemC->GetDataFromTime(CurTime, DataC, iCurC);
		if (blr&&blc)
		{
			se = se + (DataR - DataC)*(DataR - DataC);
			sx = sx + (DataR - RAdvData)*(DataR - RAdvData);
		}
	}
	if (sx != 0)
	{
		SX = 1.0 - se / sx;
	}
	else
	{
		SX = 0.0;
	}
	CAdvData = CAdvData*(EndTime - StarTime)*86400.0 / area / 1000.0;	//转化径流深
	RAdvData = RAdvData*(EndTime - StarTime)*86400.0 / area / 1000.0;	//转化径流深
	//	下面主要计算平均降雨量与净雨量
	mRain0 = mRain = 0.0;
	m_XY = m_Rain0DataOut.m_XY;
	n = m_Rain0DataOut.m_CurSerial;
	for (i = 0; i < n; i++)
	{
		da = m_XY[i][0];
		if ((da) > StarTime.m_dt && (da) <= EndTime.m_dt)
		{
			mRain0 += m_XY[i][1];
		}
		else if ((da) > EndTime.m_dt)
			break;
	}
	m_XY = m_RainDataOut.m_XY;
	n = m_RainDataOut.m_CurSerial;
	for (i = 0; i < n; i++)
	{
		da = m_XY[i][0];
		if ((da) > StarTime.m_dt && (da) <= EndTime.m_dt)
		{
			mRain += m_XY[i][1];
		}
		else if ((da) > EndTime.m_dt)
			break;
	}
	return TRUE;
}
//调整
void CHydroPredictModelAutoParaPage::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	Invalidate();	//_AFXWIN_INLINE void CWnd::Invalidate(BOOL bErase){ASSERT(::IsWindow(m_hWnd)); ::InvalidateRect(m_hWnd, NULL, bErase);}
}
//设置水文序列初始状态按钮
void CHydroPredictModelAutoParaPage::OnButtonChang()
{
	// TODO: Add your control notification handler code here
	CEditHongHaoBasinState dlg(m_HydroManageArray);
	dlg.DoModal();
	if (dlg.m_bChanged)
		m_ComputePredictModel->m_bChange = TRUE;
}
//重置控制状态  控制界面上各个按钮的状态，是否可点击
void CHydroPredictModelAutoParaPage::ResetControlState()
{
	GetDlgItem(IDC_COMBO_ADDRES_A)->EnableWindow(!m_bIsRun);
	GetDlgItem(IDC_COMBO_ADDRES_B)->EnableWindow(!m_bIsRun);
	GetDlgItem(IDOK)->EnableWindow(!m_bIsRun);
	GetDlgItem(IDC_BUTTON_CHANG)->EnableWindow(!m_bIsRun);
	GetDlgItem(IDC_BUTTON_ADD)->EnableWindow(m_ComputePredictModel != NULL&&m_HydroDataEmendation != NULL && !m_bIsRun);
	//GetDlgItem(IDC_BUTTON_DELETE)->EnableWindow(m_bIsRun);
	GetDlgItem(IDC_BUTTON_APPENDDATA)->EnableWindow(m_ComputePredictModel != NULL&&m_HydroDataEmendation != NULL && !m_bIsRun);
}
////停止率定	按钮
//void CHydroPredictModelAutoParaPage::OnButtonDelete() //中断自动率定计算
//{
//	// TODO: Add your control notification handler code here
//	PostMessage(RUNTIME_ALARM_TIMER_RUN);
//}

//获取计算结果
//编辑当前水文序列中子洪号 菜单
void CHydroPredictModelAutoParaPage::OnAutohydroparaEditchildHonghao()
{
	// TODO: Add your command handler code here
	if (m_HydroDataEmendation == NULL || m_HydroDataManage == NULL) return;
	UINT HongHao;
	HongHao = m_HydroDataManage->m_HongHao;
	CChildHangHaoInfo * ChildHangHaoInfo = m_HydroDataEmendation->GetChildHongHaoInfoFromHongHao(HongHao);//子洪号信息
	CEditHongHaoChild dlg(ChildHangHaoInfo);
	dlg.DoModal();
	if (m_BasicModel->m_ElementArry.GetSize() > 0 && dlg.m_bChang)
		m_BasicModel->m_ElementArry[0]->m_bChange = TRUE;
}
//编辑率定计算水文序列  按钮
void CHydroPredictModelAutoParaPage::OnAutohydroparaEditComputehonghao()
{
	// TODO: Add your command handler code here
	CEditLocalHongHaoPage dlg(m_HydroManageArray, &m_LocalHydroManageArray);		//编辑率定计算水文序列 对话框
	dlg.DoModal();
	m_HohaoCombo.ResetContent(); //重置内容 _AFXWIN_INLINE void CComboBox::ResetContent(){ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, CB_RESETCONTENT, 0, 0); }
	int i, n;
	n = m_LocalHydroManageArray.GetSize();	//获取参加率定计算的水文序列个数
	for (i = 0; i < n; i++)
	{
		m_HohaoCombo.AddString(m_LocalHydroManageArray[i]->m_Text);	//将参加率定计算的水文序列名称添加到洪号组合框
	}
	m_HydroDataManage = NULL;
}
//重设起止时间
void CHydroPredictModelAutoParaPage::OnGraphResetminmaxtime()
{
	// TODO: Add your command handler code here
	if (m_HydroDataManage == NULL || m_DataOutManage == NULL || m_HydroDataEmendation == NULL) return;
	COleDateTime m_QueryStarTime, m_QueryEndTime;
	int DTime;
	UINT HongHao;
	HongHao = m_HydroDataManage->m_HongHao;
	CChildHangHaoInfo * ChildHangHaoInfo = m_HydroDataEmendation->GetChildHongHaoInfoFromHongHao(HongHao);
	CEditDataTimeInterzone dlg(m_HydroDataManage->m_QueryStarTime, m_HydroDataManage->m_QueryEndTime, 864000, &ChildHangHaoInfo->m_RealHydroItemArray);
	if (dlg.DoModal() == IDOK)
	{
		m_QueryStarTime = dlg.GetStartTime();
		m_QueryEndTime = dlg.GetEndTime();
		DTime = dlg.m_Dtime;
		m_DataOutManage->ResetXAsixMinMaxTime(m_QueryStarTime, m_QueryEndTime, DTime);
		m_Graph.Invalidate();
	}
}
//计算当前选择的水文序列
BOOL CHydroPredictModelAutoParaPage::ModelRun(CHydroDataManage * mHydroDataManage, double &m_minSX, double &maxMax, double &maxAdev, double &DTime,vector<int> * vecPurposeofHonghao)
{//mHydroDataManage存储当前选中的水文序列，即率定当前选中的水文序列

	m_BasicModel->m_Parent->m_HydroDataManage = mHydroDataManage;

	mHydroDataManage->AssembleHydryData();
	UINT star = mHydroDataManage->ResetStarOffset();
	if (star == 0)
	{
		AfxMessageBox(_T("没有水文数据资料\n请重新从数据库、ASCI文件中引入数据"));
		return FALSE;
	}
	if (m_Number != star || !m_AO)
	{
		m_Number = star;
		if (m_AO != NULL)
		{
			delete m_AO;
			m_AO = NULL;
		}
		m_AO = new double[m_Number];
		memset(m_AO, 0, sizeof(double)*m_Number);
	}
	//mHydroDataManage->StartRead();
	m_DataOutManage->ResetNullData(mHydroDataManage);
	m_DataOutManage->ResetHydroData(m_BasicModel, mHydroDataManage);
	m_DataOutManage->ResetChangHangHaoInfo(mHydroDataManage, TRUE);//读取实测数据
	BOOL bla;
	CString info;
	info = "水文序列：" + mHydroDataManage->m_Text + "\n";
	bla = m_ComputePredictModel->IsCanRun(info, mHydroDataManage);
	UINT HongHao = mHydroDataManage->m_HongHao;
	CChildHangHaoInfo * ChildHangHaoInfo = m_HydroDataEmendation->GetChildHongHaoInfoFromHongHao(HongHao);
	if (!bla)
	{
		//	m_ComputePredictModel->WriteRead(m_TempPath+"临时交换文件.TMP",TRUE);//读取初始条件
		info = info + "\n 不能进行率定计算!";
		AfxMessageBox(info);
		int n = ChildHangHaoInfo->m_RealHydroItemArray.GetSize();
		for (int i = 0; i < n; i++)
		{
			SHydroDataManageItem ManageItem = ChildHangHaoInfo->m_RealHydroItemArray[i];
			ManageItem.m_CMin = 0;
			ManageItem.m_RMin = 0;
			ManageItem.m_CMinTime = 0;
			ManageItem.m_RMinTime = 0;
			ManageItem.m_CMax = 0;
			ManageItem.m_RMax = 0;
			ManageItem.m_CMaxTime = 0;
			ManageItem.m_RMaxTime = 0;
			ManageItem.m_RAdev = 0;
			ManageItem.m_CAdev = 0;
			ManageItem.m_SX = 0;
			ManageItem.m_Rain = 0;
			ManageItem.m_Rain0 = 0;
			ChildHangHaoInfo->m_RealHydroItemArray[i] = ManageItem;
		}
		return FALSE;
	}
	m_ComputePredictModel->GetLocalRunHydroDataArray(mHydroDataManage, m_HydroDataArray);//获取当地运行水文数据数组
	m_ComputePredictModel->RunHydroModel(m_DataOutManage, m_HydroDataArray, mHydroDataManage, m_AO, m_ModelDataOut, &m_Rain0DataOut, &m_RainDataOut);//计算水文模型
	//	m_ComputePredictModel->WriteRead(m_TempPath+"临时交换文件.TMP",TRUE);//读取初始条件
	//下面计算子洪号中的统计特征数据  下面计算各种目标函数值
	COleDateTime st, et, Rtim, Ctim, RminTim, CminTim;
	double SX, Rmax, Cmax, RSum, CSum, RAdvData, CAdvData, Rmin, Cmin, m_Rain0, m_Rain, sumR, sumA;
	m_minSX = 1.0;
	maxMax = 0;
	maxAdev = 0;
	DTime = 0.0;
	int i, n, kl = 0;
	sumR = 0.0;
	sumA = 0.0;
	n = ChildHangHaoInfo->m_RealHydroItemArray.GetSize();
	for (i = 0; i < n; i++)
	{
		if (vecPurposeofHonghao)
		{
			if ((vecPurposeofHonghao->at(i) != 1))
			{
				continue;
			}
		}
		SHydroDataManageItem ManageItem = ChildHangHaoInfo->m_RealHydroItemArray[i];
		st = ManageItem.m_StartTime;
		et = ManageItem.m_EndTime;
		ManageItem.ResetNullData();
		if (GetStatDataInfo(st, et, m_Rain0, m_Rain, SX, Rmin, Cmin, RminTim, CminTim, Rmax, Cmax, Rtim, Ctim, RSum, CSum, RAdvData, CAdvData))
		{
			ManageItem.m_CMin = Cmin;
			ManageItem.m_RMin = Rmin;
			ManageItem.m_CMinTime = CminTim;
			ManageItem.m_RMinTime = RminTim;
			ManageItem.m_CMax = Cmax;
			ManageItem.m_RMax = Rmax;
			ManageItem.m_CMaxTime = Ctim;
			ManageItem.m_RMaxTime = Rtim;
			ManageItem.m_RAdev = RAdvData;
			ManageItem.m_CAdev = CAdvData;
			ManageItem.m_Rain0 = m_Rain0;
			ManageItem.m_Rain = m_Rain;
			ManageItem.m_SX = SX;
			RSum = ((Cmax - Rmax) / Rmax)*100.0;
			CSum = ((CAdvData - RAdvData) / RAdvData)*100.0;
			if (ManageItem.m_bIsLU)
			{
				if (mID == 0)
				{
					if (fabs(RSum) > fabs(maxMax)) maxMax = RSum;
					if (fabs(CSum) > fabs(maxAdev)) maxAdev = CSum;
				}
				else if (mID == 1)
				{
					maxMax += RSum;
					maxAdev += CSum;
				}
				else if (mID == 2)
				{
					maxMax += RSum*RSum;
					maxAdev += CSum*CSum;
				}
				else if (mID == 3)
				{
					maxMax += RSum*RSum*Rmax;
					maxAdev += CSum*CSum*RAdvData;
					sumR += Rmax;
					sumA += RAdvData;
				}
				if (SX < m_minSX) m_minSX = SX;
				DTime = DTime + (Ctim - Rtim) * 24;//转为小时
				kl++;
			}
		}
		ChildHangHaoInfo->m_RealHydroItemArray[i] = ManageItem;
	}
	if (kl != 0)
	{
		if (mID == 1)
		{
			maxMax = maxMax / kl;
			maxAdev = maxAdev / kl;
		}
		else if (mID == 2)
		{
			maxMax = sqrt(maxMax / kl);
			maxAdev = sqrt(maxAdev / kl);
		}
		else if (mID == 3)
		{
			maxMax = sqrt(maxMax / sumR);
			maxAdev = sqrt(maxAdev / sumA);
		}
		DTime = DTime / kl;
	}
	return TRUE;
}
//计算水文序列
void CHydroPredictModelAutoParaPage::ModelRunAll(double &m_minSX, double &maxMax, double &maxAdev, double &DTime)
{
	if (m_HydroDataManage == NULL)  //计算所有水文序列  如果m_HydroDataManage为空，即没选中确定的水文序列，计算文档中的所有水文序列
	{
		ModelRun(m_minSX, maxMax, maxAdev, DTime);
	}
	else  //否则，计算选中的水文序列
	{
		m_BasicModel->ResetRainParaForHongHaoRainPlan(m_HydroDataManage->m_CurRainScalePlan, m_HydroDataManage);
		m_ComputePredictModel->RunHydroModel(m_DataOutManage, m_HydroDataManage, m_AO, m_ModelDataOut, &m_Rain0DataOut, &m_RainDataOut);
		GetStatDataInfo(m_minSX, maxMax, maxAdev, DTime);
	}
}
//率定所有水文序列
void CHydroPredictModelAutoParaPage::ModelRun(double &m_minSX, double &maxMax, double &maxAdev, double &DTime)//计算当前率定对象所有水文序列的结果，可以生成率定报表
{
	double m_minSX0, maxMax0, maxAdev0, DTime0;
	int i, n, _, mCurRainScalePlan = -1;
	UpdateData();
	if (m_DataOutManage == NULL) return;
	m_minSX = 1.0;
	maxMax = 0;
	maxAdev = 0;
	DTime = 0.0;
	n = m_LocalHydroManageArray.GetSize();
	_ = 0;
	for (i = 0; i < n; i++)
	{
		m_BasicModel->m_Parent->m_HydroDataManage = m_LocalHydroManageArray[i];
		m_LocalHydroManageArray[i]->StartRead();		//打开当前计算的水文序列
		if (mCurRainScalePlan != m_LocalHydroManageArray[i]->m_CurRainScalePlan)
		{
			mCurRainScalePlan = m_LocalHydroManageArray[i]->m_CurRainScalePlan;//雨量站方案
			m_BasicModel->ResetRainParaForHongHaoRainPlan(mCurRainScalePlan, m_LocalHydroManageArray[i]);
		}
		if (ModelRun(m_LocalHydroManageArray[i], m_minSX0, maxMax0, maxAdev0, DTime0))
		{
			if (m_minSX0 < m_minSX) m_minSX = m_minSX0;
			if (mID == 0)
			{
				if (fabs(maxMax0) > fabs(maxMax)) maxMax = maxMax0;
				if (fabs(maxAdev0) > fabs(maxAdev)) maxAdev = maxAdev0;
			}
			else if (mID == 1)
			{
				maxMax += maxMax0;
				maxAdev += maxAdev0;
			}
			else if (mID == 2 || mID == 3)
			{
				maxMax += maxMax0*maxMax0;
				maxAdev += maxAdev0*maxAdev0;
			}
			DTime = DTime + DTime0;
			m++;
		}
		m_LocalHydroManageArray[i]->SaveHeadInfo(); //关闭当前计算的水文序列
	}
	if (m != 0)
	{
		DTime = DTime / m;
		if (mID == 1)
		{
			maxMax = maxMax / m;
			maxAdev = maxAdev / m;
		}
		else if (mID == 2 || mID == 3)
		{
			maxMax = sqrt(maxMax / m);
			maxAdev = sqrt(maxAdev / m);
		}
	}
}
void CHydroPredictModelAutoParaPage::ModelRun(vector<vector<int>> & PurposeofHonghao,double &m_minSX, double &maxMax, double &maxAdev, double &DTime)//计算当前率定对象所有水文序列的结果，可以生成率定报表
{
	double m_minSX0, maxMax0, maxAdev0, DTime0;
	int i, n, m, mCurRainScalePlan = -1;

	if (m_DataOutManage == NULL) return;
	m_minSX = 1.0;
	maxMax = 0;
	maxAdev = 0;
	DTime = 0.0;
	n = m_LocalHydroManageArray.GetSize();
	m = 0;
	vector<bool> vecCal;
	vecCal.resize(n);
	for (i = 0; i < n; i++)
	{
		vecCal[i] = false;
		for (int j = 0; j < PurposeofHonghao[i].size(); ++j)
		{
			if (PurposeofHonghao[i][j])
			{
				vecCal[i] = true;
				break;
			}
		}
	}
	for (i = 0; i < n; i++)
	{
		if (!vecCal[i]) continue;
		m_BasicModel->m_Parent->m_HydroDataManage = m_LocalHydroManageArray[i];
		m_LocalHydroManageArray[i]->StartRead();		//打开当前计算的水文序列
		if (mCurRainScalePlan != m_LocalHydroManageArray[i]->m_CurRainScalePlan)
		{
			mCurRainScalePlan = m_LocalHydroManageArray[i]->m_CurRainScalePlan;
			m_BasicModel->ResetRainParaForHongHaoRainPlan(mCurRainScalePlan, m_LocalHydroManageArray[i]);
		}
		if (ModelRun(m_LocalHydroManageArray[i], m_minSX0, maxMax0, maxAdev0, DTime0, &PurposeofHonghao[i]))
		{
			if (m_minSX0 < m_minSX) m_minSX = m_minSX0;
			if (mID == 0)
			{
				if (fabs(maxMax0) > fabs(maxMax)) maxMax = maxMax0;
				if (fabs(maxAdev0) > fabs(maxAdev)) maxAdev = maxAdev0;
			}
			else if (mID == 1)
			{
				maxMax += maxMax0;
				maxAdev += maxAdev0;
			}
			else if (mID == 2 || mID == 3)
			{
				maxMax += maxMax0*maxMax0;
				maxAdev += maxAdev0*maxAdev0;
			}
			DTime = DTime + DTime0;
			m++;
		}
		m_LocalHydroManageArray[i]->SaveHeadInfo();		//关闭当前计算的水文序列
	}
	if (m != 0)
	{
		DTime = DTime / m;
		if (mID == 1)
		{
			maxMax = maxMax / m;
			maxAdev = maxAdev / m;
		}
		else if (mID == 2 || mID == 3)
		{
			maxMax = sqrt(maxMax / m);
			maxAdev = sqrt(maxAdev / m);
		}
	}
}
//输出当前水文序列的计算成果 标记
void CHydroPredictModelAutoParaPage::OnAutohydroparaOutHonghaojg()
{
	// TODO: Add your command handler code here
	if (m_HydroDataEmendation == NULL) return;
	ofstream outfile;
	CFileDialog file(FALSE, NULL, NULL, OFN_OVERWRITEPROMPT, "文本文件(*.Dat)|*.Dat||");
	file.m_ofn.lpstrTitle = "输出水文率定成果文本文件";
	file.m_ofn.Flags |= OFN_CREATEPROMPT;
	if (file.DoModal() == IDOK)
	{
		char buf[256];
		TRY
		{
			outfile.open((const char *)file.GetPathName(),ios::out | ios::trunc);
		}
		CATCH(CFileException, e)
		{
			sprintf(buf, "Cannot open data from file:\n%s", (const char *)file.GetPathName());
			::MessageBox(NULL, buf, "File I/O Error", MB_ICONEXCLAMATION | MB_OK);
			return;
		}
		END_CATCH
			sprintf(buf, "%s计算成果", (const char *)m_HydroDataEmendation->m_Name);
		outfile << buf << endl;
		int i, n;
		UINT HongHao;
		n = m_LocalHydroManageArray.GetSize();
		for (i = 0; i < n; i++)
		{
			HongHao = m_LocalHydroManageArray[i]->m_HongHao;
			CChildHangHaoInfo * ChildHangHaoInfo = m_HydroDataEmendation->GetChildHongHaoInfoFromHongHao(HongHao);
			ChildHangHaoInfo->OutChildHongHaoDataInfo(outfile);
		}
		outfile.clear();
		outfile.close();
	}
}
//显示当前水文序列的计算成果 菜单
void CHydroPredictModelAutoParaPage::OnAutohydroparaShowHonghaojg()
{
	// TODO: Add your command handler code here
	m_bIsReport = !m_bIsReport;
	if (m_bIsReport)
	{
		int i, n, row, col, j, m;
		int k = 0;
		CString strText;
		double dat;
		UINT HongHao;	//typedef unsigned int  UINT;	无符号整数
		n = m_LocalHydroManageArray.GetSize();	//获取洪号管理数组的维数  即模型中有几个水文序列
		col = 0;
		for (i = 0; i < n; i++)
		{
			CHydroDataManage * mHydroDataManage = m_LocalHydroManageArray[i];	//
			HongHao = mHydroDataManage->m_HongHao;		//获取洪号个数
			CChildHangHaoInfo * ChildHangHaoInfo = m_HydroDataEmendation->GetChildHongHaoInfoFromHongHao(HongHao);
			m = ChildHangHaoInfo->m_RealHydroItemArray.GetSize();		//
			for (j = 0; j < m; j++)
			{
				SHydroDataManageItem ManageItem = ChildHangHaoInfo->m_RealHydroItemArray[j];
				if (ManageItem.m_RMax == 0) continue;
				k++;
			}
		}
		m_GridReport.OpenFile(BDSGetAppPath() + "一般通用表格模板.gcl");
		m_GridReport.ResetGridSize(14, k + 2);//设置表格大小
		m_GridReport.LockGrid(1, 1);
		strText = CString("率定成果报表:") + m_HydroDataEmendation->m_Name;	//	m_Name：为流域名称
		m_GridReport.JoinCells0(0, 0, 13, 0);
		m_GridReport.QuickSetText(0, 0, strText);
		row = 1;
		col = 0;
		strText.Format(_T("序号"));
		m_GridReport.SetColWidth(col, 60);
		m_GridReport.QuickSetText(col++, row, strText);
		strText.Format(_T("水文序列"));
		m_GridReport.SetColWidth(col, 100);
		m_GridReport.QuickSetText(col++, row, strText);
		strText.Format(_T("降雨量"));
		m_GridReport.SetColWidth(col, 100);
		m_GridReport.QuickSetText(col++, row, strText);
		strText.Format(_T("净雨量"));
		m_GridReport.SetColWidth(col, 100);
		m_GridReport.QuickSetText(col++, row, strText);
		strText.Format(_T("确定性系数"));
		m_GridReport.SetColWidth(col, 100);
		m_GridReport.QuickSetText(col++, row, strText);
		strText.Format(_T("实测洪峰"));
		m_GridReport.SetColWidth(col, 100);
		m_GridReport.QuickSetText(col++, row, strText);
		strText.Format(_T("计算洪峰"));
		m_GridReport.SetColWidth(col, 100);
		m_GridReport.QuickSetText(col++, row, strText);
		strText.Format(_T("洪峰相对误差"));
		m_GridReport.SetColWidth(col, 100);
		m_GridReport.QuickSetText(col++, row, strText);
		strText.Format(_T("实测峰现时间"));
		m_GridReport.SetColWidth(col, 100);
		m_GridReport.QuickSetText(col++, row, strText);
		strText.Format(_T("计算峰现时间"));
		m_GridReport.SetColWidth(col, 100);
		m_GridReport.QuickSetText(col++, row, strText);
		strText.Format(_T("峰现时间差"));
		m_GridReport.SetColWidth(col, 100);
		m_GridReport.QuickSetText(col++, row, strText);
		strText.Format(_T("实测径流深"));
		m_GridReport.SetColWidth(col, 100);
		m_GridReport.QuickSetText(col++, row, strText);
		strText.Format(_T("计算径流深"));
		m_GridReport.SetColWidth(col, 100);
		m_GridReport.QuickSetText(col++, row, strText);
		strText.Format(_T("径流深相对误差"));
		m_GridReport.SetColWidth(col, 100);
		m_GridReport.QuickSetText(col++, row, strText);
		k = 1;
		for (i = 0; i < n; i++)
		{
			CHydroDataManage * mHydroDataManage = m_LocalHydroManageArray[i];
			HongHao = mHydroDataManage->m_HongHao;
			CChildHangHaoInfo * ChildHangHaoInfo = m_HydroDataEmendation->GetChildHongHaoInfoFromHongHao(HongHao);//
			m = ChildHangHaoInfo->m_RealHydroItemArray.GetSize();//洪号个数
			for (j = 0; j < m; j++)
			{
				SHydroDataManageItem ManageItem = ChildHangHaoInfo->m_RealHydroItemArray[j];//
				if (ManageItem.m_RMax == 0) continue;
				strText.Format("%d", k);
				col = 0;
				row = k + 1;
				m_GridReport.QuickSetText(col++, row, strText);
				strText = ManageItem.GetHongHaoInfo();
				m_GridReport.QuickSetText(col++, row, strText);
				strText.Format("%.1f", ManageItem.m_Rain0);
				m_GridReport.QuickSetText(col++, row, strText);
				strText.Format("%.2f", ManageItem.m_Rain);
				m_GridReport.QuickSetText(col++, row, strText);
				strText.Format("%.2f", ManageItem.m_SX);//确定性系数
				m_GridReport.QuickSetText(col++, row, strText);
				strText.Format("%10.2f", ManageItem.m_RMax);
				m_GridReport.QuickSetText(col++, row, strText);
				strText.Format("%10.2f", ManageItem.m_CMax);
				m_GridReport.QuickSetText(col++, row, strText);
				dat = (ManageItem.m_CMax - ManageItem.m_RMax);
				if (ManageItem.m_RMax == 0)
					dat = 100;
				else
					dat = dat / ManageItem.m_RMax * 100;
				strText.Format("%10.2f", dat);
				m_GridReport.QuickSetText(col++, row, strText);
				strText = COleDateTime(ManageItem.m_RMaxTime).Format("%m月%d日%H时");
				m_GridReport.QuickSetText(col++, row, strText);
				strText = COleDateTime(ManageItem.m_CMaxTime).Format("%m月%d日%H时");
				m_GridReport.QuickSetText(col++, row, strText);
				strText.Format("%.1f小时", double(ManageItem.m_CMaxTime - ManageItem.m_RMaxTime) * 24);//转为小时
				m_GridReport.QuickSetText(col++, row, strText);
				strText.Format("%10.2f", ManageItem.m_RAdev);
				m_GridReport.QuickSetText(col++, row, strText);
				strText.Format("%10.2f", ManageItem.m_CAdev);
				m_GridReport.QuickSetText(col++, row, strText);
				if (ManageItem.m_RAdev == 0)
					dat = 100.0;
				else
					dat = (ManageItem.m_CAdev - ManageItem.m_RAdev) / ManageItem.m_RAdev*100.0;
				strText.Format("%10.2f", dat);
				m_GridReport.QuickSetText(col++, row, strText);
				k++;
			}
		}
		m_GridReport.SetModifiedFlag(FALSE);
		m_GridReport.RedrawAll();
	}
	Invalidate();
}
//
void CHydroPredictModelAutoParaPage::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	OnAutohydroparaShowHonghaojg();
	CDialog::OnLButtonDblClk(nFlags, point);
}
//利用计算结果统计误差  获取状态数据信息
void CHydroPredictModelAutoParaPage::GetStatDataInfo(double &m_minSX, double &maxMax, double &maxAdev, double &DTime)
{
	/*
		将这些变量的地址传到函数里面，函数便可直接操作变量所在内存。
		double &m_minSX 确定性系数
		double &maxMax 洪峰相对误差
		double &maxAdev 均值相对误差
		double &DTime 峰现时间差
		double m_RMax, m_CMax, m_RMin, m_CMin, m_RAdev, m_CAdev, m_SX, m_Rain0, m_Rain;		real  calculate
		实测最大值、计算最大值、实测最小值、计算最小值、实测平均值、计算平均值、确定性系数,平均降雨量与平均净雨量
	*/			//st:start time		et:end time
	COleDateTime st, et, Rtim, Ctim, RminTim, CminTim;	//COleDateTime:A COleDateTime value represents an absolute date and time value.一种时间类型
	double SX, Rmax, Cmax, RSum, CSum, RAdvData, CAdvData, Rmin, Cmin, Rain0, Rain, sumR, sumA;//确定性系数 实测最大值 计算最大值 实测和 计算和 实测均值 计算均值 实测最小值 计算最小值 平均降雨量 平均净雨量
	int i, n;	//循环控制变量 水文序列长度
	UINT HongHao;		//UINT:unsigned int  HongHao:洪号数量
	HongHao = m_HydroDataManage->m_HongHao;		//水文序列管理页面的水文序列
	CChildHangHaoInfo * ChildHangHaoInfo = m_HydroDataEmendation->GetChildHongHaoInfoFromHongHao(HongHao);//主要用于率定时保存计算与实测结果对比用,对每一个率定数据均有一个子洪号信息与之对应
	n = ChildHangHaoInfo->m_RealHydroItemArray.GetSize();//当前水文序列子洪号的个数
	m_minSX = 1.0;	//设置确定性系数的初始值
	maxMax = maxAdev = 0.0;		//设置洪峰相对误差、均值相对误差的初值
	sumR = sumA = 0.0;	//
	DTime = 0.0;	//峰现时间差
	int kl = 0;		//
	for (i = 0; i < n; i++)		//n 水文序列的长度
	{
		SHydroDataManageItem ManageItem = ChildHangHaoInfo->m_RealHydroItemArray[i];	//主要用于率定时保存计算与实测结果对比用  是一个结构
		st = ManageItem.m_StartTime;	//开始时间
		et = ManageItem.m_EndTime;		//结束时间
		ManageItem.ResetNullData();		//管理项目.重置空数据
		if (GetStatDataInfo(st, et, Rain0, Rain, SX, Rmin, Cmin, RminTim, CminTim, Rmax, Cmax, Rtim, Ctim, RSum, CSum, RAdvData, CAdvData))
		{
			if (/*fabs(RAdvData)>1.0&&*/fabs(Rmax) > 1.0)	//Rmax 实测最大值	下面计算的是哪一个变量的统计值呢？流量？
			{
				ManageItem.m_CMin = Cmin;		//计算最小值	
				ManageItem.m_RMin = Rmin;		//实测最小值
				ManageItem.m_CMinTime = CminTim;	//
				ManageItem.m_RMinTime = RminTim;	//
				ManageItem.m_CMax = Cmax;	//计算最大值
				ManageItem.m_RMax = Rmax;	//实测最大值
				ManageItem.m_CMaxTime = Ctim;	//
				ManageItem.m_RMaxTime = Rtim;	//
				ManageItem.m_RAdev = RAdvData;	//实测平均值
				ManageItem.m_CAdev = CAdvData;	//计算平均值
				ManageItem.m_SX = SX;		//确定性系数
				ManageItem.m_Rain0 = Rain0;	//平均降雨量
				ManageItem.m_Rain = Rain;	//平均净雨量
				RSum = ((Cmax - Rmax) / Rmax)*100.0;	//计算洪峰 实测洪峰 洪峰相对误差
				CSum = ((CAdvData - RAdvData) / RAdvData)*100.0;	//计算均值 实测均值 
				if (ManageItem.m_bIsLU)		//m_bIsLU:区分率定与验证，= TRUE为率定
				{
					if (mID == 0)
					{
						if (fabs(RSum) > fabs(maxMax)) maxMax = RSum;		//洪峰相对误差
						if (fabs(CSum) > fabs(maxAdev)) maxAdev = CSum;		//均值相对误差
					}
					else if (mID == 1)
					{
						maxMax += RSum;
						maxAdev += CSum;
					}
					else if (mID == 2)
					{
						maxMax += RSum*RSum;
						maxAdev += CSum*CSum;
					}
					else if (mID == 3)
					{
						maxMax += RSum*RSum*Rmax;
						maxAdev += CSum*CSum*RAdvData;
						sumA += RAdvData;
						sumR += Rmax;
					}
					if (SX < m_minSX) m_minSX = SX;
					DTime = DTime + (Ctim - Rtim) * 24;			//转为小时
					kl++;
				}
			}
		}
		ChildHangHaoInfo->m_RealHydroItemArray[i] = ManageItem;
	}
	if (kl != 0)
	{
		if (mID == 1)
		{
			maxMax = maxMax / kl;
			maxAdev = maxAdev / kl;
		}
		else if (mID == 2)
		{
			maxMax = sqrt(maxMax / kl);
			maxAdev = sqrt(maxAdev / kl);
		}
		else if (mID == 3)
		{
			maxMax = sqrt(maxMax / sumR);
			maxAdev = sqrt(maxAdev / sumA);
		}
		DTime = DTime / kl;			//DTime 峰现时间差
	}
}
//参数率定页关闭按钮
void CHydroPredictModelAutoParaPage::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	if (m_bIsReport)
	{
		AfxMessageBox(_T("请在图形状态下关闭！"));
		return;
	}
	if (m_HydroDataManage != NULL)
		m_HydroDataManage->SaveHeadInfo();
	CDialog::OnClose();
}
//输出子洪号报表 菜单
void CHydroPredictModelAutoParaPage::OnAutohydroparaEditHonghaoreport()
{
	// TODO: Add your command handler code here
	COutHongHaoChildReport dlg(m_BasicModel, m_HydroDataEmendationArray);
	dlg.DoModal();
}
//输出汇流报表
void CHydroPredictModelAutoParaPage::OnAutohydroparaEditHydroparaReport() //输出汇流报表
{
	// TODO: Add your command handler code here
	if (m_BasicModel->m_HydroPredictModelArray.GetSize() > 0)
	{
		COutHongHaoChildReport dlg(m_BasicModel, &m_BasicModel->m_HydroPredictModelArray, 1);
		dlg.DoModal();
	}
	else
	{
		COutHongHaoChildReport dlg(m_BasicModel, m_BasicModel->m_MapElement, 1);
		dlg.DoModal();
	}
}
//输出产流报表
void CHydroPredictModelAutoParaPage::OnAutohydroparaEditHydroparaItemreport() //输出产流报表
{
	// TODO: Add your command handler code here
	if (m_BasicModel->m_HydroPredictModelArray.GetSize() > 0)
	{
		COutHongHaoChildReport dlg(m_BasicModel, &m_BasicModel->m_HydroPredictModelArray, 2); //申明一个名为dlg的COutHongHaoChildReport类对象，此类有4个构造函数，并将构造函数的参数传入。重载。
		dlg.DoModal();
	}
	else
	{
		COutHongHaoChildReport dlg(m_BasicModel, m_BasicModel->m_MapElement, 2);
		dlg.DoModal();
	}
}
//获取参数的最大最小值
BOOL CHydroPredictModelAutoParaPage::GetDataFromMinMaxData(int l, double minData, double maxData, double DData, double & aData, double &Data, double &bData)
{
	BOOL bl = TRUE;
	switch (l)
	{
	case 0:
	{
		if (Data - minData <= 0 || Data - maxData >= 0)
			bl = FALSE;
		else
		{
			aData = Data - DData;
			if (aData < minData)
				aData = minData;
			bData = Data + DData;
			if (bData > maxData)
				bData = maxData;
		}
	}
	break;
	case 1:
	{
		if (Data - maxData >= 0)
			bl = FALSE;
		else
		{
			bData = Data + DData;
			if (bData > maxData)
				bData = maxData;
		}
	}
	break;
	case 2:
	{
		if (Data - minData <= 0)
			bl = FALSE;
		else
		{
			aData = Data - DData;
			if (aData < minData)
				aData = minData;
		}
	}
	break;
	default:
		bl = FALSE;
		break;
	}
	return bl;
}
//
double CHydroPredictModelAutoParaPage::GetResult(int DatType, double SX, double Cmax, double CAdvData)
{
	double Result = 0;
	switch (DatType)
	{
	case 0://0----为综合系数(ABS(洪峰相对误差)+ABS(1.0-确定性系数)*100
		//Result = fabs(Cmax) + fabs(1.0 - SX)*100.0;//主要强调过程  最小为0  Cmax，洪峰相对误差；SX，确定性系数。
		Result = 1.0 - SX;
		break;
	case 1://1----为洪峰相对误差(最佳为零可正可负)
		Result = fabs(Cmax);//最小为0
		break;
	case 2://2----平均值相对误差(最佳为零)  
		Result = fabs(CAdvData);//最小为0
		break;
	case 3://3----为确定性系数(为最小)
		Result = 1.0 - SX;//最小为0
		break;
	} 
	return Result;
}
//返回SCE-UA目标值
double CHydroPredictModelAutoParaPage::CaculateTargetValue(double * ParaS)	//yll  2018.11.08
{
	//产流参数
	int i, j = 0, mParaNumber, nModelType;
	nModelType = m_PredicModelTypeArray.size();

	mParaNumber = m_bl.size();
	for (i = 0; i < mParaNumber; i++)
	{
		j = m_ModelDriverIndex[i];//参数和模型driver一一对应
		if (j == -1)//-1表示马斯京根河道汇流模型
		{
			//处理AK、AX参数回写
			if (m_ParaIndex[i] < 0 && m_ParaIndex[i] != -11)
			{
				CHydroPredictModel * HydroPredictModel;
				int n = m_InputPredictModelArray->GetSize();//获取文档中的水文预报模型对象的个数 即流域分区个数
				for (int k = 0; k < n; k++)//分别给每个模型设置A
				{
					HydroPredictModel = ((CHydroPredictModel *)m_InputPredictModelArray->GetAt(k));
					if (HydroPredictModel->GetRiverGradient() == 0)//如果河道梯度=0，说明水文模型不来自DEM。
					{
						HydroPredictModel->ResetAK(ParaS[i]);//单独给手绘的水文模型设置参数A。
					}
					else
					{
						for (int ii = 0; ii < HydroPredictModel->m_RiverNumber; ii++)
						{
							for (int jj = 0; jj < m_ParaIndex.size(); jj++)
							{
								if (HydroPredictModel->GetRiverClass() == -m_ParaIndex[jj] - 1)
								{
									HydroPredictModel->m_AK[ii] = ParaS[(mParaNumber - 1) - m_RiverClass.size() + jj] * (HydroPredictModel->GetRiverLengh())*pow(HydroPredictModel->GetRiverGradient(), -0.5);//K=A*L/(J^0.5)
								}
							}
						}
					}
				}
			}
			if (m_ParaIndex[i] == -11)//AX
			{
				m_ComputePredictModel->ResetAXForAll(ParaS[i]);
			}
		}
		else//否则为
		{
			//回写各个模型的参数
			CVirtHydroModelDriver * HydroModelDriver = m_PredicModelTypeArray[j];
			m_ComputePredictModel->ResetParaForAll(HydroModelDriver->GetClassGuid(), m_ParaIndex[i], ParaS[i]);		//否则将同一Guid的该参数设置为ParaS
		}
	}	
	//资料的统一化处理
	double m_minSX, maxMax, maxAdev, DTime;
	ModelRun(m_PurposeofHonghao,m_minSX, maxMax, maxAdev, DTime);
	
	//return 1 - m_minSX;
	return GetResult(m_DatType, m_minSX, maxMax, maxAdev);
}

//自动率定按钮 点击自动率定按钮后程序跳转到此函数，执行函数体
void CHydroPredictModelAutoParaPage::OnButtonAutomaticCalibration() //开始自动率定计算	//yll 2018.11.08  增加SCE-UA算法
{
	// TODO: Add your control notification handler code here
	//1.获取率定断面以上包含哪些子区域。各子区域是用的什么模型
	//2.把这些模型参数的初值、上下限都读取出来，规定一下ParaS里面个参数的顺序
	//创建一个SCEUA优化器
	//把this指针作为参数调用RunSceua（this,………………)
	UpdateData();//同步前后台数据
	if (m_Grid.GetModifiedFlag())//表格没有被改动时返回false,表格被修改后返回ture
	{
		ResetGridData(FALSE);//读取表格的数据
		m_ComputePredictModel->m_bChange = TRUE;//标识模型被改动了。
	}
	m_ComputePredictModel->ResetAutoLUDIFlag();

	CSequencePartitioningandSelectionPage dlgSps(m_HydroDataEmendation, &m_LocalHydroManageArray, m_PurposeofHonghao);
	if (dlgSps.DoModal() != IDOK)
	{
		return;
	}

	int mParaNumber = 0, LuDiInfo;//参数个数 是否参加率定
	int n = m_PredicModelTypeArray.size();//模型种类数
	int i, j,k = 0;
	CString str;
	for (i = 0; i < n; i++)
	{
		CVirtHydroModelDriver * HydroModelDriver = m_PredicModelTypeArray[i];
		int temp = HydroModelDriver->m_ParaNo - HydroModelDriver->m_MidVarNo;
		for (j = 0; j < temp; j++)
		{
			LuDiInfo = m_LuDiInfo[k];//获取第j个参数是否参加率定的信息
			if ((LuDiInfo & 0x3) == 0x3)
			{
				m_StartPoint.push_back(m_ParaData[k]);
				m_ParaName.Add(m_ParaInfo[k]);
				m_bl.push_back(m_minData[k]);
				m_bu.push_back(m_maxData[k]);
				m_ParaIndex.push_back(j);
				m_ModelDriverIndex.push_back(i);
			}
			k++;
		}
	}
	LuDiInfo = m_LuDiInfo[k];//A
	if ((LuDiInfo & 0x3) == 0x3)
	{
		for (i = 0; i < m_RiverClass.size(); i++)//不同河道级数的用不同的综合系数A 
		{
			m_StartPoint.push_back(m_ParaData[k]);
			str.Format("综合系数A(%d级河道)", m_RiverClass[i]);
			m_ParaName.Add(str);
			m_bl.push_back(m_minData[k]);
			m_bu.push_back(m_maxData[k]);
			m_ParaIndex.push_back(-(m_RiverClass[i]+1));//A
			m_ModelDriverIndex.push_back(-1);
		}
	}
	LuDiInfo = m_LuDiInfo[k+1];//AX
	if ((LuDiInfo & 0x3) == 0x3)
	{
		m_StartPoint.push_back(m_ParaData[k+1]);
		m_ParaName.Add("流量比重系数AX");
		m_bl.push_back(m_minData[k+1]);
		m_bu.push_back(m_maxData[k+1]);
		m_ParaIndex.push_back(-11);//AX 用-11表示
		m_ModelDriverIndex.push_back(-1);//通用参数
	}
	m_sceua.SetData(this, m_ParaName, m_StartPoint, m_bl, m_bu);
	ModelParameterOptimizePage dlg(&m_sceua,m_DatType);
	dlg.DoModal();
	CHydroPredictModelParaChoosePage dlgShowResultAndChoose(this);//总是在这里报有未经处理的异常
	dlgShowResultAndChoose.DoModal();
	//将选中的参数组回写到参数总集中
	m_beSelectedParam = dlgShowResultAndChoose.GetbeSelectedParam();
	if (m_beSelectedParam < m_sceua.GetParaGroupCountNeed() + 1 && m_beSelectedParam > 1)
	{
		//产流参数
		int i, j = 0, nParaNumber, nModelType;
		nModelType = m_PredicModelTypeArray.size();
		nParaNumber = m_bl.size();//参与率定的参数总数
		int nParaSum = m_ParaData.size();//参数总数
		m_ParaData[nParaSum - 2] = 0;
		for (i = 0; i < nParaNumber; i++)
		{
			if (m_ParaIndex[i] < 0 && m_ParaIndex[i] != -11)//A
			{
				m_ParaData[nParaSum - 2] += m_sceua.m_TopXParaGroup[m_beSelectedParam - 2][i];
			}
			else if (m_ParaIndex[i] == -11)//AX
			{
				m_ParaData[nParaSum - 1] = m_sceua.m_TopXParaGroup[m_beSelectedParam - 2][i];
			}
			else
			{
				m_ParaData[m_ParaIndex[i]] = m_sceua.m_TopXParaGroup[m_beSelectedParam - 2][i];//将待率定参数回写到模型整体参数里面，方便写表格和调用单独计算的函数。
			}
		}
		m_ParaData[nParaSum - 2] /= m_RiverClass.size();//A取各级数A的平均值。
	}
	m_bIsRun = FALSE;//判断是否在计算
	ResetControlState();
	ResetGridData(m_ParaData);
	CaculateTargetValue(&m_sceua.m_TopXParaGroup[m_beSelectedParam - 2][0]);
	if (!m_HydroDataManage || m_LocalHydroManageArray.GetSize() <= 1)//这个地方最后绘图还有点问题
	{
		OnButtonCalibrationCalculate();//当未选择水文序列或者文档中只有一个水文序列时，调用率定计算函数，绘图。
	}
	m_ComputePredictModel->m_bChange = TRUE;
	//一次自动率定完全结束后，清空自动率定使用的数组，以便下次使用
	m_bl.clear();
	m_bu.clear();
	m_StartPoint.clear();
	m_ParaIndex.clear();
	m_ModelDriverIndex.clear();
}

void CHydroPredictModelAutoParaPage::UpdateMenuUI()		//更新菜单UI
{
	CMenu * MainMenu = GetMenu();
	if (MainMenu == NULL) return;
	MainMenu->CheckMenuItem(IDM_AUTOPARA_MAX, MF_BYCOMMAND | (mID == 0 ? MF_CHECKED : MF_UNCHECKED));
	MainMenu->CheckMenuItem(IDM_AUTOPARA_ADV, MF_BYCOMMAND | (mID == 1 ? MF_CHECKED : MF_UNCHECKED));
	MainMenu->CheckMenuItem(IDM_AUTOPARA_STAT, MF_BYCOMMAND | (mID == 2 ? MF_CHECKED : MF_UNCHECKED));
	MainMenu->CheckMenuItem(IDM_AUTOPARA_STAQ, MF_BYCOMMAND | (mID == 3 ? MF_CHECKED : MF_UNCHECKED));
	MainMenu->CheckMenuItem(IDM_AUTOPARA_DEFAULT, MF_BYCOMMAND | (m_DatType == 0 ? MF_CHECKED : MF_UNCHECKED));//综合系数
	MainMenu->CheckMenuItem(IDM_AUTOPARA_MAX_DAT, MF_BYCOMMAND | (m_DatType == 1 ? MF_CHECKED : MF_UNCHECKED));//洪峰相对误差
	MainMenu->CheckMenuItem(IDM_AUTOPARA_ADV_DATA, MF_BYCOMMAND | (m_DatType == 2 ? MF_CHECKED : MF_UNCHECKED));//均值相对误差
	MainMenu->CheckMenuItem(IDM_AUTOPARA_SX_DATA, MF_BYCOMMAND | (m_DatType == 3 ? MF_CHECKED : MF_UNCHECKED));//确定性系数
}
//界面最大化
void CHydroPredictModelAutoParaPage::OnAutoparaMax()
{
	// TODO: Add your command handler code here
	mID = 0;
	UpdateMenuUI();
}
//界面由最大化缩小到适中
void CHydroPredictModelAutoParaPage::OnAutoparaAdv()
{
	// TODO: Add your command handler code here
	mID = 1;
	UpdateMenuUI();
}
//改变界面大小
void CHydroPredictModelAutoParaPage::OnAutoparaStat()
{
	// TODO: Add your command handler code here
	mID = 2;
	UpdateMenuUI();
}
//改变界面大小
void CHydroPredictModelAutoParaPage::OnAutoparaStaq()
{
	// TODO: Add your command handler code here
	mID = 3;
	UpdateMenuUI();
}
//率定目标函数 系统缺省
void CHydroPredictModelAutoParaPage::OnAutoparaDefault()
{
	// TODO: Add your command handler code here
	m_DatType = 0;
	ResetGridData(true);
	UpdateMenuUI();
}
//率定目标函数 洪峰相对误差
void CHydroPredictModelAutoParaPage::OnAutoparaMaxDat()
{
	// TODO: Add your command handler code here
	m_DatType = 1;
	ResetGridData(true);
	UpdateMenuUI();
}
//率定目标函数 均值相对误差
void CHydroPredictModelAutoParaPage::OnAutoparaAdvData()
{
	// TODO: Add your command handler code here
	m_DatType = 2;
	ResetGridData(true);
	UpdateMenuUI();
}
//率定目标函数 均方值
void CHydroPredictModelAutoParaPage::OnAutoparaSxData()
{
	// TODO: Add your command handler code here
	m_DatType = 3;
	ResetGridData(true);
	UpdateMenuUI();
}
//查询初始条件信息 菜单
void CHydroPredictModelAutoParaPage::OnAutohydroparaQureBasinInitidata()
{
	// TODO: Add your command handler code here
	if (m_HydroDataManage == NULL || m_ComputePredictModel == NULL) return;
	COleDateTime StarTime, EndTime;
	StarTime = m_HydroDataManage->m_QueryStarTime;
	EndTime = m_HydroDataManage->m_QueryEndTime;
	if (m_HydroDataEmendation == NULL)
	{
		int i, n;
		CHydroDataArray HydroDataArray;
		m_ComputePredictModel->GetLocalRunHydroDataArray(m_HydroDataManage, HydroDataArray);
		n = HydroDataArray.GetSize();
		for (i = 0; i < n; i++)
		{
			if (HydroDataArray[i]->GetCode() != eHydroRail)
			{
				if (StarTime < COleDateTime(HydroDataArray[i]->m_StarTime))
					StarTime = COleDateTime(HydroDataArray[i]->m_StarTime);
				if (EndTime > COleDateTime(HydroDataArray[i]->m_EndTime))
					EndTime = COleDateTime(HydroDataArray[i]->m_EndTime);
			}
		}
	}
	else
	{
		if (m_ComputePredictModel->m_HydroDataEmendation->m_HydroDataItem != NULL)
			StarTime = m_ComputePredictModel->m_HydroDataEmendation->m_HydroDataItem->m_StarTime;
	}
	StarTime = GetModDateTime(StarTime, m_BasicModel->m_HydroDtt, TRUE);
	EndTime = GetModDateTime(EndTime, m_BasicModel->m_HydroDtt, FALSE);
	m_BasicModel->ResetInitiCondit(TRUE);
	m_ComputePredictModel->ResetBasinInitiStae(m_HydroDataManage, TRUE);
	m_BasicModel->ResetHydroInitiRiverFlow(m_HydroDataManage, StarTime, m_BasicModel->m_PreAreaBaseFlow);
	CPropertySheet dlg("初始条件编辑");
	CEditInitiData DataPage(m_BasicModel, FALSE, ePredictHydro);
	dlg.AddPage(&DataPage);
	dlg.DoModal();
}

//王老师给的代码
void CHydroPredictModel::RunHydroModel(CDataOutManage * DataOutManage, CHydroDataManage * HydroDataManage, double * mAO,
	CDataOutItem * DataOutItem, CDataOutItem * m_Rain0DataOut, CDataOutItem * m_RainDataOut)//仅仅运行水文模型
{
	CHydroDataArray HydroDataArray;
	GetLocalRunHydroDataArray(HydroDataManage, HydroDataArray);
	for (int i = 0; i < HydroDataArray.GetSize(); i++)
	{
		HydroDataArray[i]->m_bHaveData = FALSE;
	}
	RunHydroModel(DataOutManage, HydroDataArray, HydroDataManage, mAO, DataOutItem, m_Rain0DataOut, m_RainDataOut);//仅仅运行水文模型
}

void CHydroPredictModel::RunHydroModel(CDataOutManage * DataOutManage, CHydroDataArray &HydroDataArray, CHydroDataManage * HydroDataManage, double * mAO,
	CDataOutItem * DataOutItem, CDataOutItem * m_Rain0DataOut, CDataOutItem * m_RainDataOut)//仅仅运行水文模型
{
	COleDateTime curTime, StarTime, EndTime;
	COleDateTimeSpan HydroDT(m_Parent->m_HydroDtt / 86400.0);
	bool bl;
	double data;
	int i, n;
	if (DataOutItem != NULL)
		DataOutItem->ResetNullData();
	if (m_Rain0DataOut != NULL)
		m_Rain0DataOut->ResetNullData();
	if (m_RainDataOut != NULL)
		m_RainDataOut->ResetNullData();
	StarTime = HydroDataManage->m_QueryStarTime;
	EndTime = HydroDataManage->m_QueryEndTime;
	StarTime = GetModDateTime(StarTime, m_Parent->m_HydroDtt, TRUE);
	EndTime = GetModDateTime(EndTime, m_Parent->m_HydroDtt, FALSE);
	m_Parent->ResetRainErrorDispos();
	m_Parent->ResetInitiCondit(TRUE);
	ResetBasinInitiStae(HydroDataManage, TRUE);//设置流域初始状态(水文模型的初始条件)
	m_Parent->ResetHydroInitiRiverFlow(HydroDataManage, StarTime, m_Parent->m_PreAreaBaseFlow);
	n = m_InputPredictModelArray.GetSize();
	CHydroPredictModel * Item = NULL;
	for (curTime = StarTime + HydroDT; curTime <= EndTime; curTime = curTime + HydroDT)
	{
		bl = TRUE;
		for (i = 0; i < n; i++)
		{
			Item = (CHydroPredictModel *)m_InputPredictModelArray[i];
			bl = bl&&Item->ResetIntiInPQ(HydroDataManage, curTime);
		}
		if (!bl) break;
		bl = HydroDataManage->CurRead(&HydroDataArray, mAO, curTime, m_Parent->m_HydroDtt, TRUE);//获得降雨数据	这里有个bug
		//这里要求处理雨量资料的冒大数问题
		GetParent()->RainErrorDispos(mAO);
		GetRainData(mAO);//计算平均降雨量、及降雨量缺测时的处理
		for (i = 0; i < n; i++)
		{
			Item = (CHydroPredictModel *)m_InputPredictModelArray[i];
			Item->RunHydroModel(mAO);
		}
		for (i = 0; i < n; i++)
		{
			Item = (CHydroPredictModel *)m_InputPredictModelArray[i];
			Item->BackRun_A();
		}
		//计算流域平均雨量
		m_SumAdevRainO = m_SumAdevRain = 0.0;
		for (i = 0; i < n; i++)
		{
			CHydroPredictModel * Item = ((CHydroPredictModel *)m_InputPredictModelArray[i]);
			m_SumAdevRainO = m_SumAdevRainO + Item->m_AdevRainO*Item->GetArea();
			m_SumAdevRain = m_SumAdevRain + Item->m_AdevRain*Item->GetArea();
		}
		m_SumAdevRainO = m_SumAdevRainO / m_SumArea;
		m_SumAdevRain = m_SumAdevRain / m_SumArea;
		if (DataOutManage != NULL)
		{
			DataOutManage->ResetTimeOutData(HydroDataManage, curTime);
		}
		else
		{
			//写入图形数据,CDataOutItem * DataOutItem
			if (DataOutItem != NULL)
			{
				i = *(int *)(DataOutItem->m_AddressItem.m_ItemID);
				if (i<0 || i>m_RiverNumber) i = m_RiverNumber;
				data = m_QQ[i];
				DataOutItem->AddData(double(curTime), data, FALSE);
			}
		}
		if (m_Rain0DataOut != NULL&&m_SumAdevRainO > 0)
		{
			data = m_SumAdevRainO;
			m_Rain0DataOut->AddData(double(curTime), data, FALSE);
		}
		if (m_RainDataOut != NULL&&m_SumAdevRain > 0)
		{
			data = m_SumAdevRain;
			m_RainDataOut->AddData(double(curTime), data, FALSE);
		}
	}
	if (DataOutItem != NULL)
	{
		DataOutItem->m_Feature->ResetMaxMin();
	}
}