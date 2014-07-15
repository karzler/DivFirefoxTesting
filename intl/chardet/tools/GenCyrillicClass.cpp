/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "nsICharsetConverterManager.h"
#include <iostream.h>
#include "nsISupports.h"
#include "nsIComponentManager.h"
#include "nsIServiceManager.h"
#include "nsIUnicodeDecoder.h"
#include "nsIUnicodeEncoder.h"
#include "nsCRT.h"
#include <stdio.h>
#include <stdlib.h>
#if defined(XP_WIN)
#include <io.h>
#endif
#ifdef XP_UNIX
#include <unistd.h>
#endif

//---------------------------------------------------------------------------
void header()
{
char *header=
"#ifndef nsCyrillicClass_h__\n"
"#define nsCyrillicClass_h__\n"
"/* PLEASE DO NOT EDIT THIS FILE DIRECTLY. THIS FILE IS GENERATED BY \n"
"   GenCyrllicClass found in mozilla/intl/chardet/tools\n"
" */\n";
   printf(header);
}
//---------------------------------------------------------------------------
void footer()
{
   printf("#endif\n");
}
//---------------------------------------------------------------------------
void npl()
{
char *npl=
"/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */\n"
"/* This Source Code Form is subject to the terms of the Mozilla Public\n"
" * License, v. 2.0. If a copy of the MPL was not distributed with this\n"
" * file, You can obtain one at http://mozilla.org/MPL/2.0/. */\n";
   printf(npl);
}
//---------------------------------------------------------------------------
static nsIUnicodeEncoder* gKOI8REncoder = nullptr;
static nsICharsetConverterManager* gCCM = nullptr;

//---------------------------------------------------------------------------
uint8_t CyrillicClass(nsIUnicodeDecoder* decoder, uint8_t byte)
{
   char16_t ubuf[2];
   uint8_t bbuf[2];

   int32_t blen = 1;
   int32_t ulen = 1;
   nsresult res = decoder->Convert((char*)&byte, &blen, ubuf, &ulen);
   if(NS_SUCCEEDED(res) && (1 == ulen ))
   {
     ubuf[0] = nsCRT::ToUpper(ubuf[0]);
     blen=1;
     res = gKOI8REncoder->Convert(ubuf,&ulen,(char*)bbuf,&blen);
     if(NS_SUCCEEDED(res) && (1 == blen))
     {
        if(0xe0 <= bbuf[0])
        {
              return bbuf[0] - (uint8_t)0xdf;
        }
     }
   }
   return 0;
}
//---------------------------------------------------------------------------
void genCyrillicClass(const char* name, const char* charset)
{
   nsIUnicodeDecoder *decoder = nullptr;
   nsresult res = NS_OK;
   nsAutoString str(charset);
   res = gCCM->GetUnicodeDecoder(&str, &decoder);
   if(NS_FAILED(res))
   {
      printf("cannot locate %s Decoder\n", charset);
      return;
   }
   printf("static const uint8_t %sMap [128] = {\n",name);
   uint8_t i,j;
   for(i=0x80;i!=0x00;i+=0x10)
   {
     for(j=0;j<=0x0f;j++)
     {
        uint8_t cls = CyrillicClass(decoder, i+j);
        printf(" %2d, ",cls);
     }
     printf("\n");
   }
   printf("};\n");
   NS_IF_RELEASE(decoder);
}
//---------------------------------------------------------------------------


int main(int argc, char** argv) {
  nsresult res = nullptr;

  nsCOMPtr<nsICharsetConverterManager> gCCM = do_GetService(kCharsetConverterManagerCID, &res);

  if(NS_FAILED(res) && (nullptr != gCCM))
   {
      printf("cannot locate CharsetConverterManager\n");
      return(-1);
   }
   nsAutoString koi8r("KOI8-R");
   res = gCCM->GetUnicodeEncoder(&koi8r,&gKOI8REncoder);
   if(NS_FAILED(res) && (nullptr != gKOI8REncoder))
   {
      printf("cannot locate KOI8-R Encoder\n");
      return(-1);
   }


   npl();
   header();
   
     genCyrillicClass("KOI8", "KOI8-R");
     genCyrillicClass("CP1251", "windows-1251");
     genCyrillicClass("IBM866", "IBM866");
     genCyrillicClass("ISO88595", "ISO-8859-5");
     genCyrillicClass("MacCyrillic", "x-mac-cyrillic");
   footer();
   NS_IF_RELEASE(gKOI8REncoder);
   return(0);
};
