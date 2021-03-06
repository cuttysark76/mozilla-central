/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * Implementation of DOMSettableTokenList specified by HTML5.
 */

#ifndef nsDOMSettableTokenList_h___
#define nsDOMSettableTokenList_h___

#include "nsDOMTokenList.h"

class nsIAtom;

// nsISupports must be on the primary inheritance chain
// because nsDOMSettableTokenList is traversed by Element.
class nsDOMSettableTokenList MOZ_FINAL : public nsDOMTokenList
{
public:

  nsDOMSettableTokenList(mozilla::dom::Element* aElement, nsIAtom* aAttrAtom)
    : nsDOMTokenList(aElement, aAttrAtom) {}

  virtual JSObject* WrapObject(JSContext *cx,
                               JS::Handle<JSObject*> scope) MOZ_OVERRIDE;

  // WebIDL
  void GetValue(nsAString& aResult) { Stringify(aResult); }
  void SetValue(const nsAString& aValue, mozilla::ErrorResult& rv);
};

#endif // nsDOMSettableTokenList_h___

