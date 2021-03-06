/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=2 sw=2 et tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsDOMClassInfo_h___
#define nsDOMClassInfo_h___

#include "mozilla/Attributes.h"
#include "nsIXPCScriptable.h"
#include "nsIScriptGlobalObject.h"
#include "nsIDOMScriptObjectFactory.h"
#include "js/Id.h"
#include "nsIXPConnect.h"

#ifdef XP_WIN
#undef GetClassName
#endif

class nsContentList;
class nsDocument;
class nsGlobalWindow;
class nsIScriptSecurityManager;

struct nsDOMClassInfoData;

typedef nsIClassInfo* (*nsDOMClassInfoConstructorFnc)
  (nsDOMClassInfoData* aData);

typedef nsresult (*nsDOMConstructorFunc)(nsISupports** aNewObject);

struct nsDOMClassInfoData
{
  const char *mName;
  const PRUnichar *mNameUTF16;
  union {
    nsDOMClassInfoConstructorFnc mConstructorFptr;
    nsDOMClassInfoExternalConstructorFnc mExternalConstructorFptr;
  } u;

  nsIClassInfo *mCachedClassInfo; // low bit is set to 1 if external,
                                  // so be sure to mask if necessary!
  const nsIID *mProtoChainInterface;
  const nsIID **mInterfaces;
  uint32_t mScriptableFlags : 31; // flags must not use more than 31 bits!
  uint32_t mHasClassInterface : 1;
  uint32_t mInterfacesBitmap;
  bool mChromeOnly : 1;
  bool mAllowXBL : 1;
  bool mDisabled : 1;
#ifdef DEBUG
  uint32_t mDebugID;
#endif
};

struct nsExternalDOMClassInfoData : public nsDOMClassInfoData
{
  const nsCID *mConstructorCID;
};


// To be used with the nsDOMClassInfoData::mCachedClassInfo pointer.
// The low bit is set when we created a generic helper for an external
// (which holds on to the nsDOMClassInfoData).
#define GET_CLEAN_CI_PTR(_ptr) (nsIClassInfo*)(uintptr_t(_ptr) & ~0x1)
#define MARK_EXTERNAL(_ptr) (nsIClassInfo*)(uintptr_t(_ptr) | 0x1)
#define IS_EXTERNAL(_ptr) (uintptr_t(_ptr) & 0x1)


class nsDOMClassInfo : public nsXPCClassInfo
{
  friend class nsHTMLDocumentSH;
public:
  nsDOMClassInfo(nsDOMClassInfoData* aData);
  virtual ~nsDOMClassInfo();

  NS_DECL_NSIXPCSCRIPTABLE

  NS_DECL_ISUPPORTS

  NS_DECL_NSICLASSINFO

  // Helper method that returns a *non* refcounted pointer to a
  // helper. So please note, don't release this pointer, if you do,
  // you better make sure you've addreffed before release.
  //
  // Whaaaaa! I wanted to name this method GetClassInfo, but nooo,
  // some of Microsoft devstudio's headers #defines GetClassInfo to
  // GetClassInfoA so I can't, those $%#@^! bastards!!! What gives
  // them the right to do that?

  static nsIClassInfo* GetClassInfoInstance(nsDOMClassInfoData* aData);

  static void ShutDown();

  static nsIClassInfo *doCreate(nsDOMClassInfoData* aData)
  {
    return new nsDOMClassInfo(aData);
  }

  /*
   * The following two functions exist because of the way that Xray wrappers
   * work. In order to allow scriptable helpers to define non-IDL defined but
   * still "safe" properties for Xray wrappers, we call into the scriptable
   * helper with |obj| being the wrapper.
   *
   * Ideally, that would be the end of the story, however due to complications
   * dealing with document.domain, it's possible to end up in a scriptable
   * helper with a wrapper, even though we should be treating the lookup as a
   * transparent one.
   *
   * Note: So ObjectIsNativeWrapper(cx, obj) check usually means "through xray
   * wrapper this part is not visible" while combined with
   * || xpc::WrapperFactory::XrayWrapperNotShadowing(obj) it means "through
   * xray wrapper it is visible only if it does not hide any native property."
   */
  static bool ObjectIsNativeWrapper(JSContext* cx, JSObject* obj);

  static nsISupports *GetNative(nsIXPConnectWrappedNative *wrapper, JSObject *obj);

  static nsIXPConnect *XPConnect()
  {
    return sXPConnect;
  }
  static nsIScriptSecurityManager *ScriptSecurityManager()
  {
    return sSecMan;
  }

protected:
  friend nsIClassInfo* NS_GetDOMClassInfoInstance(nsDOMClassInfoID aID);

  const nsDOMClassInfoData* mData;

  virtual void PreserveWrapper(nsISupports *aNative) MOZ_OVERRIDE
  {
  }

  virtual uint32_t GetInterfacesBitmap() MOZ_OVERRIDE
  {
    return mData->mInterfacesBitmap;
  }

  static nsresult Init();
  static nsresult RegisterClassProtos(int32_t aDOMClassInfoID);
  static nsresult RegisterExternalClasses();
  nsresult ResolveConstructor(JSContext *cx, JSObject *obj,
                              JSObject **objp);

  // Checks if id is a number and returns the number, if aIsNumber is
  // non-null it's set to true if the id is a number and false if it's
  // not a number. If id is not a number this method returns -1
  static int32_t GetArrayIndexFromId(JSContext *cx, JS::Handle<jsid> id,
                                     bool *aIsNumber = nullptr);

  static nsIXPConnect *sXPConnect;
  static nsIScriptSecurityManager *sSecMan;

  // nsIXPCScriptable code
  static nsresult DefineStaticJSVals(JSContext *cx);

  static bool sIsInitialized;

public:
  static jsid sLocation_id;
  static jsid sConstructor_id;
  static jsid sLength_id;
  static jsid sItem_id;
  static jsid sNamedItem_id;
  static jsid sEnumerate_id;
  static jsid sTop_id;
  static jsid sDocument_id;
  static jsid sWrappedJSObject_id;
};

// THIS ONE ISN'T SAFE!! It assumes that the private of the JSObject is
// an nsISupports.
inline
const nsQueryInterface
do_QueryWrappedNative(nsIXPConnectWrappedNative *wrapper, JSObject *obj)
{
  return nsQueryInterface(nsDOMClassInfo::GetNative(wrapper, obj));
}

// THIS ONE ISN'T SAFE!! It assumes that the private of the JSObject is
// an nsISupports.
inline
const nsQueryInterfaceWithError
do_QueryWrappedNative(nsIXPConnectWrappedNative *wrapper, JSObject *obj,
                      nsresult *aError)

{
  return nsQueryInterfaceWithError(nsDOMClassInfo::GetNative(wrapper, obj),
                                   aError);
}

inline
nsQueryInterface
do_QueryWrapper(JSContext *cx, JSObject *obj)
{
  nsISupports *native =
    nsDOMClassInfo::XPConnect()->GetNativeOfWrapper(cx, obj);
  return nsQueryInterface(native);
}

inline
nsQueryInterfaceWithError
do_QueryWrapper(JSContext *cx, JSObject *obj, nsresult* error)
{
  nsISupports *native =
    nsDOMClassInfo::XPConnect()->GetNativeOfWrapper(cx, obj);
  return nsQueryInterfaceWithError(native, error);
}


typedef nsDOMClassInfo nsDOMGenericSH;

// Makes sure that the wrapper is preserved if new properties are added.
class nsEventTargetSH : public nsDOMGenericSH
{
protected:
  nsEventTargetSH(nsDOMClassInfoData* aData) : nsDOMGenericSH(aData)
  {
  }

  virtual ~nsEventTargetSH()
  {
  }
public:
  NS_IMETHOD PreCreate(nsISupports *nativeObj, JSContext *cx,
                       JSObject *globalObj, JSObject **parentObj) MOZ_OVERRIDE;
  NS_IMETHOD AddProperty(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                         JSObject *obj, jsid id, JS::Value *vp, bool *_retval) MOZ_OVERRIDE;

  virtual void PreserveWrapper(nsISupports *aNative) MOZ_OVERRIDE;

  static nsIClassInfo *doCreate(nsDOMClassInfoData* aData)
  {
    return new nsEventTargetSH(aData);
  }
};

// Window scriptable helper

class nsWindowSH : public nsDOMGenericSH
{
protected:
  nsWindowSH(nsDOMClassInfoData *aData) : nsDOMGenericSH(aData)
  {
  }

  virtual ~nsWindowSH()
  {
  }

  static nsresult GlobalResolve(nsGlobalWindow *aWin, JSContext *cx,
                                JS::Handle<JSObject*> obj, JS::Handle<jsid> id,
                                bool *did_resolve);

public:
  NS_IMETHOD PreCreate(nsISupports *nativeObj, JSContext *cx,
                       JSObject *globalObj, JSObject **parentObj) MOZ_OVERRIDE;
  NS_IMETHOD PostCreatePrototype(JSContext * cx, JSObject * proto) MOZ_OVERRIDE;
  NS_IMETHOD PostCreate(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                        JSObject *obj) MOZ_OVERRIDE;
  NS_IMETHOD Enumerate(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                       JSObject *obj, bool *_retval) MOZ_OVERRIDE;
  NS_IMETHOD NewResolve(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                        JSObject *obj, jsid id, uint32_t flags,
                        JSObject **objp, bool *_retval) MOZ_OVERRIDE;
  NS_IMETHOD Finalize(nsIXPConnectWrappedNative *wrapper, JSFreeOp *fop,
                      JSObject *obj) MOZ_OVERRIDE;
  NS_IMETHOD OuterObject(nsIXPConnectWrappedNative *wrapper, JSContext * cx,
                         JSObject * obj, JSObject * *_retval) MOZ_OVERRIDE;

  static bool GlobalScopePolluterNewResolve(JSContext *cx, JS::Handle<JSObject*> obj,
                                            JS::Handle<jsid> id, unsigned flags,
                                            JS::MutableHandle<JSObject*> objp);
  static bool GlobalScopePolluterGetProperty(JSContext *cx, JS::Handle<JSObject*> obj,
                                             JS::Handle<jsid> id, JS::MutableHandle<JS::Value> vp);
  static bool InvalidateGlobalScopePolluter(JSContext *cx,
                                            JS::Handle<JSObject*> obj);
  static nsresult InstallGlobalScopePolluter(JSContext *cx,
                                             JS::Handle<JSObject*> obj);
  static nsIClassInfo *doCreate(nsDOMClassInfoData* aData)
  {
    return new nsWindowSH(aData);
  }
};

// Location scriptable helper

class nsLocationSH : public nsDOMGenericSH
{
protected:
  nsLocationSH(nsDOMClassInfoData* aData) : nsDOMGenericSH(aData)
  {
  }

  virtual ~nsLocationSH()
  {
  }

public:
  NS_IMETHOD CheckAccess(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                         JSObject *obj, jsid id, uint32_t mode,
                         JS::Value *vp, bool *_retval) MOZ_OVERRIDE;

  NS_IMETHOD PreCreate(nsISupports *nativeObj, JSContext *cx,
                       JSObject *globalObj, JSObject **parentObj) MOZ_OVERRIDE;
  NS_IMETHODIMP AddProperty(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                            JSObject *obj, jsid id, JS::Value *vp, bool *_retval);

  static nsIClassInfo *doCreate(nsDOMClassInfoData* aData)
  {
    return new nsLocationSH(aData);
  }
};


// Generic array scriptable helper

class nsGenericArraySH : public nsDOMClassInfo
{
protected:
  nsGenericArraySH(nsDOMClassInfoData* aData) : nsDOMClassInfo(aData)
  {
  }

  virtual ~nsGenericArraySH()
  {
  }
  
public:
  NS_IMETHOD NewResolve(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                        JSObject *obj, jsid id, uint32_t flags,
                        JSObject **objp, bool *_retval) MOZ_OVERRIDE;
  NS_IMETHOD Enumerate(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                       JSObject *obj, bool *_retval) MOZ_OVERRIDE;
  
  virtual nsresult GetLength(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                             JSObject *obj, uint32_t *length);

  static nsIClassInfo *doCreate(nsDOMClassInfoData* aData)
  {
    return new nsGenericArraySH(aData);
  }
};


// Array scriptable helper

class nsArraySH : public nsGenericArraySH
{
protected:
  nsArraySH(nsDOMClassInfoData* aData) : nsGenericArraySH(aData)
  {
  }

  virtual ~nsArraySH()
  {
  }

  // Subclasses need to override this, if the implementation can't fail it's
  // allowed to not set *aResult.
  virtual nsISupports* GetItemAt(nsISupports *aNative, uint32_t aIndex,
                                 nsWrapperCache **aCache, nsresult *aResult) = 0;

public:
  NS_IMETHOD GetProperty(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                         JSObject *obj, jsid id, JS::Value *vp, bool *_retval) MOZ_OVERRIDE;

private:
  // Not implemented, nothing should create an instance of this class.
  static nsIClassInfo *doCreate(nsDOMClassInfoData* aData);
};


// HTMLAllCollection

extern const JSClass sHTMLDocumentAllClass;

class nsHTMLDocumentSH
{
protected:
  static bool GetDocumentAllNodeList(JSContext *cx, JS::Handle<JSObject*> obj,
                                     nsDocument *doc,
                                     nsContentList **nodeList);
public:
  static bool DocumentAllGetProperty(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> id,
                                     JS::MutableHandle<JS::Value> vp);
  static bool DocumentAllNewResolve(JSContext *cx, JS::Handle<JSObject*> obj, JS::Handle<jsid> id,
                                    unsigned flags, JS::MutableHandle<JSObject*> objp);
  static void ReleaseDocument(JSFreeOp *fop, JSObject *obj);
  static bool CallToGetPropMapper(JSContext *cx, unsigned argc, JS::Value *vp);
};


// String array helper

class nsStringArraySH : public nsGenericArraySH
{
protected:
  nsStringArraySH(nsDOMClassInfoData* aData) : nsGenericArraySH(aData)
  {
  }

  virtual ~nsStringArraySH()
  {
  }

  virtual nsresult GetStringAt(nsISupports *aNative, int32_t aIndex,
                               nsAString& aResult) = 0;

public:
  NS_IMETHOD GetProperty(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                         JSObject *obj, jsid id, JS::Value *vp, bool *_retval) MOZ_OVERRIDE;
};


// StringList scriptable helper

class nsStringListSH : public nsStringArraySH
{
protected:
  nsStringListSH(nsDOMClassInfoData* aData) : nsStringArraySH(aData)
  {
  }

  virtual ~nsStringListSH()
  {
  }

  virtual nsresult GetStringAt(nsISupports *aNative, int32_t aIndex,
                               nsAString& aResult) MOZ_OVERRIDE;

public:
  // Inherit GetProperty, Enumerate from nsStringArraySH
  
  static nsIClassInfo *doCreate(nsDOMClassInfoData* aData)
  {
    return new nsStringListSH(aData);
  }
};


// StyleSheetList helper

class nsStyleSheetListSH : public nsArraySH
{
protected:
  nsStyleSheetListSH(nsDOMClassInfoData* aData) : nsArraySH(aData)
  {
  }

  virtual ~nsStyleSheetListSH()
  {
  }

  virtual nsISupports* GetItemAt(nsISupports *aNative, uint32_t aIndex,
                                 nsWrapperCache **aCache, nsresult *aResult) MOZ_OVERRIDE;

public:
  static nsIClassInfo *doCreate(nsDOMClassInfoData* aData)
  {
    return new nsStyleSheetListSH(aData);
  }
};


// CSSRuleList helper

class nsCSSRuleListSH : public nsArraySH
{
protected:
  nsCSSRuleListSH(nsDOMClassInfoData* aData) : nsArraySH(aData)
  {
  }

  virtual ~nsCSSRuleListSH()
  {
  }

  virtual nsISupports* GetItemAt(nsISupports *aNative, uint32_t aIndex,
                                 nsWrapperCache **aCache, nsresult *aResult) MOZ_OVERRIDE;

public:
  static nsIClassInfo *doCreate(nsDOMClassInfoData* aData)
  {
    return new nsCSSRuleListSH(aData);
  }
};

// WebApps Storage helpers

class nsStorage2SH : public nsDOMGenericSH
{
protected:
  nsStorage2SH(nsDOMClassInfoData* aData) : nsDOMGenericSH(aData)
  {
  }

  virtual ~nsStorage2SH()
  {
  }

  NS_IMETHOD NewResolve(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                        JSObject *obj, jsid id, uint32_t flags,
                        JSObject **objp, bool *_retval) MOZ_OVERRIDE;
  NS_IMETHOD SetProperty(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                         JSObject *obj, jsid id, JS::Value *vp, bool *_retval) MOZ_OVERRIDE;
  NS_IMETHOD GetProperty(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                         JSObject *obj, jsid id, JS::Value *vp, bool *_retval) MOZ_OVERRIDE;
  NS_IMETHOD DelProperty(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                         JSObject *obj, jsid id, bool *_retval) MOZ_OVERRIDE;
  NS_IMETHOD NewEnumerate(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                          JSObject *obj, uint32_t enum_op, JS::Value *statep,
                          jsid *idp, bool *_retval) MOZ_OVERRIDE;

public:
  static nsIClassInfo *doCreate(nsDOMClassInfoData* aData)
  {
    return new nsStorage2SH(aData);
  }
};

// Event handler 'this' translator class, this is called by XPConnect
// when a "function interface" (nsIDOMEventListener) is called, this
// class extracts 'this' fomr the first argument to the called
// function (nsIDOMEventListener::HandleEvent(in nsIDOMEvent)), this
// class will pass back nsIDOMEvent::currentTarget to be used as
// 'this'.

class nsEventListenerThisTranslator : public nsIXPCFunctionThisTranslator
{
public:
  nsEventListenerThisTranslator()
  {
  }

  virtual ~nsEventListenerThisTranslator()
  {
  }

  // nsISupports
  NS_DECL_ISUPPORTS

  // nsIXPCFunctionThisTranslator
  NS_DECL_NSIXPCFUNCTIONTHISTRANSLATOR
};

class nsDOMConstructorSH : public nsDOMGenericSH
{
protected:
  nsDOMConstructorSH(nsDOMClassInfoData* aData) : nsDOMGenericSH(aData)
  {
  }

public:
  NS_IMETHOD PreCreate(nsISupports *nativeObj, JSContext *cx,
                       JSObject *globalObj, JSObject **parentObj) MOZ_OVERRIDE;
  NS_IMETHOD PostCreatePrototype(JSContext * cx, JSObject * proto) MOZ_OVERRIDE
  {
    return NS_OK;
  }
  NS_IMETHOD NewResolve(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                        JSObject *obj, jsid id, uint32_t flags,
                        JSObject **objp, bool *_retval) MOZ_OVERRIDE;
  NS_IMETHOD Call(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                  JSObject *obj, const JS::CallArgs &args, bool *_retval) MOZ_OVERRIDE;

  NS_IMETHOD Construct(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                       JSObject *obj, const JS::CallArgs &args, bool *_retval) MOZ_OVERRIDE;

  NS_IMETHOD HasInstance(nsIXPConnectWrappedNative *wrapper, JSContext *cx,
                         JSObject *obj, const JS::Value &val, bool *bp,
                         bool *_retval);

  static nsIClassInfo *doCreate(nsDOMClassInfoData* aData)
  {
    return new nsDOMConstructorSH(aData);
  }
};

class nsNonDOMObjectSH : public nsDOMGenericSH
{
protected:
  nsNonDOMObjectSH(nsDOMClassInfoData* aData) : nsDOMGenericSH(aData)
  {
  }

  virtual ~nsNonDOMObjectSH()
  {
  }

public:
  NS_IMETHOD GetFlags(uint32_t *aFlags) MOZ_OVERRIDE;

  static nsIClassInfo *doCreate(nsDOMClassInfoData* aData)
  {
    return new nsNonDOMObjectSH(aData);
  }
};

#endif /* nsDOMClassInfo_h___ */
