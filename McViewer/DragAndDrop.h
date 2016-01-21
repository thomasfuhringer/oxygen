/*
  Copyright (C) 2008 MySQL AB

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  There are special exceptions to the terms and conditions of the GPL
  as it is applied to this software. View the full text of the exception
  in file LICENSE.exceptions in the top-level directory of this software
  distribution.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  */
/*
 * File: DragAndDrop.h
 * A layer for implemention Drag and Drop in Windows for C-applications. See DragAndDrop.c for more comments.
 */
#ifndef DRAGANDDROP_H
#define DRAGANDDROP_H

/* Public typedefs. */
typedef struct tagMYDROPDATA
{
	CLIPFORMAT cf;
	POINTL pt;
	DWORD dwKeyState;
	HGLOBAL hData;
} MYDROPDATA, *PMYDROPDATA;

typedef struct tagMYDROPSOURCE *PMYDROPSOURCE;
typedef struct tagMYDROPTARGET *PMYDROPTARGET;
typedef DWORD(*MYDDCALLBACK)(CLIPFORMAT cf, HGLOBAL hData, HWND hWnd, DWORD dwKeyState, POINTL pt,
	void *pUserData);

/* Public function prototypes. */
void MyDragDropInit(HANDLE hHeap);
PMYDROPSOURCE CreateMyDropSource(BOOL bRightClick, CLIPFORMAT *pFormat, HGLOBAL *phData, ULONG lFmt);
PMYDROPSOURCE CreateMyDropSourceText(BOOL bRightClick, LPCTSTR pText);
DWORD MyDragDropSourceEx(PMYDROPSOURCE pDropSrc, DWORD dwOKEffect, DWORD *pdwEffect);
DWORD MyDragDropSource(PMYDROPSOURCE pDropSrc);
DWORD MyDragDropText(LPCTSTR pText);
PMYDROPSOURCE FreeMyDropSource(PMYDROPSOURCE pDropSrc);
PMYDROPTARGET MyRegisterDragDrop(HWND hWnd, CLIPFORMAT *pFormat, ULONG lFmt, UINT nMsg, MYDDCALLBACK, void *pUserData);
PMYDROPTARGET MyRevokeDragDrop(PMYDROPTARGET pTarget);
IDataObject *CreateDataObject(CLIPFORMAT *pFormat, HGLOBAL *phData, ULONG lFmt);
IDropSource *CreateDropSource(BOOL bRightClick);
IDropTarget *CreateDropTarget(CLIPFORMAT *pFormat, ULONG lFmt, HWND hWnd, UINT nMsg, MYDDCALLBACK pDropProc, void *pUserData);
#endif