// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


#include "stdafx.h"
#include "wmpcdrom.h"

// Dispatch interfaces referenced by this interface
#include "wmpplaylist.h"


/////////////////////////////////////////////////////////////////////////////
// CWMPCdrom properties

/////////////////////////////////////////////////////////////////////////////
// CWMPCdrom operations

CString CWMPCdrom::GetDriveSpecifier()
{
    CString result;
    InvokeHelper(0xfb, DISPATCH_PROPERTYGET, VT_BSTR, (void*)&result, NULL);
    return result;
}

CWMPPlaylist CWMPCdrom::GetPlaylist()
{
    LPDISPATCH pDispatch;
    InvokeHelper(0xfc, DISPATCH_PROPERTYGET, VT_DISPATCH, (void*)&pDispatch, NULL);
    return CWMPPlaylist(pDispatch);
}

void CWMPCdrom::eject()
{
    InvokeHelper(0xfd, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}