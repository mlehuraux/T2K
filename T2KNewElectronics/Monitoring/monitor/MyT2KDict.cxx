// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME MyT2KDict

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
#include "../src/DAQ.h"
#include "../src/Mapping.h"
#include "../src/Pixel.h"
#include "../src/Pads.h"
#include "../src/datum_decoder.h"
#include "../src/fdecoder.h"
#include "../src/frame.h"
#include "../src/Globals.h"
#include "../src/platform_spec.h"
#include "../src/T2KMainFrame.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *DAQ_Dictionary();
   static void DAQ_TClassManip(TClass*);
   static void *new_DAQ(void *p = 0);
   static void *newArray_DAQ(Long_t size, void *p);
   static void delete_DAQ(void *p);
   static void deleteArray_DAQ(void *p);
   static void destruct_DAQ(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::DAQ*)
   {
      ::DAQ *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::DAQ));
      static ::ROOT::TGenericClassInfo 
         instance("DAQ", "../src/DAQ.h", 6,
                  typeid(::DAQ), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &DAQ_Dictionary, isa_proxy, 4,
                  sizeof(::DAQ) );
      instance.SetNew(&new_DAQ);
      instance.SetNewArray(&newArray_DAQ);
      instance.SetDelete(&delete_DAQ);
      instance.SetDeleteArray(&deleteArray_DAQ);
      instance.SetDestructor(&destruct_DAQ);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::DAQ*)
   {
      return GenerateInitInstanceLocal((::DAQ*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::DAQ*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *DAQ_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::DAQ*)0x0)->GetClass();
      DAQ_TClassManip(theClass);
   return theClass;
   }

   static void DAQ_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *Mapping_Dictionary();
   static void Mapping_TClassManip(TClass*);
   static void *new_Mapping(void *p = 0);
   static void *newArray_Mapping(Long_t size, void *p);
   static void delete_Mapping(void *p);
   static void deleteArray_Mapping(void *p);
   static void destruct_Mapping(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Mapping*)
   {
      ::Mapping *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::Mapping));
      static ::ROOT::TGenericClassInfo 
         instance("Mapping", "../src/Mapping.h", 6,
                  typeid(::Mapping), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &Mapping_Dictionary, isa_proxy, 4,
                  sizeof(::Mapping) );
      instance.SetNew(&new_Mapping);
      instance.SetNewArray(&newArray_Mapping);
      instance.SetDelete(&delete_Mapping);
      instance.SetDeleteArray(&deleteArray_Mapping);
      instance.SetDestructor(&destruct_Mapping);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Mapping*)
   {
      return GenerateInitInstanceLocal((::Mapping*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Mapping*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *Mapping_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::Mapping*)0x0)->GetClass();
      Mapping_TClassManip(theClass);
   return theClass;
   }

   static void Mapping_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *Pixel_Dictionary();
   static void Pixel_TClassManip(TClass*);
   static void *new_Pixel(void *p = 0);
   static void *newArray_Pixel(Long_t size, void *p);
   static void delete_Pixel(void *p);
   static void deleteArray_Pixel(void *p);
   static void destruct_Pixel(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Pixel*)
   {
      ::Pixel *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::Pixel));
      static ::ROOT::TGenericClassInfo 
         instance("Pixel", "../src/Pixel.h", 11,
                  typeid(::Pixel), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &Pixel_Dictionary, isa_proxy, 4,
                  sizeof(::Pixel) );
      instance.SetNew(&new_Pixel);
      instance.SetNewArray(&newArray_Pixel);
      instance.SetDelete(&delete_Pixel);
      instance.SetDeleteArray(&deleteArray_Pixel);
      instance.SetDestructor(&destruct_Pixel);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Pixel*)
   {
      return GenerateInitInstanceLocal((::Pixel*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Pixel*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *Pixel_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::Pixel*)0x0)->GetClass();
      Pixel_TClassManip(theClass);
   return theClass;
   }

   static void Pixel_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *T2KMainFrame_Dictionary();
   static void T2KMainFrame_TClassManip(TClass*);
   static void delete_T2KMainFrame(void *p);
   static void deleteArray_T2KMainFrame(void *p);
   static void destruct_T2KMainFrame(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::T2KMainFrame*)
   {
      ::T2KMainFrame *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::T2KMainFrame));
      static ::ROOT::TGenericClassInfo 
         instance("T2KMainFrame", "../src/T2KMainFrame.h", 15,
                  typeid(::T2KMainFrame), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &T2KMainFrame_Dictionary, isa_proxy, 4,
                  sizeof(::T2KMainFrame) );
      instance.SetDelete(&delete_T2KMainFrame);
      instance.SetDeleteArray(&deleteArray_T2KMainFrame);
      instance.SetDestructor(&destruct_T2KMainFrame);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::T2KMainFrame*)
   {
      return GenerateInitInstanceLocal((::T2KMainFrame*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::T2KMainFrame*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *T2KMainFrame_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::T2KMainFrame*)0x0)->GetClass();
      T2KMainFrame_TClassManip(theClass);
   return theClass;
   }

   static void T2KMainFrame_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_DAQ(void *p) {
      return  p ? new(p) ::DAQ : new ::DAQ;
   }
   static void *newArray_DAQ(Long_t nElements, void *p) {
      return p ? new(p) ::DAQ[nElements] : new ::DAQ[nElements];
   }
   // Wrapper around operator delete
   static void delete_DAQ(void *p) {
      delete ((::DAQ*)p);
   }
   static void deleteArray_DAQ(void *p) {
      delete [] ((::DAQ*)p);
   }
   static void destruct_DAQ(void *p) {
      typedef ::DAQ current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::DAQ

namespace ROOT {
   // Wrappers around operator new
   static void *new_Mapping(void *p) {
      return  p ? new(p) ::Mapping : new ::Mapping;
   }
   static void *newArray_Mapping(Long_t nElements, void *p) {
      return p ? new(p) ::Mapping[nElements] : new ::Mapping[nElements];
   }
   // Wrapper around operator delete
   static void delete_Mapping(void *p) {
      delete ((::Mapping*)p);
   }
   static void deleteArray_Mapping(void *p) {
      delete [] ((::Mapping*)p);
   }
   static void destruct_Mapping(void *p) {
      typedef ::Mapping current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Mapping

namespace ROOT {
   // Wrappers around operator new
   static void *new_Pixel(void *p) {
      return  p ? new(p) ::Pixel : new ::Pixel;
   }
   static void *newArray_Pixel(Long_t nElements, void *p) {
      return p ? new(p) ::Pixel[nElements] : new ::Pixel[nElements];
   }
   // Wrapper around operator delete
   static void delete_Pixel(void *p) {
      delete ((::Pixel*)p);
   }
   static void deleteArray_Pixel(void *p) {
      delete [] ((::Pixel*)p);
   }
   static void destruct_Pixel(void *p) {
      typedef ::Pixel current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Pixel

namespace ROOT {
   // Wrapper around operator delete
   static void delete_T2KMainFrame(void *p) {
      delete ((::T2KMainFrame*)p);
   }
   static void deleteArray_T2KMainFrame(void *p) {
      delete [] ((::T2KMainFrame*)p);
   }
   static void destruct_T2KMainFrame(void *p) {
      typedef ::T2KMainFrame current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::T2KMainFrame

namespace {
  void TriggerDictionaryInitialization_MyT2KDict_Impl() {
    static const char* headers[] = {
"../src/DAQ.h",
"../src/Mapping.h",
"../src/Pixel.h",
"../src/Pads.h",
"../src/datum_decoder.h",
"../src/fdecoder.h",
"../src/frame.h",
"../src/Globals.h",
"../src/platform_spec.h",
"../src/T2KMainFrame.h",
0
    };
    static const char* includePaths[] = {
"/local/home/t2kt/root/build/include",
"/local/home/t2kt/Documents/GitLab/T2K/T2KNewElectronics/Monitoring/monitor/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "MyT2KDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$../src/DAQ.h")))  DAQ;
class __attribute__((annotate("$clingAutoload$../src/Mapping.h")))  Mapping;
class __attribute__((annotate("$clingAutoload$../src/Pixel.h")))  Pixel;
class __attribute__((annotate("$clingAutoload$../src/T2KMainFrame.h")))  T2KMainFrame;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "MyT2KDict dictionary payload"

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "../src/DAQ.h"
#include "../src/Mapping.h"
#include "../src/Pixel.h"
#include "../src/Pads.h"
#include "../src/datum_decoder.h"
#include "../src/fdecoder.h"
#include "../src/frame.h"
#include "../src/Globals.h"
#include "../src/platform_spec.h"
#include "../src/T2KMainFrame.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"DAQ", payloadCode, "@",
"Mapping", payloadCode, "@",
"Pixel", payloadCode, "@",
"T2KMainFrame", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("MyT2KDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_MyT2KDict_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_MyT2KDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_MyT2KDict() {
  TriggerDictionaryInitialization_MyT2KDict_Impl();
}
