//#include <windows.h>
//#include <stdlib.h>        //CHARת��ΪWCHAR��Ҫ�õ����ļ�  
#include <locale.h>  //CHARת��ΪWCHARʱ��Ҫ���ػ�������ת�������ַ�����  
#include <vector>
#include <tchar.h>
#include <iostream>
#include <Oleacc.h>
#pragma comment(lib,"Oleacc.lib")

struct WinAccObj{
	LPCTSTR m_ClsName;		// ���ڴ�������
	LPCTSTR m_WndName;		// ��������
	LPCTSTR m_RoleName;		// ����role name
    LPCTSTR m_Description;  // ������Ϣ
    TCHAR   m_Value[MAX_PATH];  // ֵ
    RECT    m_rect;         // ��������λ��

    /*
    @param mainClsName ����������
    @param wndName ��������
    @param roleName ����role name
    @param clsName ���ڴ�������(�е�������������һ��)
    */
	WinAccObj(
        LPCTSTR wndName = TEXT(""), LPCTSTR roleName = TEXT(""),
        LPCTSTR clsName = TEXT(""), LPCTSTR descpition = TEXT(""))
	{
		m_WndName = wndName;
		m_RoleName = roleName;
		m_ClsName = clsName;
        m_Description = descpition;

		m_WndName = (NULL == m_WndName) ? TEXT("") : m_WndName;
		m_RoleName = (NULL == m_RoleName) ? TEXT("") : m_RoleName;
		m_ClsName = (NULL == m_ClsName ) ? TEXT("") : m_ClsName;
        m_Description = (NULL == m_Description) ? TEXT("") : m_Description;
        memset(m_Value, 0, MAX_PATH);
        m_rect.left = 0; m_rect.right = 0; m_rect.top = 0; m_rect.bottom = 0;
	}
};


// class AccObjectData{
// public:
//     /*
//     @param mainClsName ����������
//     @param wndName ��������
//     @param roleName ����role name
//     @param clsName ���ڴ�������(�е�������������һ��)
//     */
// 	AccObjectData(LPCTSTR mainClsName = TEXT("") ,
//         LPCTSTR wndName = TEXT(""),LPCTSTR roleName = TEXT(""),
//         LPCTSTR clsName = TEXT(""))
// 	{
// 		m_MainClsName = mainClsName;
// 		m_WndName = wndName;
// 		m_RoleName = roleName;
// 		m_ClsName = clsName;
// 
// 		m_MainClsName = (NULL == m_MainClsName) ? TEXT("") : m_MainClsName;
// 		m_WndName = (NULL == m_WndName) ? TEXT("") : m_WndName;
// 		m_RoleName = (NULL == m_RoleName) ? TEXT("") : m_RoleName;
// 		m_ClsName = (NULL == m_ClsName || TEXT("") == m_ClsName) ? m_MainClsName : m_ClsName;
// 		m_hMainWnd = NULL;
// 		m_hAddressWnd = NULL;
// 		m_ptCenterAddress.x = 0;
// 		m_ptCenterAddress.y = 0;
// 	}
// 
// 	~AccObjectData(){}
// 
// 	LPCTSTR m_MainClsName;	// ����������
// 	LPCTSTR m_ClsName;		// ���ڴ�������
// 	LPCTSTR m_WndName;		// ��������
// 	LPCTSTR m_RoleName;		// ����role name
// 	HWND m_hMainWnd;		// �����ھ��
// 	HWND m_hAddressWnd;		// �������ڴ��ڵľ��
// 	POINT m_ptCenterAddress;	// �������ĵ�λ�ã���Ļ���꣩
//     RECT  rect;             // ��������λ��
// 
// 	void GetBrowserMainWnd();
// 	void GetAddressCenter();
// private:
// 	void GetAddressHwnd();
// };

/**  
   WindowAccessHelper �����ռ������ȡ���ھ����MSAA��ع��ܡ�ģ���������ģ�������ַ����Ƚӿ�
*/
namespace WindowAccessHelper{
	typedef struct tagEnumWindowStruct{
		std::vector<HWND> hwnd_vec;
		LPCTSTR wndClass;
		LPCTSTR wndName;
	} EnumWndStruct;
	
	BOOL CALLBACK  EnumWindowProc(HWND hwnd, LPARAM lParam)
	{
		EnumWndStruct *  struc = (EnumWndStruct *)lParam;
		TCHAR ClassName[MAX_PATH];
		GetClassName(hwnd, ClassName, MAX_PATH);
		TCHAR WndName[MAX_PATH];
		GetWindowText(hwnd, WndName, MAX_PATH);
		if (NULL == struc->wndClass && NULL == struc->wndName)
		{
			return FALSE;
		}
		if (NULL == struc->wndClass || _tcscmp(struc->wndClass, TEXT("")) == 0 )
		{
			if(_tcscmp(WndName, struc->wndName) == 0)
			{
				struc->hwnd_vec.push_back(hwnd);
			}
			return TRUE;
		}
		else if (NULL == struc->wndName || _tcscmp(struc->wndName, TEXT("")) == 0 )
		{
			if( _tcscmp(ClassName, struc->wndClass) == 0)
			{
				struc->hwnd_vec.push_back(hwnd);
			}
			return TRUE;
		}
		else if (_tcscmp(ClassName, struc->wndClass) == 0 && _tcscmp(WndName, struc->wndName) == 0)
		{
			struc->hwnd_vec.push_back(hwnd);
		}
		return TRUE;
	}

	// ���ݴ����������������Ҷ�������
	bool FindMainWnds(std::vector<HWND> & h_vec, LPCTSTR clsName, LPCTSTR wndName = NULL)
	{
		h_vec.clear();
		std::vector<HWND> wnds;
		EnumWndStruct wndStruct;
		wndStruct.hwnd_vec = wnds;
		wndStruct.wndClass = clsName;
		wndStruct.wndName = wndName;

		EnumWindows(EnumWindowProc,(LPARAM)(&wndStruct));
		wnds = wndStruct.hwnd_vec;
		if (wnds.empty())
		{
			return false;
		}

		// �������classnameһ�µĴ��ڿ����ж������һ�������Ӵ��ڣ���һ�����ɼ���

// 		std::vector<HWND> v_wndsVisable;
// 		for (std::vector<HWND>::iterator it = wnds.begin(); it != wnds.end(); ++it)
// 		{
// 			if (IsWindowVisible(*it))
// 			{
// 				v_wndsVisable.push_back(*it);
// 			}
// 		}


		for (std::vector<HWND>::iterator it = wnds.begin(); it != wnds.end(); ++it)
		{
			if (IsWindowVisible(*it))//FindWindowEx(*it,NULL,NULL,NULL) != NULL
			{
				h_vec.push_back(*it);
			}
		}
		if (h_vec.empty())
		{
			return false;
		}
		return true;
	}

	// ���ݴ�����������������ĳ�������ڵ��Ӵ��ڿؼ�
	void FindSubWindows(HWND hParent, std::vector<HWND> & h_vec, LPCTSTR clsName, LPCTSTR wndName = NULL)
	{
		h_vec.clear();
		std::vector<HWND> wnds;
		EnumWndStruct wndStruct;
		wndStruct.hwnd_vec = wnds;
		wndStruct.wndClass = clsName;
		wndStruct.wndName = wndName;
		EnumChildWindows(hParent, EnumWindowProc, (LPARAM)(&wndStruct));
		h_vec = wndStruct.hwnd_vec;
//		wnds = wndStruct.hwnd_vec;
// 		if (wnds.empty())
// 		{
// 			return;
// 		}
// 		for (std::vector<HWND>::iterator it = wnds.begin(); it != wnds.end(); ++it)
// 		{
// 			h_vec.push_back(*it);
// 		}
	}

	// ��ȡ�����޸ĵ�����������ڣ����ж�����ڣ���Ѱ�һ����
	// �е�������磺�ѹ�������֮��������������������ݴ���û�и��ӹ�ϵ��
	// ����������ʱ��ȡ���Ļ���������ݴ��ڲ��������ڡ���������Ľ�
	HWND GetHwndToModify(std::vector<HWND> h_vec)
	{
		if (h_vec.empty())
		{
			return NULL;
		}

		HWND frontWnd = GetForegroundWindow();
		if (h_vec.size() == 1)
		{
			return *(h_vec.begin());
		}

		for (std::vector<HWND>::iterator it = h_vec.begin(); it != h_vec.end(); ++it)
		{
			if (*it == frontWnd)
			{
				return *it;
			}
		}
		return NULL;
	}

	// UIԪ�ص�״̬Ҳ��ʾ��������ʽ����Ϊһ��״̬�����ж��ֵ��
	// �����ѡ�ġ���������ģ��������Ƿ�ӳ��Щֵ��λ�Ļ���������
	// ����Щ����ת������Ӧ���ö��ŷָ��״̬�ַ�����
	UINT GetObjectState(IAccessible* pacc, 
		VARIANT* pvarChild, 
		LPTSTR lpszState, 
		UINT cchState)
	{
		HRESULT hr;
		VARIANT varRetVal;

		*lpszState = 0;

		VariantInit(&varRetVal);

		hr = pacc->get_accState(*pvarChild, &varRetVal);

		if (!SUCCEEDED(hr))
			return(0);

		DWORD dwStateBit;
		int cChars = 0;
		if (varRetVal.vt == VT_I4)
		{
			// ���ݷ��ص�״ֵ̬�����Զ������ӵ��ַ�����
			for (dwStateBit = STATE_SYSTEM_UNAVAILABLE; 
				dwStateBit < STATE_SYSTEM_ALERT_HIGH; 
				dwStateBit <<= 1)
			{
				if (varRetVal.lVal & dwStateBit)
				{
					cChars += GetStateText(dwStateBit, 
						lpszState + cChars, 
						cchState - cChars);
					*(lpszState + cChars++) = ',';
				}
			}
			if(cChars > 1)
				*(lpszState + cChars - 1) = '\0';
		}
		else if (varRetVal.vt == VT_BSTR)
		{
			WideCharToMultiByte(CP_ACP, 
				0, 
				varRetVal.bstrVal, 
				-1, 
				(LPSTR)lpszState,
				cchState, 
				NULL, 
				NULL);
		}

		VariantClear(&varRetVal);

		return(lstrlen(lpszState));
	}

	//ͨ��get_accName�õ�Name
	UINT GetObjectName(IAccessible* pacc, VARIANT* pvarChild, LPTSTR lpszName, UINT cchName)
	{
		HRESULT hr;
		BSTR bstrName;

		*lpszName = 0;
		bstrName = NULL;

		hr = pacc->get_accName(*pvarChild, &bstrName);

		if (SUCCEEDED(hr) && bstrName)
		{
#if defined(UNICODE) || defined (_UNICODE)
			UINT len = ::SysStringLen(bstrName);
			_tcsncpy(lpszName, bstrName, len);
			lpszName[len] = L'\0';
#else
			WideCharToMultiByte(CP_ACP, 0, bstrName, -1, lpszName, cchName, NULL, NULL);
#endif
			SysFreeString(bstrName);
		}

		return(lstrlen(lpszName));
	}

	//ͨ��get_accRole�õ�Role
	UINT GetObjectRole(IAccessible* pacc, VARIANT* pvarChild, LPTSTR lpszRole, UINT cchRole)
	{
		HRESULT hr;
		VARIANT varRetVal;

		*lpszRole = 0;

		VariantInit(&varRetVal);

		hr = pacc->get_accRole(*pvarChild, &varRetVal);

		if (!SUCCEEDED(hr))
			return(0);

		if (varRetVal.vt == VT_I4)
		{
			//����GetRoleText���������͵Ľ�ɫ��ʾת��Ϊ�ַ�����ʾ
			GetRoleText(varRetVal.lVal, lpszRole, cchRole);
		}
		else if (varRetVal.vt == VT_BSTR)
		{//��BSTRת��ΪlpszRole�����ͣ�LPSTR����
#if defined(UNICODE) || defined (_UNICODE)
			//��BSTRת��ΪLPWSTR��
			UINT len = ::SysStringLen(varRetVal.bstrVal);
			_tcsncpy(lpszRole, varRetVal.bstrVal, len);
 #else
			//��BSTRת��ΪLPSTR��
			WideCharToMultiByte(CP_ACP, 0, varRetVal.bstrVal, -1, lpszRole,
				cchRole, NULL, NULL);
 #endif
			//SysFreeString(varRetVal.bstrVal);
		}

		VariantClear(&varRetVal);

		return(lstrlen(lpszRole));
	}

	//ͨ��WindowFromAccessibleObject��GetClassName�õ�Class
	UINT GetObjectClass(IAccessible* pacc, LPTSTR lpszClass, UINT cchClass)
	{
		HWND hWnd;
		if(S_OK == WindowFromAccessibleObject(pacc,  &hWnd))
		{
			if(hWnd)
				GetClassName(hWnd, lpszClass, cchClass);
			else
				_tcscpy_s(lpszClass, cchClass, TEXT("No window"));
		}

		return 1;
	}

    UINT GetAccObjectDescription(IAccessible* pacc, VARIANT* pvarChild, LPTSTR lpszDesc, UINT cchDesc)
    {
        HRESULT hr;
        BSTR bstrDesc;

        *lpszDesc = 0;
        bstrDesc = NULL;

        hr = pacc->get_accDescription(*pvarChild, &bstrDesc);

        if (SUCCEEDED(hr) && bstrDesc)
        {
#if defined(UNICODE) || defined (_UNICODE)
            UINT len = ::SysStringLen(bstrDesc);
            _tcsncpy(lpszDesc, bstrDesc, len);
            lpszDesc[len] = L'\0';
#else
            WideCharToMultiByte(CP_ACP, 0, bstrDesc, -1, lpszDesc, cchDesc, NULL, NULL);
#endif
            SysFreeString(bstrDesc);
        }

        return(lstrlen(lpszDesc));
    }

    UINT GetObjectValue(IAccessible* pacc, VARIANT* pvarChild, LPTSTR lpszVal, UINT cchVal)
    {
        HRESULT hr;
        BSTR bstrVal;

        *lpszVal = 0;
        bstrVal = NULL;

        hr = pacc->get_accValue(*pvarChild, &bstrVal);

        if (SUCCEEDED(hr) && bstrVal)
        {
#if defined(UNICODE) || defined (_UNICODE)
            UINT len = ::SysStringLen(bstrVal);
            _tcsncpy(lpszVal, bstrVal, len);
            lpszVal[len] = L'\0';
#else
            WideCharToMultiByte(CP_ACP, 0, bstrVal, -1, lpszVal, cchVal, NULL, NULL);
#endif
            SysFreeString(bstrVal);
        }
        return(lstrlen(lpszVal));
    }

    void GetObjectRect(IAccessible* pacc, RECT& rectArea)
    {
        VARIANT posVar;
        VariantInit(&posVar);
        posVar.vt = VT_I4;
        posVar.lVal = CHILDID_SELF;
        long pxleft = 0, pxtop = 0, pxwidth = 0, pxheight = 0;
        pacc->accLocation(&pxleft, &pxtop, &pxwidth, &pxheight, posVar);
        rectArea.left = pxleft; rectArea.top = pxtop;
        rectArea.right = pxwidth; rectArea.bottom = pxheight;
    }

    bool RectIsEmpty(const RECT& rect)
    {
        return rect.bottom == 0 && rect.left == 0 && rect.top == 0 && rect.right == 0;
    }

    bool IsSameRect(const RECT& rec1, const RECT& rec2)
    {
        return rec1.bottom == rec2.bottom && rec1.left == rec2.left
            && rec1.top == rec2.top && rec1.right == rec2.right;
    }

	HRESULT SelectItemAtPoint(POINT point)
	{
		VARIANT varItem;
		IAccessible* pAcc;
		HRESULT hr = AccessibleObjectFromPoint(point, &pAcc, &varItem);
		if ((hr == S_OK))
		{
			hr = pAcc->accSelect((SELFLAG_TAKEFOCUS | SELFLAG_TAKESELECTION), varItem);
			VariantClear(&varItem);
			pAcc->Release();
		}
		return hr;
	}

	void ShowAccObjAndChildren(IAccessible *paccObj)
	{
		VARIANT varGet;
		TCHAR szObjName[256] = {0}, szObjRole[256] = {0}, szObjClass[256] = {0};
		VariantInit(&varGet);
		//ͨ��get_accName�õ�Name
		GetObjectName(paccObj, &varGet, szObjName, sizeof(szObjName));
		//ͨ��get_accRole�õ�Role
		GetObjectRole(paccObj, &varGet, szObjRole, sizeof(szObjRole));
		//ͨ��WindowFromAccessibleObject��GetClassName�õ�Class
		GetObjectClass(paccObj, szObjClass, sizeof(szObjClass));
		std::locale loc("");
		std::wcout.imbue(loc);
		std::wcout<<L"Name: "<<szObjName<<L" Role: "<<szObjRole<<L" Class: "<<szObjClass<<std::endl;
		long lChildNum;
		paccObj->get_accChildCount(&lChildNum);
		std::cout<<"child No. "<<lChildNum<<std::endl;
		IEnumVARIANT* pEnum = NULL;
		HRESULT hrs = paccObj -> QueryInterface(IID_IEnumVARIANT, (PVOID*) & pEnum);
		if (pEnum != NULL)
		{
			pEnum->Reset();
		}

		IDispatch* pDisp = NULL;
		for (int idx = 1; idx <= lChildNum; ++idx)
		{

			IAccessible* pCAcc = NULL;
			unsigned long numFetched;
			VARIANT varChild;
			
			// ���֧��IEnumVARIANT�ӿڣ��õ���һ����ID
			// �Լ����Ӧ�� IDispatch �ӿ�
			if (pEnum)
			{
				hrs = pEnum -> Next(1, &varChild, &numFetched);	
			}
			else
			{
				//���һ�����ײ�֧��IEnumVARIANT�ӿڣ���ID�����������
				varChild.vt = VT_I4;
				varChild.lVal = idx;
			}

			// �ҵ�����ID��Ӧ�� IDispatch �ӿ�
			if (varChild.vt == VT_I4)
			{
				//ͨ����ID��ŵõ���Ӧ�� IDispatch �ӿ�
				pDisp = NULL;
				hrs = paccObj -> get_accChild(varChild, &pDisp);
			}
			else
			{
				//�����֧��IEnumVARIANT�ӿڿ���ֱ�ӵõ���IDispatch �ӿ�
				pDisp = varChild.pdispVal;
			}

			// ͨ�� IDispatch �ӿڵõ��ӵ� IAccessible �ӿ� pCAcc
			if (pDisp)
			{
				hrs = pDisp->QueryInterface(IID_IAccessible, (void**)&pCAcc);
				hrs = pDisp->Release();
			}
			if (pCAcc != NULL)
			{
				ShowAccObjAndChildren(pCAcc);
			}
		}

		if(pEnum)
			pEnum -> Release();

	}
	
	void PrintString(LPCTSTR str)
	{
#if defined(UNICODE) || defined (_UNICODE)
		std::locale loc("");
		std::wcout.imbue(loc);
		std::wcout<<str;
#else
		std::cout<<str;
#endif
	}

	void PrintBstr(BSTR str)
	{
		if (str == NULL)
		{
			return;
		}
		std::locale loc("");
		std::wcout.imbue(loc);
		std::wcout<<str;
	}

	HRESULT PrintName(IAccessible* pAcc, long childId)
	{
		if (pAcc == NULL)
		{
			return E_INVALIDARG;
		}
		BSTR bstrName;
		VARIANT varChild;
		varChild.vt = VT_I4;
		varChild.lVal = childId;
		HRESULT hr = pAcc->get_accName(varChild, &bstrName);
		printf("Name: " );
		if (hr == S_OK)
		{
			PrintBstr(bstrName);
			SysFreeString(bstrName);
		}
		return hr;
	}

	HRESULT PrintRole(IAccessible* pAcc, long childId)
	{
		DWORD roleId;
		if (pAcc == NULL)
		{
			return E_INVALIDARG;    
		}
		VARIANT varChild;
		varChild.vt = VT_I4;
		varChild.lVal = childId;
		VARIANT varResult;
		HRESULT hr = pAcc->get_accRole(varChild, &varResult);
		if ((hr == S_OK) && (varResult.vt == VT_I4))
		{
			roleId = varResult.lVal;
			UINT   roleLength;
			LPTSTR lpszRoleString;

			// Get the length of the string. 
			roleLength = GetRoleText(roleId, NULL, 0);

			// Allocate memory for the string. Add one character to 
			// the length you got in the previous call to make room 
			// for the null character. 
			lpszRoleString = (LPTSTR)malloc((roleLength+1) * sizeof(TCHAR));
			if (lpszRoleString != NULL)
			{
				// Get the string. 
				GetRoleText(roleId, lpszRoleString, roleLength + 1);
#ifdef UNICODE
				printf("Role: ");
				PrintBstr(lpszRoleString);
#else
				printf("Role: ");
				printf("%s", lpszRoleString);
#endif
				// Free the allocated memory 
				free(lpszRoleString);
			}
			else 
			{
				return E_OUTOFMEMORY;
			}
		}
		TCHAR szObjClass[MAX_PATH] = {0};
		GetObjectClass(pAcc, szObjClass, sizeof(szObjClass));
		printf(" Class:");
		PrintString(szObjClass);
		return S_OK;
	}

	long GetObjectChildNum(IDispatch* pDisp)
	{
		if (pDisp == NULL)
		{
			return 0;
		}
		VARIANT vResult;
		DISPPARAMS dispParams = { NULL, NULL, 0, 0 }; 

		VariantInit(&vResult);

		HRESULT hr = pDisp->Invoke( DISPID_ACC_CHILDCOUNT
			, IID_NULL, LOCALE_SYSTEM_DEFAULT
			, DISPATCH_PROPERTYGET
			, &dispParams // ����
			, &vResult // ����ֵ
			, NULL, NULL);
		if (hr == S_OK)
		{
			if (vResult.vt == VT_I4)
			{
				return vResult.lVal;
			}
		}
		return 0;
	}

	IDispatch * GetObjectChildIdx(IDispatch* pDisp, int idx)
	{
		VARIANT vResult;
		VariantInit(&vResult);
		VARIANTARG v[1];
		v[0].vt = VT_I4;
		v[0].lVal = idx;
		DISPPARAMS dispParams = { v, NULL, 1, 0 }; 


		HRESULT hr = pDisp->Invoke( DISPID_ACC_CHILD
			, IID_NULL, LOCALE_SYSTEM_DEFAULT
			, DISPATCH_PROPERTYGET
			, &dispParams // ����
			, &vResult // ����ֵ
			, NULL, NULL);
		if (hr == S_OK)
		{
			if (vResult.vt == VT_DISPATCH)
			{
				if (vResult.pdispVal == NULL)
				{
					return NULL;
				}
				return vResult.pdispVal;
			}
		}
		else if (hr == S_FALSE)
		{
			return NULL;
		}
		return NULL;
	}

	void PrintObjectName(IDispatch* pDisp)
	{
		if (pDisp == NULL)
		{
			return;
		}
		VARIANT varChild;
		VariantInit(&varChild);
		VARIANTARG v[1];
		v[0] = varChild;
		DISPPARAMS dispParams = { v, NULL, 1, 0 }; 

		VARIANT vResult;
		VariantInit(&vResult);
		vResult.vt = VT_BSTR;
		vResult.bstrVal = NULL;

		EXCEPINFO excepInfo;
		memset(&excepInfo, 0, sizeof excepInfo);
		
		UINT nArgErr = (UINT)-1;
		
		HRESULT hr = pDisp->Invoke( DISPID_ACC_NAME
			, IID_NULL, LOCALE_SYSTEM_DEFAULT
			, DISPATCH_PROPERTYGET
			, &dispParams // ����
			, &vResult // ����ֵ
			, &excepInfo
			, &nArgErr);
		printf("Name: ");
		if (hr == S_OK)
		{
			if (vResult.vt == VT_BSTR)
			{
				PrintBstr(vResult.bstrVal);
			}
		}
	}

	HRESULT PrintName(IDispatch* pDisp)
	{
		PrintObjectName(pDisp);
		return S_OK;
	}

	void PrintObjectRole(IDispatch* pDisp)
	{
		if (pDisp == NULL)
		{
			return;
		}
		VARIANT vResult;
		VARIANT varChild;
		VariantInit(&varChild);
		VARIANTARG v[1];
		v[0] = varChild;
		DISPPARAMS dispParams = { v, NULL, 1, 0 }; 

		VariantInit(&vResult);

		HRESULT hr = pDisp->Invoke( DISPID_ACC_ROLE
			, IID_NULL, LOCALE_SYSTEM_DEFAULT
			, DISPATCH_PROPERTYGET
			, &dispParams // ����
			, &vResult // ����ֵ
			, NULL, NULL);
		printf("Role: ");
		if (hr == S_OK)
		{
			if (vResult.vt == VT_I4)
			{
				//����GetRoleText���������͵Ľ�ɫ��ʾת��Ϊ�ַ�����ʾ
				TCHAR lpszRole[MAX_PATH] = {0};
				GetRoleText(vResult.lVal, lpszRole, sizeof(lpszRole));
				PrintString(lpszRole);
			}
		}
	}

	HRESULT PrintRole(IDispatch* pDisp)
	{
		PrintObjectRole(pDisp);
		return S_OK;
	}

	HRESULT WalkTreeWithAccessibleChildren(IDispatch* pDisp, int depth)
	{
		long childCount = GetObjectChildNum(pDisp);
		if (childCount == 0)
		{
			return S_FALSE;
		}
		for (int i = 1; i <= childCount; ++i)
		{
			IDispatch* dispChild = GetObjectChildIdx(pDisp, i);
			for (int y = 0; y < depth; y++)
			{
				printf("  ");
			}
			std::cout<<"Child Num: "<<i<<" Total: "<<childCount<<" ";
			PrintName(dispChild);
			printf("(Object) ");
			PrintRole(dispChild);
			printf("\n");
			WalkTreeWithAccessibleChildren(dispChild, depth + 1);
		}
		return S_OK;
	}

	HRESULT WalkTreeWithAccessibleChildren(IAccessible* pAcc, int depth)
	{
		HRESULT hr;
		long childCount;
		long returnCount;

		if (!pAcc)
		{
			return E_INVALIDARG;
		}
		hr = pAcc->get_accChildCount(&childCount);
		if (FAILED(hr))
		{
			return hr;
		};
		if (childCount == 0)
		{
			return S_FALSE;
		}
		VARIANT* pArray = new VARIANT[childCount];
		hr = AccessibleChildren(pAcc, 0L, childCount, pArray, &returnCount);
		if (FAILED(hr))
		{
			return hr;
		}
		if (childCount != returnCount)
		{
			return S_FALSE;
		}
		// Iterate through children.
		for (int x = 0; x < returnCount; x++)
		{
			VARIANT vtChild = pArray[x];
			// If it's an accessible object, get the IAccessible, and recurse.
			if (vtChild.vt == VT_DISPATCH)
			{
				IDispatch* pDisp = vtChild.pdispVal;
				IAccessible* pChild = NULL;
				hr = pDisp->QueryInterface(IID_IAccessible, (void**) &pChild);

 				if (hr == S_OK)
 				{
					for (int y = 0; y < depth; y++)
					{
						printf("  ");
					}
					std::cout<<"Child Num: "<<x + 1<<" Total: "<<returnCount<<" ";
					PrintName(pChild, CHILDID_SELF);
					printf("(Object) ");
					PrintRole(pChild, CHILDID_SELF);
					printf("\n");
					WalkTreeWithAccessibleChildren(pChild, depth + 1);
					pChild->Release();
				}
				pDisp->Release();
			}
			// Else it's a child element so we have to call accNavigate on the parent,
			//   and we do not recurse because child elements can't have children.
			else
			{
				for (int y = 0; y < depth; y++)
				{
					printf("  ");
				}
				std::cout<<"Child Num: "<<x + 1<<" Total: "<<returnCount<<" ";
				PrintName(pAcc, vtChild.lVal);
				printf("(Child element) ");
				PrintRole(pAcc, vtChild.lVal);
				printf("\n");
			}
		}
		delete[] pArray;
		return S_OK;
	}

	void ShowAccObjTree(HWND parent)
	{
		if (NULL == parent)
		{
			return;
		}

		//ͨ�����ھ���õ����ڵ� IAccessible �ӿ�ָ�롣
		IAccessible *paccMainWindow = NULL;
		HRESULT hr;

		if(S_OK == (hr = AccessibleObjectFromWindow(parent, 
			OBJID_WINDOW, 
			IID_IAccessible,
			(void**)&paccMainWindow)))
		{
			std::cout<<"Parent:  ";
			PrintName(paccMainWindow, CHILDID_SELF);
			printf(" ");
			PrintRole(paccMainWindow, CHILDID_SELF);
			printf("\n");

			WalkTreeWithAccessibleChildren(paccMainWindow, 1);
		}
	}

    bool AccObjCompare(IAccessible* pacc, VARIANT* pvarChild, WinAccObj& accObj)
    {
        WinAccObj tmpObj = accObj;
        TCHAR tName[MAX_PATH] = {0};
        GetObjectName(pacc, pvarChild, tName, MAX_PATH);
        if (_tcscmp(tmpObj.m_WndName,TEXT("")) == 0)
        {
            tmpObj.m_WndName = tName;
        }
        else if (_tcscmp(tmpObj.m_WndName, tName) != 0)
        {
            return false;
        }

        TCHAR tRole[MAX_PATH] = {0};
        GetObjectRole(pacc, pvarChild, tRole, MAX_PATH);
        if (_tcscmp(tmpObj.m_RoleName, TEXT("")) == 0)
        {
            tmpObj.m_RoleName = tRole;
        }
        else if ( _tcscmp(tmpObj.m_RoleName, tRole) != 0 )
        {
            return false;
        }

        TCHAR tClass[MAX_PATH] = {0};
        GetObjectClass(pacc, tClass, MAX_PATH);
        if (_tcscmp(tmpObj.m_ClsName, TEXT("")) == 0)
        {
            tmpObj.m_ClsName = tClass;
        }
        else if (_tcscmp(tmpObj.m_ClsName, tClass) != 0)
        {
            return false;
        }

        TCHAR tDesc[MAX_PATH] = {0};
        GetAccObjectDescription(pacc, pvarChild, tDesc, MAX_PATH);
        if (_tcscmp(tmpObj.m_Description, TEXT("")) == 0)
        {
            tmpObj.m_Description = tDesc;
        }
        else if (_tcscmp(tmpObj.m_Description, tDesc) != 0)
        {
            return false;
        }

        TCHAR tValue[MAX_PATH] = {0};
        GetObjectValue(pacc, pvarChild, tValue, MAX_PATH);
        if (_tcscmp(tmpObj.m_Value, TEXT("")) == 0)
        {
            memcpy(tmpObj.m_Value, tValue, MAX_PATH);
        }
        else if (_tcscmp(tmpObj.m_Description, tValue) != 0)
        {
            return false;
        }

        RECT rect;
        GetObjectRect(pacc, rect);
        if (RectIsEmpty(tmpObj.m_rect))
        {
            tmpObj.m_rect = rect;
        }
        else if (!IsSameRect(tmpObj.m_rect, rect))
        {
            return false;
        }

        accObj = tmpObj;
        return true;
    }

	bool FindChild (IAccessible* paccParent, 
		WinAccObj& accObj,
		IAccessible** paccChild, 
		VARIANT* pvarChild)
	{
		HRESULT hr;
		long numChildren;
		unsigned long numFetched;
		VARIANT varChild;
		int index;
		IAccessible* pCAcc = NULL;
		IEnumVARIANT* pEnum = NULL;
		IDispatch* pDisp = NULL;
		bool found = false;
		TCHAR szObjState[256] = {0};

		//�õ�����֧�ֵ�IEnumVARIANT�ӿ�
		hr = paccParent -> QueryInterface(IID_IEnumVARIANT, (PVOID*) & pEnum);

		if(pEnum)
			pEnum -> Reset(); //Resets the enumeration sequence to the beginning.

		//ȡ�ø���ӵ�еĿɷ��ʵ��ӵ���Ŀ
		paccParent -> get_accChildCount(&numChildren);

		//�������Ƚ�ÿһ����ID���ҵ����֡���ɫ������������һ�µġ�
		for(index = 1; index <= numChildren && !found; index++)
		{
			pCAcc = NULL;		
			// ���֧��IEnumVARIANT�ӿڣ��õ���һ����ID
			// �Լ����Ӧ�� IDispatch �ӿ�
			if (pEnum)
			{
				hr = pEnum -> Next(1, &varChild, &numFetched);	
			}
			else
			{
				//���һ�����ײ�֧��IEnumVARIANT�ӿڣ���ID�����������
				varChild.vt = VT_I4;
				varChild.lVal = index;
			}

			// �ҵ�����ID��Ӧ�� IDispatch �ӿ�
			if (varChild.vt == VT_I4)
			{
				//ͨ����ID��ŵõ���Ӧ�� IDispatch �ӿ�
				pDisp = NULL;
				hr = paccParent -> get_accChild(varChild, &pDisp);
			}
			else
			{
				//�����֧��IEnumVARIANT�ӿڿ���ֱ�ӵõ���IDispatch �ӿ�
				pDisp = varChild.pdispVal;
			}

			// ͨ�� IDispatch �ӿڵõ��ӵ� IAccessible �ӿ� pCAcc
			if (pDisp)
			{
				hr = pDisp->QueryInterface(IID_IAccessible, (void**)&pCAcc);
				hr = pDisp->Release();
			}

			long numChilds = 0;
			// Get information about the child
			if(pCAcc)
			{
				//�����֧��IAccessible �ӿڣ���ô��ID����CHILDID_SELF
				VariantInit(&varChild);
				varChild.vt = VT_I4;
				varChild.lVal = CHILDID_SELF;

				*paccChild = pCAcc;
				pCAcc-> get_accChildCount(&numChilds);
			}
			else
			{
				//����Ӳ�֧��IAccessible �ӿ�
				*paccChild = paccParent;
				numChilds = 0;
//			continue;
			}

			//�������в��ɷ���״̬��Ԫ��
			GetObjectState(*paccChild, 
				&varChild, 
				(LPTSTR)szObjState, 
				sizeof(szObjState));
			if(NULL != _tcsstr(szObjState, TEXT("unavailable")))
			{
				if(pCAcc)
					pCAcc->Release();
				continue;
			}

            if (AccObjCompare(*paccChild, &varChild, accObj))
			{
				found = true;
				*pvarChild = varChild;
				break;
			}
			if(!found && pCAcc)
			{
				// ����εõ����ӽӿ�Ϊ���ݹ����
				found = FindChild(pCAcc, 
					accObj, 
					paccChild, 
					pvarChild);
				if(*paccChild != pCAcc)
					pCAcc->Release();
			}
		}//End for

		// Clean up
		if(pEnum)
			pEnum -> Release();

		return found;
	}

	bool FindAccObj(HWND parent, WinAccObj& accObj)
	{
		if (NULL == parent)
		{
			return false;
		}

        IAccessible *paccMainWindow = NULL;
		HRESULT hr;
		bool isFind = false;
		if(S_OK == (hr = AccessibleObjectFromWindow(parent, 
			OBJID_WINDOW, 
			IID_IAccessible,
			(void**)&paccMainWindow)))
		{
			IAccessible *paccObj = NULL;
			VARIANT var;
			isFind = FindChild(paccMainWindow, accObj, &paccObj, &var);
			if (isFind)
			{
				paccMainWindow->Release();
			}
		}
        return isFind;
    }
}

// void AccObjectData::GetBrowserMainWnd()
// {
//     std::vector<HWND> wnds;
// 
//     WindowAccessHelper::FindMainWnds(wnds, m_MainClsName);
// 
//     if (wnds.empty())
//     {
//         m_hMainWnd = NULL;
//         return;
//     }
// #ifdef ADDRESS_TEST
//     m_hMainWnd = *(wnds.begin());
// #else
//     //	m_hMainWnd = *(wnds.begin());
//     m_hMainWnd = WindowAccessHelper::GetHwndToModify(wnds);
// #endif
//     // ���ڱ�������ǰ������ΪNULL����չ����
// }
// 
// void AccObjectData::GetAddressHwnd()
// {
//     if (NULL == m_hMainWnd)
//     {
//         GetBrowserMainWnd();
//     }
//     if ( _tcscmp(m_MainClsName, m_ClsName) == 0)
//     {
//         m_hAddressWnd = m_hMainWnd;
//         return;
//     }
//     std::vector<HWND> hControls;
//     WindowAccessHelper::FindSubWindows(m_hMainWnd, hControls, m_ClsName, m_WndName);
//     if (hControls.empty())
//     {
//         m_hAddressWnd = NULL;
//         return;
//     }
//     m_hAddressWnd = *(hControls.begin());
// }
// 
// void AccObjectData::GetAddressCenter()
// {
//     if (NULL == m_hAddressWnd)
//     {
//         GetAddressHwnd();
//     }
// 
//     if (NULL == m_hAddressWnd)
//     {
//         return;
//     }
// 
//     if (m_hAddressWnd == m_hMainWnd)
//     {
//         WinAccObj winObj(m_WndName, m_RoleName, m_ClsName);
//         WindowAccessHelper::FindAccObj( m_hAddressWnd, winObj);
//         m_ptCenterAddress.x = winObj.m_rect.left + winObj.m_rect.right / 2;
//         m_ptCenterAddress.y = winObj.m_rect.top + winObj.m_rect.bottom / 2;
//     }
//     else
//     {
//         GetWindowRect(m_hAddressWnd, &rect);
//         m_ptCenterAddress.x = rect.left / 2 + rect.right / 2;
//         m_ptCenterAddress.y = rect.top / 2 + rect.bottom / 2;
//     }
// }
