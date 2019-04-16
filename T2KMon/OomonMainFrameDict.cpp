// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME OomonMainFrameDict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "MMonSelectCanvasFrame.h"
#include "mygblink.h"
#include "MMonMainFrame.h"
#include "T2KPadPanel.h"
#include "Handlers.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void delete_MMonSelectCanvasFrame(void *p);
   static void deleteArray_MMonSelectCanvasFrame(void *p);
   static void destruct_MMonSelectCanvasFrame(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MMonSelectCanvasFrame*)
   {
      ::MMonSelectCanvasFrame *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::MMonSelectCanvasFrame >(0);
      static ::ROOT::TGenericClassInfo 
         instance("MMonSelectCanvasFrame", ::MMonSelectCanvasFrame::Class_Version(), "MMonSelectCanvasFrame.h", 7,
                  typeid(::MMonSelectCanvasFrame), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::MMonSelectCanvasFrame::Dictionary, isa_proxy, 4,
                  sizeof(::MMonSelectCanvasFrame) );
      instance.SetDelete(&delete_MMonSelectCanvasFrame);
      instance.SetDeleteArray(&deleteArray_MMonSelectCanvasFrame);
      instance.SetDestructor(&destruct_MMonSelectCanvasFrame);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MMonSelectCanvasFrame*)
   {
      return GenerateInitInstanceLocal((::MMonSelectCanvasFrame*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::MMonSelectCanvasFrame*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_MMonMainFrame(void *p = 0);
   static void *newArray_MMonMainFrame(Long_t size, void *p);
   static void delete_MMonMainFrame(void *p);
   static void deleteArray_MMonMainFrame(void *p);
   static void destruct_MMonMainFrame(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MMonMainFrame*)
   {
      ::MMonMainFrame *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::MMonMainFrame >(0);
      static ::ROOT::TGenericClassInfo 
         instance("MMonMainFrame", ::MMonMainFrame::Class_Version(), "MMonMainFrame.h", 29,
                  typeid(::MMonMainFrame), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::MMonMainFrame::Dictionary, isa_proxy, 4,
                  sizeof(::MMonMainFrame) );
      instance.SetNew(&new_MMonMainFrame);
      instance.SetNewArray(&newArray_MMonMainFrame);
      instance.SetDelete(&delete_MMonMainFrame);
      instance.SetDeleteArray(&deleteArray_MMonMainFrame);
      instance.SetDestructor(&destruct_MMonMainFrame);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MMonMainFrame*)
   {
      return GenerateInitInstanceLocal((::MMonMainFrame*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::MMonMainFrame*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static TClass *T2KPadPanel_Dictionary();
   static void T2KPadPanel_TClassManip(TClass*);
   static void *new_T2KPadPanel(void *p = 0);
   static void *newArray_T2KPadPanel(Long_t size, void *p);
   static void delete_T2KPadPanel(void *p);
   static void deleteArray_T2KPadPanel(void *p);
   static void destruct_T2KPadPanel(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::T2KPadPanel*)
   {
      ::T2KPadPanel *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::T2KPadPanel));
      static ::ROOT::TGenericClassInfo 
         instance("T2KPadPanel", "T2KPadPanel.h", 11,
                  typeid(::T2KPadPanel), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &T2KPadPanel_Dictionary, isa_proxy, 4,
                  sizeof(::T2KPadPanel) );
      instance.SetNew(&new_T2KPadPanel);
      instance.SetNewArray(&newArray_T2KPadPanel);
      instance.SetDelete(&delete_T2KPadPanel);
      instance.SetDeleteArray(&deleteArray_T2KPadPanel);
      instance.SetDestructor(&destruct_T2KPadPanel);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::T2KPadPanel*)
   {
      return GenerateInitInstanceLocal((::T2KPadPanel*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::T2KPadPanel*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *T2KPadPanel_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::T2KPadPanel*)0x0)->GetClass();
      T2KPadPanel_TClassManip(theClass);
   return theClass;
   }

   static void T2KPadPanel_TClassManip(TClass* ){
   }

} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr MMonSelectCanvasFrame::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *MMonSelectCanvasFrame::Class_Name()
{
   return "MMonSelectCanvasFrame";
}

//______________________________________________________________________________
const char *MMonSelectCanvasFrame::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MMonSelectCanvasFrame*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int MMonSelectCanvasFrame::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MMonSelectCanvasFrame*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *MMonSelectCanvasFrame::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MMonSelectCanvasFrame*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *MMonSelectCanvasFrame::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MMonSelectCanvasFrame*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr MMonMainFrame::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *MMonMainFrame::Class_Name()
{
   return "MMonMainFrame";
}

//______________________________________________________________________________
const char *MMonMainFrame::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MMonMainFrame*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int MMonMainFrame::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MMonMainFrame*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *MMonMainFrame::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MMonMainFrame*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *MMonMainFrame::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MMonMainFrame*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void MMonSelectCanvasFrame::Streamer(TBuffer &R__b)
{
   // Stream an object of class MMonSelectCanvasFrame.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(MMonSelectCanvasFrame::Class(),this);
   } else {
      R__b.WriteClassBuffer(MMonSelectCanvasFrame::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_MMonSelectCanvasFrame(void *p) {
      delete ((::MMonSelectCanvasFrame*)p);
   }
   static void deleteArray_MMonSelectCanvasFrame(void *p) {
      delete [] ((::MMonSelectCanvasFrame*)p);
   }
   static void destruct_MMonSelectCanvasFrame(void *p) {
      typedef ::MMonSelectCanvasFrame current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::MMonSelectCanvasFrame

//______________________________________________________________________________
void MMonMainFrame::Streamer(TBuffer &R__b)
{
   // Stream an object of class MMonMainFrame.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(MMonMainFrame::Class(),this);
   } else {
      R__b.WriteClassBuffer(MMonMainFrame::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_MMonMainFrame(void *p) {
      return  p ? new(p) ::MMonMainFrame : new ::MMonMainFrame;
   }
   static void *newArray_MMonMainFrame(Long_t nElements, void *p) {
      return p ? new(p) ::MMonMainFrame[nElements] : new ::MMonMainFrame[nElements];
   }
   // Wrapper around operator delete
   static void delete_MMonMainFrame(void *p) {
      delete ((::MMonMainFrame*)p);
   }
   static void deleteArray_MMonMainFrame(void *p) {
      delete [] ((::MMonMainFrame*)p);
   }
   static void destruct_MMonMainFrame(void *p) {
      typedef ::MMonMainFrame current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::MMonMainFrame

namespace ROOT {
   // Wrappers around operator new
   static void *new_T2KPadPanel(void *p) {
      return  p ? new(p) ::T2KPadPanel : new ::T2KPadPanel;
   }
   static void *newArray_T2KPadPanel(Long_t nElements, void *p) {
      return p ? new(p) ::T2KPadPanel[nElements] : new ::T2KPadPanel[nElements];
   }
   // Wrapper around operator delete
   static void delete_T2KPadPanel(void *p) {
      delete ((::T2KPadPanel*)p);
   }
   static void deleteArray_T2KPadPanel(void *p) {
      delete [] ((::T2KPadPanel*)p);
   }
   static void destruct_T2KPadPanel(void *p) {
      typedef ::T2KPadPanel current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::T2KPadPanel

namespace {
  void TriggerDictionaryInitialization_OomonMainFrameDict_Impl() {
    static const char* headers[] = {
"MMonSelectCanvasFrame.h",
"mygblink.h",
"MMonMainFrame.h",
"T2KPadPanel.h",
"Handlers.h",
0
    };
    static const char* includePaths[] = {
"/home/attie/root/INSTALL/v6.15.02/build/include",
"/home/attie/PROJETS/T2K/T2KMon/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "OomonMainFrameDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$MMonSelectCanvasFrame.h")))  MMonSelectCanvasFrame;
class __attribute__((annotate("$clingAutoload$MMonMainFrame.h")))  MMonMainFrame;
class __attribute__((annotate("$clingAutoload$T2KPadPanel.h")))  T2KPadPanel;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "OomonMainFrameDict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "MMonSelectCanvasFrame.h"
#include "mygblink.h"
#include "MMonMainFrame.h"
#include "T2KPadPanel.h"
#include "Handlers.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"HandleResizeEvent", payloadCode, "@",
"MMonMainFrame", payloadCode, "@",
"MMonSelectCanvasFrame", payloadCode, "@",
"T2KPadPanel", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("OomonMainFrameDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_OomonMainFrameDict_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_OomonMainFrameDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_OomonMainFrameDict() {
  TriggerDictionaryInitialization_OomonMainFrameDict_Impl();
}
