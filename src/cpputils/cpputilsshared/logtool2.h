#ifndef SCPP_LOGTOOL_HH
#define SCPP_LOGTOOL_HH

#include <format.h>

#ifndef NOWAMAS

#include <logtool.h>

#if TOOLS_VERSION - 0 < 40
# ifdef LogPrintf
#  undef LogPrintf
# endif
#endif

#else // NOWAMAS

enum {
  LT_DEBUG,
  LT_ALERT,
  LT_NOTIFY,
  LT_TRACE
};

int _LogSetLocation( const char *file, int line );
int _LogPrintf( const char *fac, int level, const char *str , const char *str2 );

#endif // NOWAMAS

// don't redefine LogPrintf, since version in TB2020 is already this implementation
#if TOOLS_VERSION - 0 < 40
# define LogPrintf _LogSetLocation(__FILE__,__LINE__)?0:_CPPLogPrintf
#endif

#if __cplusplus < 201103

template<typename Arg0>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_).c_str());
}

template<typename Arg0, typename Arg1>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_, const Arg14& var14_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_, var14_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_, const Arg14& var14_, const Arg15& var15_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_, var14_, var15_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_, const Arg14& var14_, const Arg15& var15_, const Arg16& var16_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_, var14_, var15_, var16_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_, const Arg14& var14_, const Arg15& var15_, const Arg16& var16_, const Arg17& var17_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_, var14_, var15_, var16_, var17_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_, const Arg14& var14_, const Arg15& var15_, const Arg16& var16_, const Arg17& var17_, const Arg18& var18_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_, var14_, var15_, var16_, var17_, var18_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_, const Arg14& var14_, const Arg15& var15_, const Arg16& var16_, const Arg17& var17_, const Arg18& var18_, const Arg19& var19_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_, var14_, var15_, var16_, var17_, var18_, var19_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_, const Arg14& var14_, const Arg15& var15_, const Arg16& var16_, const Arg17& var17_, const Arg18& var18_, const Arg19& var19_, const Arg20& var20_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_, var14_, var15_, var16_, var17_, var18_, var19_, var20_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_, const Arg14& var14_, const Arg15& var15_, const Arg16& var16_, const Arg17& var17_, const Arg18& var18_, const Arg19& var19_, const Arg20& var20_, const Arg21& var21_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_, var14_, var15_, var16_, var17_, var18_, var19_, var20_, var21_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21, typename Arg22>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_, const Arg14& var14_, const Arg15& var15_, const Arg16& var16_, const Arg17& var17_, const Arg18& var18_, const Arg19& var19_, const Arg20& var20_, const Arg21& var21_, const Arg22& var22_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_, var14_, var15_, var16_, var17_, var18_, var19_, var20_, var21_, var22_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21, typename Arg22, typename Arg23>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_, const Arg14& var14_, const Arg15& var15_, const Arg16& var16_, const Arg17& var17_, const Arg18& var18_, const Arg19& var19_, const Arg20& var20_, const Arg21& var21_, const Arg22& var22_, const Arg23& var23_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_, var14_, var15_, var16_, var17_, var18_, var19_, var20_, var21_, var22_, var23_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21, typename Arg22, typename Arg23, typename Arg24>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_, const Arg14& var14_, const Arg15& var15_, const Arg16& var16_, const Arg17& var17_, const Arg18& var18_, const Arg19& var19_, const Arg20& var20_, const Arg21& var21_, const Arg22& var22_, const Arg23& var23_, const Arg24& var24_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_, var14_, var15_, var16_, var17_, var18_, var19_, var20_, var21_, var22_, var23_, var24_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21, typename Arg22, typename Arg23, typename Arg24, typename Arg25>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_, const Arg14& var14_, const Arg15& var15_, const Arg16& var16_, const Arg17& var17_, const Arg18& var18_, const Arg19& var19_, const Arg20& var20_, const Arg21& var21_, const Arg22& var22_, const Arg23& var23_, const Arg24& var24_, const Arg25& var25_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_, var14_, var15_, var16_, var17_, var18_, var19_, var20_, var21_, var22_, var23_, var24_, var25_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21, typename Arg22, typename Arg23, typename Arg24, typename Arg25, typename Arg26>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_, const Arg14& var14_, const Arg15& var15_, const Arg16& var16_, const Arg17& var17_, const Arg18& var18_, const Arg19& var19_, const Arg20& var20_, const Arg21& var21_, const Arg22& var22_, const Arg23& var23_, const Arg24& var24_, const Arg25& var25_, const Arg26& var26_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_, var14_, var15_, var16_, var17_, var18_, var19_, var20_, var21_, var22_, var23_, var24_, var25_, var26_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21, typename Arg22, typename Arg23, typename Arg24, typename Arg25, typename Arg26, typename Arg27>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_, const Arg14& var14_, const Arg15& var15_, const Arg16& var16_, const Arg17& var17_, const Arg18& var18_, const Arg19& var19_, const Arg20& var20_, const Arg21& var21_, const Arg22& var22_, const Arg23& var23_, const Arg24& var24_, const Arg25& var25_, const Arg26& var26_, const Arg27& var27_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_, var14_, var15_, var16_, var17_, var18_, var19_, var20_, var21_, var22_, var23_, var24_, var25_, var26_, var27_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21, typename Arg22, typename Arg23, typename Arg24, typename Arg25, typename Arg26, typename Arg27, typename Arg28>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_, const Arg14& var14_, const Arg15& var15_, const Arg16& var16_, const Arg17& var17_, const Arg18& var18_, const Arg19& var19_, const Arg20& var20_, const Arg21& var21_, const Arg22& var22_, const Arg23& var23_, const Arg24& var24_, const Arg25& var25_, const Arg26& var26_, const Arg27& var27_, const Arg28& var28_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_, var14_, var15_, var16_, var17_, var18_, var19_, var20_, var21_, var22_, var23_, var24_, var25_, var26_, var27_, var28_).c_str());
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6, typename Arg7, typename Arg8, typename Arg9, typename Arg10, typename Arg11, typename Arg12, typename Arg13, typename Arg14, typename Arg15, typename Arg16, typename Arg17, typename Arg18, typename Arg19, typename Arg20, typename Arg21, typename Arg22, typename Arg23, typename Arg24, typename Arg25, typename Arg26, typename Arg27, typename Arg28, typename Arg29>
int _CPPLogPrintf( const std::string & fac, int level, const std::string & fs,const Arg0& var0_, const Arg1& var1_, const Arg2& var2_, const Arg3& var3_, const Arg4& var4_, const Arg5& var5_, const Arg6& var6_, const Arg7& var7_, const Arg8& var8_, const Arg9& var9_, const Arg10& var10_, const Arg11& var11_, const Arg12& var12_, const Arg13& var13_, const Arg14& var14_, const Arg15& var15_, const Arg16& var16_, const Arg17& var17_, const Arg18& var18_, const Arg19& var19_, const Arg20& var20_, const Arg21& var21_, const Arg22& var22_, const Arg23& var23_, const Arg24& var24_, const Arg25& var25_, const Arg26& var26_, const Arg27& var27_, const Arg28& var28_, const Arg29& var29_) {
  return _LogPrintf( fac.c_str(), level, "%s", Tools::format( fs,var0_, var1_, var2_, var3_, var4_, var5_, var6_, var7_, var8_, var9_, var10_, var11_, var12_, var13_, var14_, var15_, var16_, var17_, var18_, var19_, var20_, var21_, var22_, var23_, var24_, var25_, var26_, var27_, var28_, var29_).c_str());
}


#else

  template <typename... Args> int _CPPLogPrintf(  const std::string & fac, int level, const std::string & format, Args... args )
  {
    std::vector<Tools::Format2::BaseArg*> v_args;

    Tools::Format2::add_args( v_args, args... );

    Tools::Format2::Format2 f2( format, v_args );

    for( auto x: v_args )
      {
        delete x;
      }

    return _LogPrintf( fac.c_str(), level, "%s",  f2.get_string().c_str() );
  }

#endif

int _CPPLogPrintf( const std::string & fac, int level, std::string fs );

#endif


