/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * The contents of this file are subject to the Netscape Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation. All
 * Rights Reserved.
 *
 * Contributor(s): 
 *   David Epstein <depstein@netscape.com> 
 */

// File Overview....
//
// These are QA test case implementations
//

#include "stdafx.h"
#include "TestEmbed.h"
#include "BrowserImpl.h"
#include "BrowserFrm.h"
#include "UrlDialog.h"
#include "ProfileMgr.h"
#include "ProfilesDlg.h"
#include "Tests.h"
#include "QaUtils.h"
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Register message for FindDialog communication
static UINT WM_FINDMSG = ::RegisterWindowMessage(FINDMSGSTRING);

BEGIN_MESSAGE_MAP(CTests, CWnd)
	//{{AFX_MSG_MAP(CTests)

	ON_COMMAND(ID_TESTS_CHANGEURL, OnTestsChangeUrl)
	ON_COMMAND(ID_TESTS_GLOBALHISTORY, OnTestsGlobalHistory)
	ON_COMMAND(ID_TESTS_CREATEFILE, OnTestsCreateFile)
	ON_COMMAND(ID_TESTS_CREATEPROFILE, OnTestsCreateprofile)
	ON_COMMAND(ID_TESTS_ADDWEBPROGLISTENER, OnTestsAddWebProgListener)
	ON_COMMAND(ID_TESTS_ADDHISTORYLISTENER, OnTestsAddHistoryListener)
	ON_COMMAND(ID_INTERFACES_NSIFILE, OnInterfacesNsifile)
	ON_COMMAND(ID_INTERFACES_NSISHISTORY, OnInterfacesNsishistory)
	ON_COMMAND(ID_INTERFACES_NSIWEBNAV, OnInterfacesNsiwebnav)
	ON_COMMAND(ID_TOOLS_REMOVEGHPAGE, OnToolsRemoveGHPage)
	ON_COMMAND(ID_TOOLS_REMOVEALLGH, OnToolsRemoveAllGH)

	// DHARMA
    ON_COMMAND(ID_CLIPBOARDCMD_PASTE, OnPasteTest)
    ON_COMMAND(ID_CLIPBOARDCMD_COPYSELECTION, OnCopyTest)
    ON_COMMAND(ID_CLIPBOARDCMD_SELECTALL, OnSelectAllTest)
    ON_COMMAND(ID_CLIPBOARDCMD_SELECTNONE, OnSelectNoneTest)
    ON_COMMAND(ID_CLIPBOARDCMD_CUTSELECTION, OnCutSelectionTest)
    ON_COMMAND(ID_CLIPBOARDCMD_COPYLINKLOCATION, copyLinkLocationTest)
    ON_COMMAND(ID_CLIPBOARDCMD_CANCOPYSELECTION, canCopySelectionTest)
    ON_COMMAND(ID_CLIPBOARDCMD_CANCUTSELECTION, canCutSelectionTest)
    ON_COMMAND(ID_CLIPBOARDCMD_CANPASTE, canPasteTest)
	// DHARMA

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CTests::CTests(nsIWebBrowser* mWebBrowser,
			   nsIBaseWindow* mBaseWindow,
			   nsIWebNavigation* mWebNav,
			   CBrowserImpl *mpBrowserImpl)
{
	qaWebBrowser = mWebBrowser;
	qaBaseWindow = mBaseWindow;
	qaWebNav = mWebNav;

	qaBrowserImpl = mpBrowserImpl;
}

CTests::~CTests()
{
}


// de: Start QA test cases here
// *********************************************************
// *********************************************************

void CTests::OnTestsChangeUrl() 
{
	char *theUrl = "http://www.aol.com/";
	CUrlDialog myDialog;
	nsresult rv;

	if (!qaWebNav)
	{
		CQaUtils::QAOutput("Web navigation object not found. Change URL test not performed.", 2);
		return;
	}

	if (myDialog.DoModal() == IDOK)
	{
		CQaUtils::QAOutput("Begin Change URL test.", 1);
		strcpy(theUrl, myDialog.m_urlfield);
		rv = qaWebNav->LoadURI(NS_ConvertASCIItoUCS2(theUrl).GetUnicode(), 
						nsIWebNavigation::LOAD_FLAGS_BYPASS_HISTORY);
	    CQaUtils::RvTestResult(rv, "rv LoadURI() test", 1);
		CQaUtils::FormatAndPrintOutput("The url = ", theUrl, 2); 

/*
		char *uriSpec;
		nsCOMPtr<nsIURI> pURI;
		// GetcurrentURI() declared in nsIP3PUI.idl
		// used with webNav obj in nsP3PObserverHTML.cpp, line 239
		// this will be used as an indep routine to verify the URI load
		rv = qaWebNav->GetCurrentURI( getter_AddRefs( pURI ) );
		if(NS_FAILED(rv) || !pURI)
			AfxMessageBox("Bad result for GetCurrentURI().");

		rv = pURI->GetSpec(&uriSpec);
		if (NS_FAILED(rv))
			AfxMessageBox("Bad result for GetSpec().");

		AfxMessageBox("Start URL validation test().");
		if (strcmp(uriSpec, theUrl) == 0)
		{
			CQaUtils::QAOutput("Url loaded successfully. Test Passed.", 2);	
		}
		else
		{
			CQaUtils::QAOutput("Url didn't load successfully. Test Failed.", 2);
		}
*/
		CQaUtils::QAOutput("End Change URL test.", 1);
	}
	else
		CQaUtils::QAOutput("Change URL test not executed.", 1);

}

// *********************************************************

void CTests::OnTestsGlobalHistory() 
{
	// create instance of myHistory object. Call's XPCOM
	// service manager to pass the contract ID.

	char *theUrl = "http://www.bogussite.com/";
	CUrlDialog myDialog;

	PRBool theRetVal = PR_FALSE;
	nsresult rv;

	nsCOMPtr<nsIGlobalHistory> myHistory(do_GetService(NS_GLOBALHISTORY_CONTRACTID));

	if (!myHistory)
	{
		CQaUtils::QAOutput("Couldn't find history object. No GH tests performed.", 2);
		return;
	}

	if (myDialog.DoModal() == IDOK)
	{
		CQaUtils::QAOutput("Begin IsVisited() and AddPage() tests.", 2);

		strcpy(theUrl, myDialog.m_urlfield);

		CQaUtils::FormatAndPrintOutput("The history url = ", theUrl, 1);

		// see if url is already in the GH file (pre-AddPage() test)
		rv = myHistory->IsVisited(theUrl, &theRetVal);
	    CQaUtils::RvTestResult(rv, "rv IsVisited() test", 1);

		strMsg.Format("The IsVisited() return value = %d", theRetVal); 
		AfxMessageBox(strMsg);

		if (theRetVal)
			CQaUtils::QAOutput("URL has been visited. Won't execute AddPage().", 2);
		else
		{
			CQaUtils::QAOutput("URL hasn't been visited. Will execute AddPage().", 2);

			// adds a url to the global history file
			rv = myHistory->AddPage(theUrl);

			// prints addPage() results to output file
			if (NS_FAILED(rv))
			{
				CQaUtils::QAOutput("Invalid results for AddPage(). Url not added. Test failed.", 1);
				return;
			}
			else
				CQaUtils::QAOutput("Valid results for AddPage(). Url added. Test passed.", 1);

			// checks if url was visited (post-AddPage() test)
 			myHistory->IsVisited(theUrl, &theRetVal);

			if (theRetVal)
				CQaUtils::QAOutput("URL is visited; post-AddPage() test. IsVisited() test passed.", 1);
			else
				CQaUtils::QAOutput("URL isn't visited; post-AddPage() test. IsVisited() test failed.", 1);
		}
		CQaUtils::QAOutput("End IsVisited() and AddPage() tests.", 2);
	}
	else
		CQaUtils::QAOutput("IsVisited() and AddPage() tests not executed.", 1);
}



// *********************************************************

void CTests::OnTestsCreateFile() 
{
   nsresult rv;
   PRBool exists;
   nsCOMPtr<nsILocalFile> theTestFile(do_GetService(NS_LOCAL_FILE_CONTRACTID));

    if (!theTestFile)
	{
		CQaUtils::QAOutput("File object doesn't exist. No File tests performed.", 2);
		return;
	}


	CQaUtils::QAOutput("Start Create File test.", 2);

	rv = theTestFile->InitWithPath("c:\\temp\\theFile.txt");
	rv = theTestFile->Exists(&exists);

	CQaUtils::QAOutput("File (theFile.txt) doesn't exist. We'll create it.\r\n", 1);
	rv = theTestFile->Create(nsIFile::NORMAL_FILE_TYPE, 0777);
	CQaUtils::RvTestResult(rv, "File Create() test", 2);
}

// *********************************************************

void CTests::OnTestsCreateprofile() 
{
    CProfilesDlg    myDialog;
    nsresult        rv;

	if (myDialog.DoModal() == IDOK)
    {       
//      NS_WITH_SERVICE(nsIProfile, profileService, NS_PROFILE_CONTRACTID, &rv);
		nsCOMPtr<nsIProfile> theProfServ(do_GetService(NS_PROFILE_CONTRACTID));
		if (!theProfServ)
		{
		   CQaUtils::QAOutput("Didn't get profile service. No profile tests performed.", 2);
		   return;
		}

	   CQaUtils::QAOutput("Start Profile switch test.", 2);

	   CQaUtils::QAOutput("Retrieved profile service.", 2);
       rv = theProfServ->SetCurrentProfile(myDialog.m_SelectedProfile.GetUnicode());
	   CQaUtils::RvTestResult(rv, "SetCurrentProfile() (profile switching) test", 2);

	   CQaUtils::QAOutput("End Profile switch test.", 2);
    }
	else
	   CQaUtils::QAOutput("Profile switch test not executed.", 2);
	
}

// *********************************************************

void CTests::OnTestsAddWebProgListener()
{
    nsWeakPtr weakling(
        dont_AddRef(NS_GetWeakReference(NS_STATIC_CAST(nsIWebProgressListener*, qaBrowserImpl))));
    rv = qaWebBrowser->AddWebBrowserListener(weakling, NS_GET_IID(nsIWebProgressListener));
	
	CQaUtils::RvTestResult(rv, "AddWebBrowserListener(). Add Web Prog Lstnr test", 2);
}

// *********************************************************

void CTests::OnTestsAddHistoryListener()
{
   // addSHistoryListener test
	nsWeakPtr weakling(
        dont_AddRef(NS_GetWeakReference(NS_STATIC_CAST(nsISHistoryListener*, qaBrowserImpl))));
	rv = qaWebBrowser->AddWebBrowserListener(weakling, NS_GET_IID(nsISHistoryListener));
	CQaUtils::RvTestResult(rv, "AddWebBrowserListener(). Add History Lstnr test", 2);
}

// *********************************************************
// *********************************************************
//					TOOLS to help us


void CTests::OnToolsRemoveGHPage() 
{
	char *theUrl = "http://www.bogussite.com/";
	CUrlDialog myDialog;
	PRBool theRetVal = PR_FALSE;
	nsresult rv;
	nsCOMPtr<nsIGlobalHistory> myGHistory(do_GetService(NS_GLOBALHISTORY_CONTRACTID));
	if (!myGHistory)
	{
		CQaUtils::QAOutput("Could not get the global history object.", 2);
		return;
	}
	nsCOMPtr<nsIBrowserHistory> myHistory = do_QueryInterface(myGHistory, &rv);
	if(!NS_SUCCEEDED(rv)) {
		CQaUtils::QAOutput("Could not get the history object.", 2);
		return;
	}
//	nsCOMPtr<nsIBrowserHistory> myHistory(do_GetInterface(myGHistory));


	if (myDialog.DoModal() == IDOK)
	{
		CQaUtils::QAOutput("Begin URL removal from the GH file.", 2);
		strcpy(theUrl, myDialog.m_urlfield);

		myGHistory->IsVisited(theUrl, &theRetVal);
		if (theRetVal)
		{
			rv = myHistory->RemovePage(theUrl);
			CQaUtils::RvTestResult(rv, "RemovePage() test (url removal from GH file)", 2);
		}
		else
		{
			CQaUtils::QAOutput("The URL wasn't in the GH file.\r\n", 1);
		}
		CQaUtils::QAOutput("End URL removal from the GH file.", 2);
	}
	else
		CQaUtils::QAOutput("URL removal from the GH file not executed.", 2);
}

void CTests::OnToolsRemoveAllGH()
{

	nsresult rv;
	nsCOMPtr<nsIGlobalHistory> myGHistory(do_GetService(NS_GLOBALHISTORY_CONTRACTID));
	if (!myGHistory)
	{
		CQaUtils::QAOutput("Could not get the global history object.", 2);
		return;
	}
	nsCOMPtr<nsIBrowserHistory> myHistory = do_QueryInterface(myGHistory, &rv);
	if(!NS_SUCCEEDED(rv)) {
		CQaUtils::QAOutput("Could not get the history object.", 2);
		return;
	}

	CQaUtils::QAOutput("Begin removal of all pages from the GH file.", 2);

	rv = myHistory->RemoveAllPages();
	CQaUtils::RvTestResult(rv, "removeAllPages(). Test .", 2);
	
	CQaUtils::QAOutput("End removal of all pages from the GH file.", 2);

	// removeAllPages()

}

// ***********************************************************************
// ************************** Interface Tests ****************************
// ***********************************************************************

// nsIFile:

void CTests::OnInterfacesNsifile() 
{
   nsCOMPtr<nsILocalFile> theTestFile(do_CreateInstance(NS_LOCAL_FILE_CONTRACTID));
   nsCOMPtr<nsILocalFile> theFileOpDir(do_CreateInstance(NS_LOCAL_FILE_CONTRACTID));

    if (!theTestFile)
 	{
		CQaUtils::QAOutput("File object doesn't exist. No File tests performed.", 2);
		return;
	}
	if (!theFileOpDir)
 	{
		CQaUtils::QAOutput("File object doesn't exist. No File tests performed.", 2);
		return;
	}

	CQaUtils::QAOutput("Begin nsIFile tests.", 2);

	InitWithPathTest(theTestFile);
	AppendRelativePathTest(theTestFile);
	FileCreateTest(theTestFile);
	FileExistsTest(theTestFile);

	// FILE COPY test

	FileCopyTest(theTestFile, theFileOpDir);	

	// FILE MOVE test

	FileMoveTest(theTestFile, theFileOpDir);	

	CQaUtils::QAOutput("End nsIFile tests.", 2);	
}

// ***********************************************************************
// Individual nsIFile tests

void CTests::InitWithPathTest(nsILocalFile *theTestFile)
{
	rv = theTestFile->InitWithPath("c:\\temp\\");
	CQaUtils::RvTestResult(rv, "InitWithPath() test (initializing file path)", 2);
}

void CTests::AppendRelativePathTest(nsILocalFile *theTestFile)
{
	rv = theTestFile->AppendRelativePath("myFile.txt");
	CQaUtils::RvTestResult(rv, "AppendRelativePath() test (append file to the path)", 2);
}

void CTests::FileCreateTest(nsILocalFile *theTestFile)
{
	rv = theTestFile->Exists(&exists);
	if (!exists)
	{
		CQaUtils::QAOutput("File doesn't exist. We'll try creating it.", 2);
		rv = theTestFile->Create(nsIFile::NORMAL_FILE_TYPE, 0777);
		CQaUtils::RvTestResult(rv, " File Create() test ('myFile.txt')", 2);
	}
	else
		CQaUtils::QAOutput("File already exists (myFile.txt). We won't create it.", 2);
}

void CTests::FileExistsTest(nsILocalFile *theTestFile)
{
	rv = theTestFile->Exists(&exists);
	if (!exists)
		CQaUtils::QAOutput("Exists() test Failed. File (myFile.txt) doesn't exist.", 2);
	else
		CQaUtils::QAOutput("Exists() test Passed. File (myFile.txt) exists.", 2);

}

void CTests::FileCopyTest(nsILocalFile *theTestFile, nsILocalFile *theFileOpDir)
{
	CQaUtils::QAOutput("Start File Copy test.", 2);

	rv = theFileOpDir->InitWithPath("c:\\temp\\");
	if (NS_FAILED(rv))
		CQaUtils::QAOutput("The target dir wasn't found.", 2);
	else
		CQaUtils::QAOutput("The target dir was found.", 2);

	rv = theTestFile->InitWithPath("c:\\temp\\myFile.txt");
	if (NS_FAILED(rv))
		CQaUtils::QAOutput("The path wasn't found.", 2);
	else
		CQaUtils::QAOutput("The path was found.", 2);

	rv = theTestFile->CopyTo(theFileOpDir, "myFile2.txt");
	CQaUtils::RvTestResult(rv, "rv CopyTo() test", 2);

	rv = theTestFile->InitWithPath("c:\\temp\\myFile2.txt");
	rv = theTestFile->Exists(&exists);
	if (!exists)
		CQaUtils::QAOutput("File didn't copy. CopyTo() test Failed.", 2);
	else
		CQaUtils::QAOutput("File copied. CopyTo() test Passed.", 2);
}

void CTests::FileMoveTest(nsILocalFile *theTestFile, nsILocalFile *theFileOpDir)
{
	CQaUtils::QAOutput("Start File Move test.", 2);

	rv = theFileOpDir->InitWithPath("c:\\Program Files\\");
	if (NS_FAILED(rv))
		CQaUtils::QAOutput("The target dir wasn't found.", 2);

	rv = theTestFile->InitWithPath("c:\\temp\\myFile2.txt");
	if (NS_FAILED(rv))
		CQaUtils::QAOutput("The path wasn't found.", 2);

	rv = theTestFile->MoveTo(theFileOpDir, "myFile2.txt");
	CQaUtils::RvTestResult(rv, "MoveTo() test", 2);

	rv = theTestFile->InitWithPath("c:\\Program Files\\myFile2.txt");
	rv = theTestFile->Exists(&exists);
	if (!exists)
		CQaUtils::QAOutput("File wasn't moved. MoveTo() test Failed.", 2);
	else
		CQaUtils::QAOutput("File was moved. MoveTo() test Passed.", 2);
}

// ***********************************************************************
// ***********************************************************************
// nsISHistory & nsIHistoryEntry ifaces:

void CTests::OnInterfacesNsishistory() 
{
   nsresult rv;
   PRInt32 numEntries = 5;
   PRInt32 theIndex;
   PRInt32 theMaxLength = 100;

   CString shString;

   nsCOMPtr<nsISHistory> theSessionHistory;
   nsCOMPtr<nsIHistoryEntry> theHistoryEntry;
   nsCOMPtr<nsIURI> theUri;
   // do_QueryInterface
   // NS_HISTORYENTRY_CONTRACTID
   // NS_SHISTORYLISTENER_CONTRACTID

	// get Session History through web nav iface
   if (qaWebNav)
		qaWebNav->GetSessionHistory( getter_AddRefs(theSessionHistory));

   if (!theSessionHistory)
   {
	   CQaUtils::QAOutput("theSessionHistory object wasn't created. No session history tests performed.", 2);
	   return;
   }
   else
	   CQaUtils::QAOutput("theSessionHistory object was created.", 2);


		// test count attribute in nsISHistory.idl
   GetCountTest(theSessionHistory, &numEntries);

		// test index attribute in nsISHistory.idl
   GetIndexTest(theSessionHistory, &theIndex);

		// test maxLength attribute in nsISHistory.idl
   GetMaxLength(theSessionHistory, theMaxLength);
   SetMaxLength(theSessionHistory, &theMaxLength);

	CQaUtils::QAOutput("Start nsiHistoryEntry tests.", 2);

	strMsg.Format("numEntries = %d", numEntries); 
	AfxMessageBox(strMsg); 

		// get theHistoryEntry object
	theSessionHistory->GetEntryAtIndex(0, PR_FALSE, getter_AddRefs(theHistoryEntry));
	if (!theHistoryEntry)
		CQaUtils::QAOutput("We didn't get the History Entry object.", 1);
	else 
	{
		CQaUtils::QAOutput("We have the History Entry object!", 1);	

			    // getEntryAtIndex() tests
		for (theIndex = 0; theIndex < numEntries; theIndex++)
		{
			strMsg.Format("the index = %d", theIndex); 
			AfxMessageBox(strMsg); 

//			GetEntryAtIndexTest(theSessionHistory, theHistoryEntry, theIndex);
			theSessionHistory->GetEntryAtIndex(theIndex, PR_FALSE, getter_AddRefs(theHistoryEntry));
			// nsiHistoryEntry.idl tests	

			// test URI attribute in nsIHistoryEntry.idl
			GetURIHistTest(theHistoryEntry);

			// test title attribute in nsIHistoryEntry.idl
			GetTitleHistTest(theHistoryEntry);

			// test isSubFrame attribute in nsIHistoryEntry.idl
			GetIsSubFrameTest(theHistoryEntry);

		}	// end for loop
	}		// end outer else


	// test SHistoryEnumerator attribute in nsISHistory.idl
	nsCOMPtr<nsISimpleEnumerator> theSimpleEnum;

//	GetSHEnumTest(theSessionHistory, theSimpleEnum);

	rv = theSessionHistory->GetSHistoryEnumerator(getter_AddRefs(theSimpleEnum));
	if (!theSimpleEnum)
  	   CQaUtils::QAOutput("theSimpleEnum for GetSHistoryEnumerator() invalid. Test failed.", 1);
	else
	   CQaUtils::RvTestResult(rv, "GetSHistoryEnumerator() (SHistoryEnumerator attribute) test", 2);

	SimpleEnumTest(theSimpleEnum);

	// PurgeHistory() test

	PurgeHistoryTest(theSessionHistory, numEntries);
}

// ***********************************************************************
// Individual nsISHistory tests

void CTests::GetCountTest(nsISHistory *theSessionHistory, PRInt32 *numEntries)
{
    rv = theSessionHistory->GetCount(numEntries);
	if (!(*numEntries))
		CQaUtils::QAOutput("numEntries for GetCount() invalid. Test failed.", 1);
	else
		CQaUtils::RvTestResult(rv, "GetCount() (count attribute) test", 2);
}

void CTests::GetIndexTest(nsISHistory *theSessionHistory, PRInt32 *theIndex)
{
	rv = theSessionHistory->GetIndex(theIndex);
	if (!(*theIndex))
		CQaUtils::QAOutput("theIndex for GetIndex() invalid. Test failed.", 1);
	else
		CQaUtils::RvTestResult(rv, "GetIndex() (index attribute) test", 2);
}

void CTests::GetMaxLength(nsISHistory *theSessionHistory, PRInt32 theMaxLength)
{
	rv = theSessionHistory->SetMaxLength(theMaxLength);
	CQaUtils::RvTestResult(rv, "SetMaxLength() (MaxLength attribute) test", 2);
}

void CTests::SetMaxLength(nsISHistory *theSessionHistory, PRInt32 *theMaxLength)
{
	rv = theSessionHistory->GetMaxLength(theMaxLength);
	if (!(*theMaxLength))
		CQaUtils::QAOutput("theMaxLength for GetMaxLength() invalid. Test failed.", 1);
	else
		CQaUtils::RvTestResult(rv, "GetMaxLength() (MaxLength attribute) test", 2);
}

/*
void CTests::GetEntryAtIndexTest(nsISHistory *theSessionHistory, 
								 nsIHistoryEntry *theHistoryEntry, 
								 PRInt32 theIndex)
{
	theSessionHistory->GetEntryAtIndex(theIndex, PR_FALSE, getter_AddRefs(theHistoryEntry));
}
*/

void CTests::GetURIHistTest(nsIHistoryEntry* theHistoryEntry)
{
	rv = theHistoryEntry->GetURI(getter_AddRefs(theUri));
	if (!theUri)
		CQaUtils::QAOutput("theUri for GetURI() invalid. Test failed.", 1);
	else
	{
		CQaUtils::RvTestResult(rv, "GetURI() (URI attribute) test", 1);
		rv = theUri->GetSpec(&uriSpec);
		if (NS_FAILED(rv))
			CQaUtils::QAOutput("We didn't get the uriSpec.", 1);
		else
			CQaUtils::FormatAndPrintOutput("The SH Url = ", uriSpec, 2);
	}
}

void CTests::GetTitleHistTest(nsIHistoryEntry* theHistoryEntry)
{
   nsXPIDLString theTitle;
   const char *  titleCString;

	rv = theHistoryEntry->GetTitle(getter_Copies(theTitle));
	if (!theTitle)
		CQaUtils::QAOutput("theTitle for GetTitle() invalid. Test failed.", 1);
	else
		CQaUtils::RvTestResult(rv, "GetTitle() (title attribute) test", 1);
	titleCString = NS_ConvertUCS2toUTF8(theTitle).get();
	CQaUtils::FormatAndPrintOutput("The title = ", (char *)titleCString, 2);

}

void CTests::GetIsSubFrameTest(nsIHistoryEntry* theHistoryEntry)
{
	PRBool isSubFrame;

	rv = theHistoryEntry->GetIsSubFrame(&isSubFrame);
	if (!isSubFrame)
		CQaUtils::QAOutput("isSubFrame for GetIsSubFrame() invalid. Test failed.", 1);
	else
		CQaUtils::RvTestResult(rv, "GetIsSubFrame() (isSubFrame attribute) test", 1);
	strMsg.Format("The subFrame value = %d", isSubFrame); 
	AfxMessageBox(strMsg);
}

/*
void CTests::GetSHEnumTest(nsISHistory *theSessionHistory,
						   nsISimpleEnumerator *theSimpleEnum)
{
	rv = theSessionHistory->GetSHistoryEnumerator(getter_AddRefs(theSimpleEnum));
	if (!theSimpleEnum)
  	   CQaUtils::QAOutput("theSimpleEnum for GetSHistoryEnumerator() invalid. Test failed.", 1);
	else
	   CQaUtils::RvTestResult(rv, "GetSHistoryEnumerator() (SHistoryEnumerator attribute) test", 2);
}
*/

void CTests::SimpleEnumTest(nsISimpleEnumerator *theSimpleEnum)
{
  PRBool bMore = PR_FALSE;
  nsCOMPtr<nsISupports> nextObj;
  nsCOMPtr<nsIHistoryEntry> nextHistoryEntry;

  while (NS_SUCCEEDED(theSimpleEnum->HasMoreElements(&bMore)) && bMore)
  {
	 theSimpleEnum->GetNext(getter_AddRefs(nextObj));
	 if (!nextObj)
		continue;
	 nextHistoryEntry = do_QueryInterface(nextObj);
	 if (!nextHistoryEntry)
		continue;
	 rv = nextHistoryEntry->GetURI(getter_AddRefs(theUri));
	 rv = theUri->GetSpec(&uriSpec);
	 if (!uriSpec)
		CQaUtils::QAOutput("uriSpec for GetSpec() invalid. Test failed.", 1);
	 else
		CQaUtils::FormatAndPrintOutput("The SimpleEnum URL = ", uriSpec, 2);		 
  } 
}

void CTests::PurgeHistoryTest(nsISHistory* theSessionHistory, PRInt32 numEntries)
{
   rv = theSessionHistory->PurgeHistory(numEntries);
   CQaUtils::RvTestResult(rv, "PurgeHistory() test", 2);
}

// ***********************************************************************
// ***********************************************************************
// nsIWebNavigation iface

void CTests::OnInterfacesNsiwebnav()
{
   if (qaWebNav)
	   CQaUtils::QAOutput("We have the web nav object.", 2);
   else {
	   CQaUtils::QAOutput("We don't have the web nav object. No tests performed.", 2);
	   return;
   }

   // canGoBack attribute test
   CanGoBackTest();

   // GoBack test
   GoBackTest();

   // canGoForward attribute test
   CanGoForwardTest();

   // GoForward test
   GoForwardTest();

   // GotoIndex test
   GoToIndexTest();

   // LoadURI() & reload tests

   CQaUtils::QAOutput("Run a few LoadURI() tests.", 2);

   LoadUriTest("http://www.yahoo.com/", nsIWebNavigation::LOAD_FLAGS_NONE);
   ReloadTest(nsIWebNavigation::LOAD_FLAGS_NONE);

   LoadUriTest("http://www.cisco.com/", nsIWebNavigation::LOAD_FLAGS_MASK);
// ReloadTest(nsIWebNavigation::LOAD_FLAGS_MASK);

   LoadUriTest("http://www.netscape.com/", nsIWebNavigation::LOAD_FLAGS_IS_LINK);
   LoadUriTest("http://www.aol.com/", nsIWebNavigation::LOAD_FLAGS_BYPASS_HISTORY);
 
	// Stop() test
   StopUriTest("http://www.microsoft.com/");

   // document test
   GetDocumentTest();
   
   // uri test
   GetCurrentURITest();

   // session history test
   GetSHTest();
}

// ***********************************************************************
// Individual nsISHistory tests

void CTests::CanGoBackTest()
{
   PRBool canGoBack = PR_FALSE;
   rv = qaWebNav->GetCanGoBack(&canGoBack);
   if (!canGoBack)
	  CQaUtils::QAOutput("canGoBack for GetCanGoBack() invalid. Test failed.", 1);
   else
      CQaUtils::RvTestResult(rv, "GetCanGoBack() attribute test", 2);
   strMsg.Format("canGoBack value = %d", canGoBack); 
   AfxMessageBox(strMsg); 
}

void CTests::GoBackTest()
{
   rv = qaWebNav->GoBack();
   CQaUtils::RvTestResult(rv, "GoBack() test", 2);
}

void CTests::CanGoForwardTest()
{
   PRBool canGoForward = PR_FALSE;
   rv = qaWebNav->GetCanGoForward(&canGoForward);
   if (!canGoForward)
	  CQaUtils::QAOutput("canGoForward for GetCanGoForward() invalid. Test failed.", 1);
   else
   CQaUtils::RvTestResult(rv, "GetCanGoForward() attribute test", 2);
   strMsg.Format("canGoForward value = %d", canGoForward); 
   AfxMessageBox(strMsg); 
}

void CTests::GoForwardTest()
{
   rv = qaWebNav->GoForward();
   CQaUtils::RvTestResult(rv, "GoForward() test", 2);
}

void CTests::GoToIndexTest()
{
   PRInt32 theIndex = 0;

   rv = qaWebNav->GotoIndex(theIndex);
   CQaUtils::RvTestResult(rv, "GotoIndex() test", 2);
}

void CTests::LoadUriTest(char *theUrl, const unsigned long theFlag)
{
   char theTotalString[500];
   char theFlagName[100];

   switch(theFlag)
   {
   case nsIWebNavigation::LOAD_FLAGS_NONE:
	   strcpy(theFlagName, "LOAD_FLAGS_NONE");
	   break;
   case nsIWebNavigation::LOAD_FLAGS_MASK:
	   strcpy(theFlagName, "LOAD_FLAGS_MASK");
	   break;
   case nsIWebNavigation::LOAD_FLAGS_IS_REFRESH:
	   strcpy(theFlagName, "LOAD_FLAGS_IS_REFRESH");
	   break;
   case nsIWebNavigation::LOAD_FLAGS_IS_LINK:
	   strcpy(theFlagName, "LOAD_FLAGS_IS_LINK");
	   break;
   case nsIWebNavigation::LOAD_FLAGS_BYPASS_HISTORY:
	   strcpy(theFlagName, "LOAD_FLAGS_BYPASS_HISTORY");
	   break;
   case nsIWebNavigation::LOAD_FLAGS_REPLACE_HISTORY:
	   strcpy(theFlagName, "LOAD_FLAGS_REPLACE_HISTORY");
	   break;
   case nsIWebNavigation::LOAD_FLAGS_BYPASS_CACHE:
	   strcpy(theFlagName, "LOAD_FLAGS_BYPASS_CACHE");
	   break;
   case nsIWebNavigation::LOAD_FLAGS_BYPASS_PROXY:
	   strcpy(theFlagName, "LOAD_FLAGS_BYPASS_PROXY");
	   break;
   }

   rv = qaWebNav->LoadURI(NS_ConvertASCIItoUCS2(theUrl).GetUnicode(), 
						theFlag);
   sprintf(theTotalString, "%s%s%s%s%s", "LoadURI(): ", theUrl, " w/ ", theFlagName, " test");
   CQaUtils::RvTestResult(rv, theTotalString, 2);
}

void CTests::ReloadTest(const unsigned long theFlag)
{
   char theTotalString[500];
   char theFlagName[100];

   switch(theFlag)
   {
   case nsIWebNavigation::LOAD_FLAGS_NONE:
	   strcpy(theFlagName, "LOAD_FLAGS_NONE");
	   break;
   case nsIWebNavigation::LOAD_FLAGS_MASK:
	   strcpy(theFlagName, "LOAD_FLAGS_MASK");
	   break;
   case nsIWebNavigation::LOAD_FLAGS_IS_REFRESH:
	   strcpy(theFlagName, "LOAD_FLAGS_IS_REFRESH");
	   break;
   case nsIWebNavigation::LOAD_FLAGS_IS_LINK:
	   strcpy(theFlagName, "LOAD_FLAGS_IS_LINK");
	   break;
   case nsIWebNavigation::LOAD_FLAGS_BYPASS_HISTORY:
	   strcpy(theFlagName, "LOAD_FLAGS_BYPASS_HISTORY");
	   break;
   case nsIWebNavigation::LOAD_FLAGS_REPLACE_HISTORY:
	   strcpy(theFlagName, "LOAD_FLAGS_REPLACE_HISTORY");
	   break;
   case nsIWebNavigation::LOAD_FLAGS_BYPASS_CACHE:
	   strcpy(theFlagName, "LOAD_FLAGS_BYPASS_CACHE");
	   break;
   case nsIWebNavigation::LOAD_FLAGS_BYPASS_PROXY:
	   strcpy(theFlagName, "LOAD_FLAGS_BYPASS_PROXY");
	   break;
   }

   rv = qaWebNav->Reload(theFlag);
   sprintf(theTotalString, "%s%s%s%s", "Reload(): ", " w/ ", theFlagName, " test");
   CQaUtils::RvTestResult(rv, theTotalString, 2);
}

void CTests::StopUriTest(char *theUrl)
{
   char theTotalString[200];

   qaWebNav->LoadURI(NS_ConvertASCIItoUCS2(theUrl).GetUnicode(), 
						nsIWebNavigation::LOAD_FLAGS_NONE);
   rv = qaWebNav->Stop();
   sprintf(theTotalString, "%s%s%s", "Stop(): ", theUrl, " test");
   CQaUtils::RvTestResult(rv, theTotalString, 2);
}

void CTests::GetDocumentTest()
{
   nsCOMPtr<nsIDOMDocument> theDocument; 
   rv = qaWebNav->GetDocument(getter_AddRefs(theDocument));
   if (!theDocument)
	  CQaUtils::QAOutput("We didn't get the document. Test failed.", 2);
   else
	  CQaUtils::RvTestResult(rv, "GetDocument() test", 2);
}

void CTests::GetCurrentURITest()
{
   nsCOMPtr<nsIURI> theUri;
   rv = qaWebNav->GetCurrentURI(getter_AddRefs(theUri));
   if (!theUri)
      CQaUtils::QAOutput("We didn't get the URI. Test failed.", 2);
   else
	  CQaUtils::RvTestResult(rv, "GetCurrentURI() test", 2);

   char *uriSpec;
   rv = theUri->GetSpec(&uriSpec);

   CQaUtils::FormatAndPrintOutput("the uri = ", uriSpec, 2);
}

void CTests::GetSHTest()
{
   nsCOMPtr<nsISHistory> theSessionHistory;
   rv = qaWebNav->GetSessionHistory(getter_AddRefs(theSessionHistory));
   if (!theSessionHistory)
      CQaUtils::QAOutput("We didn't get the session history. Test failed.", 2);
   else
	  CQaUtils::RvTestResult(rv, "GetSessionHistory() test", 2);
}

// ***********************************************************************
//DHARMA	- nsIClipboardCommands
// Checking the paste() method.
void CTests::OnPasteTest()
{
 //   AfxMessageBox("testing paste command");
    nsCOMPtr<nsIClipboardCommands> clipCmds = do_GetInterface(qaWebBrowser);
        if (clipCmds)
                clipCmds->Paste();
}

// Checking the copySelection() method.
void CTests::OnCopyTest()
{
//       AfxMessageBox("testing copyselection command");
         nsCOMPtr<nsIClipboardCommands> clipCmds = do_GetInterface(qaWebBrowser);
         if (clipCmds)
                clipCmds->CopySelection();
}

// Checking the selectAll() method.
void CTests::OnSelectAllTest()
{
         AfxMessageBox("testing selectall method");
         nsCOMPtr<nsIClipboardCommands> clipCmds = do_GetInterface(qaWebBrowser);
         if (clipCmds)
                clipCmds->SelectAll();
}

// Checking the selectNone() method.
void CTests::OnSelectNoneTest()
{
         AfxMessageBox("testing selectnone method");
         nsCOMPtr<nsIClipboardCommands> clipCmds = do_GetInterface(qaWebBrowser);
         if (clipCmds)
                clipCmds->SelectNone();
}

// Checking the cutSelection() method.
void CTests::OnCutSelectionTest()
{
//       AfxMessageBox("testing cutselection method");
         nsCOMPtr<nsIClipboardCommands> clipCmds = do_GetInterface(qaWebBrowser);
         if (clipCmds)
                clipCmds->CutSelection();
}
// Checking the copyLinkLocation() method.
void CTests::copyLinkLocationTest()
{
//       AfxMessageBox("testing cutselection method");
         nsCOMPtr<nsIClipboardCommands> clipCmds = do_GetInterface(qaWebBrowser);
         if (clipCmds)
                clipCmds->CopyLinkLocation();
}

// Checking the canCopySelection() method.
void CTests::canCopySelectionTest()
{
        PRBool canCopySelection = PR_FALSE;
        nsCOMPtr<nsIClipboardCommands> clipCmds = do_GetInterface(qaWebBrowser);
        if (clipCmds)
                clipCmds->CanCopySelection(&canCopySelection);

    if(canCopySelection)
                AfxMessageBox("The selection you made Can be copied");
        else
                AfxMessageBox("Either you didnot make a selection or The selection you made Cannot be copied");
}

// Checking the canCutSelection() method.
void CTests::canCutSelectionTest()
{
        PRBool canCutSelection = PR_FALSE;
        nsCOMPtr<nsIClipboardCommands> clipCmds = do_GetInterface(qaWebBrowser);
        if (clipCmds)
                clipCmds->CanCutSelection(&canCutSelection);

    if(canCutSelection)
                AfxMessageBox("The selection you made Can be cut");
        else
                AfxMessageBox("Either you didnot make a selection or The selection you made Cannot be cut");
}

// Checking the canPaste() method.
void CTests::canPasteTest()
{
        PRBool canPaste = PR_FALSE;
        nsCOMPtr<nsIClipboardCommands> clipCmds = do_GetInterface(qaWebBrowser);
        if (clipCmds)
                clipCmds->CanPaste(&canPaste);

    if(canPaste)
                AfxMessageBox("The clipboard contents can be pasted here");
        else
                AfxMessageBox("The clipboard contents cannot be pasted here");
}

//DHARMA


// ***********************************************************************
// ***************** Bug Verifications ******************
// ***********************************************************************

void CTests::OnVerifybugs70228() 
{
	CQaUtils::QAOutput("Not implemented yet.", 0);
/*
	nsCOMPtr<nsIHelperAppLauncherDialog> 
			myHALD(do_GetService(NS_IHELPERAPPLAUNCHERDLG_CONTRACTID));
	if (!myHALD)
		CQaUtils::QAOutput("Object not created. It's NOT a service!", 2);
	else
		CQaUtils::QAOutput("Object is created. But should it?! It's NOT a service!", 2);

	nsCOMPtr<nsIHelperAppLauncherDialog> 
			myHALD(do_CreateInstance(NS_IHELPERAPPLAUNCHERDLG_CONTRACTID));
	if (!myHALD)
		CQaUtils::QAOutput("Object not created. It should be. It's a component!", 2);
	else
		CQaUtils::QAOutput("Object is created. It's a component!", 2);	
*/
/*
nsCOMPtr<nsIHelperAppLauncher> 
			myHAL(do_CreateInstance(NS_IHELPERAPPLAUNCHERDLG_CONTRACTID));

	rv = myHALD->show(myHal, mySupp);
*/	
}

