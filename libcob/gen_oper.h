/*
 * Copyright (C) 2002-2007 Keisuke Nishida
 * Copyright (C) 2007 Roger While
  * Copyright (C) 2008-2020 Cobol-IT
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1,
 * or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; see the file COPYING.LIB.  If
 * not, write to the Free Software Foundation, 51 Franklin Street, Fifth Floor
 * Boston, MA 02110-1301 USA
 */

COB_PROTO_STATIC void COB_OPER_NAME(_u8_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_s8_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_u16_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_align_u16_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_s16_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_align_s16_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_u24_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_s24_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_u32_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_s32_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_u40_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_s40_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_u48_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_s48_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_u56_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_s56_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_u64_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_s64_binary) (unsigned char *p, const int val);

/*#ifndef     ALLOW_MISALIGNED*/
COB_PROTO_STATIC void COB_OPER_NAME(_align_u32_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_align_s32_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_align_u64_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(_align_s64_binary) (unsigned char *p, const int val);
/*#endif*/

COB_PROTO_STATIC void COB_OPER_NAME(swp_u16_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(swp_s16_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(swp_u24_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(swp_s24_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(swp_u32_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(swp_s32_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(swp_u40_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(swp_s40_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(swp_u48_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(swp_s48_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(swp_u56_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(swp_s56_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(swp_u64_binary) (unsigned char *p, const int val);
COB_PROTO_STATIC void COB_OPER_NAME(swp_s64_binary) (unsigned char *p, const int val);

#if defined(COB_LOCAL_INLINE) || defined(COB_LIB_INCLUDE)
COB_STATIC void                                         
COB_OPER_NAME(_u8_binary) (unsigned char *p, const int val)                  
{                                                       
    *p COB_OPER val;                                        
}                                               

COB_STATIC void                                         
COB_OPER_NAME(_s8_binary) (unsigned char *p, const int val)                  
{                                                       
    *(signed char *)p COB_OPER val;                         
}

COB_STATIC void                                         
COB_OPER_NAME(_u16_binary) (unsigned char *p, const int val)                  
{                                                       
    #ifdef ALLOW_MISALIGNED                             
    *(unsigned short MISALIGNED *)p COB_OPER val;           
    #else                                               
    unsigned char   *x;                                 
    unsigned short  n;                                  
                                                        
    x = (unsigned char *)&n;                            
    *x = *p;                                            
    *(x + 1) = *(p + 1);                                
    n COB_OPER val;                                         
    *p = *x;                                            
    *(p + 1) = *(x + 1);                                
    #endif                                              
}                                                       
                                                        
COB_STATIC void                                         
COB_OPER_NAME(_s16_binary) (unsigned char *p, const int val)                  
{                                                       
    #ifdef ALLOW_MISALIGNED                             
    *(short MISALIGNED *)p COB_OPER val;                    
    #else                                               
    unsigned char   *x;                                 
    short       n;                                      
                                                        
    x = (unsigned char *)&n;                            
    *x = *p;                                            
    *(x + 1) = *(p + 1);                                
    n COB_OPER val;                                         
    *p = *x;                                            
    *(p + 1) = *(x + 1);                                
    #endif                                              
}                                                       
                                                        
COB_STATIC void                                         
COB_OPER_NAME(_u24_binary) (unsigned char *p, const int val)                  
{                                                       
    unsigned char   *x;                                 
    unsigned int    n = 0;                              
                                                        
    #ifdef      WORDS_BIGENDIAN                         
    x = ((unsigned char *)&n) + 1;                      
    #else                                               
    x = (unsigned char *)&n;                            
    #endif                                              
    *x = *p;                                            
    *(x + 1) = *(p + 1);                                
    *(x + 2) = *(p + 2);                                
    n COB_OPER val;                                         
    *p = *x;                                            
    *(p + 1) = *(x + 1);                                
    *(p + 2) = *(x + 2);                                
}                                                       
                                                        
COB_STATIC void                                         
COB_OPER_NAME(_s24_binary) (unsigned char *p, const int val)                  
{                                                       
    unsigned char   *x;                                 
    int     n = 0;                                      
                                                        
    #ifdef      WORDS_BIGENDIAN                         
    x = (unsigned char *)&n;                            
    #else                                               
    x = ((unsigned char *)&n) + 1;                      
    #endif                                              
    *x = *p;                                            
    *(x + 1) = *(p + 1);                                
    *(x + 2) = *(p + 2);                                
    n >>= 8;    /* shift with sign */                   
    n COB_OPER val;                                         
    #ifdef      WORDS_BIGENDIAN                         
    x = ((unsigned char *)&n) + 1;                      
    #else                                               
    x = (unsigned char *)&n;                            
    #endif                                              
    *p = *x;                                            
    *(p + 1) = *(x + 1);                                
    *(p + 2) = *(x + 2);                                
}                                                       
                                                        
COB_STATIC void                                         
COB_OPER_NAME(_u32_binary) (unsigned char *p, const int val)                  
{                                                       
    #ifdef ALLOW_MISALIGNED                             
    *(unsigned int MISALIGNED *)p COB_OPER val;             
    #else                                               
    unsigned char   *x;                                 
    unsigned int    n;                                  
                                                        
    x = (unsigned char *)&n;                            
    *x = *p;                                            
    *(x + 1) = *(p + 1);                                
    *(x + 2) = *(p + 2);                                
    *(x + 3) = *(p + 3);                                
    n COB_OPER val;                                         
    *p = *x;                                            
    *(p + 1) = *(x + 1);                                
    *(p + 2) = *(x + 2);                                
    *(p + 3) = *(x + 3);                                
    #endif                                              
}                                                       
                                                        
COB_STATIC void                                         
COB_OPER_NAME(_s32_binary) (unsigned char *p, const int val)                  
{                                                       
    #ifdef ALLOW_MISALIGNED                             
    *(int MISALIGNED *)p COB_OPER val;                      
    #else                                               
    unsigned char   *x;                                 
    int     n;                                          
                                                        
    x = (unsigned char *)&n;                            
    *x = *p;                                            
    *(x + 1) = *(p + 1);                                
    *(x + 2) = *(p + 2);                                
    *(x + 3) = *(p + 3);                                
    n COB_OPER val;                                         
    *p = *x;                                            
    *(p + 1) = *(x + 1);                                
    *(p + 2) = *(x + 2);                                
    *(p + 3) = *(x + 3);                                
    #endif                                              
}                                                       
                                                        
COB_STATIC void                                         
COB_OPER_NAME(_u40_binary) (unsigned char *p, const int val)                  
{                                                       
    unsigned long long  n = 0;                          
    unsigned char       *x;                             
                                                        
    #ifdef      WORDS_BIGENDIAN                         
    x = ((unsigned char *)&n) + 3;                      
    #else                                               
    x = (unsigned char *)&n;                            
    #endif                                              
    *x = *p;                                            
    *(x + 1) = *(p + 1);                                
    *(x + 2) = *(p + 2);                                
    *(x + 3) = *(p + 3);                                
    *(x + 4) = *(p + 4);                                
    n COB_OPER val;                                         
    *p = *x;                                            
    *(p + 1) = *(x + 1);                                
    *(p + 2) = *(x + 2);                                
    *(p + 3) = *(x + 3);                                
    *(p + 4) = *(x + 4);                                
}                                                       
                                                        
COB_STATIC void                                         
COB_OPER_NAME(_s40_binary) (unsigned char *p, const int val)                 
{                                                       
    long long       n = 0;                              
    unsigned char       *x;                             
                                                        
    #ifdef      WORDS_BIGENDIAN                         
    x = (unsigned char *)&n;                            
    #else                                               
    x = ((unsigned char *)&n) + 3;                      
    #endif                                              
    *x = *p;                                            
    *(x + 1) = *(p + 1);                                
    *(x + 2) = *(p + 2);                                
    *(x + 3) = *(p + 3);                                
    *(x + 4) = *(p + 4);                                
    n >>= 24;   /* shift with sign */                   
    n COB_OPER val;                                         
    #ifdef      WORDS_BIGENDIAN                         
    x = ((unsigned char *)&n) + 3;                      
    #else                                               
    x = (unsigned char *)&n;                            
    #endif                                              
    *p = *x;                                            
    *(p + 1) = *(x + 1);                                
    *(p + 2) = *(x + 2);                                
    *(p + 3) = *(x + 3);                                
    *(p + 4) = *(x + 4);                                
}                                                       
                                                        
COB_STATIC void                                         
COB_OPER_NAME(_u48_binary) (unsigned char *p, const int val)                  
{                                                       
    unsigned long long  n = 0;                          
    unsigned char       *x;                             
                                                        
    #ifdef      WORDS_BIGENDIAN                         
    x = ((unsigned char *)&n) + 2;                      
    #else                                               
    x = (unsigned char *)&n;                            
    #endif                                              
    *x = *p;                                            
    *(x + 1) = *(p + 1);                                
    *(x + 2) = *(p + 2);                                
    *(x + 3) = *(p + 3);                                
    *(x + 4) = *(p + 4);                                
    *(x + 5) = *(p + 5);                                
    n COB_OPER val;                                         
    *p = *x;                                            
    *(p + 1) = *(x + 1);                                
    *(p + 2) = *(x + 2);                                
    *(p + 3) = *(x + 3);                                
    *(p + 4) = *(x + 4);                                
    *(p + 5) = *(x + 5);                                
}                                                       
                                                        
COB_STATIC void                                         
COB_OPER_NAME(_s48_binary) (unsigned char *p, const int val)                  
{                                                       
    long long       n = 0;                              
    unsigned char       *x;                             
                                                        
    #ifdef      WORDS_BIGENDIAN                         
    x = (unsigned char *)&n;                            
    #else                                               
    x = ((unsigned char *)&n) + 2;                      
    #endif                                              
    *x = *p;                                            
    *(x + 1) = *(p + 1);                                
    *(x + 2) = *(p + 2);                                
    *(x + 3) = *(p + 3);                                
    *(x + 4) = *(p + 4);                                
    *(x + 5) = *(p + 5);                                
    n >>= 16;   /* shift with sign */                   
    n COB_OPER val;                                         
    #ifdef      WORDS_BIGENDIAN                         
    x = ((unsigned char *)&n) + 2;                      
    #else                                               
    x = (unsigned char *)&n;                            
    #endif                                              
    *p = *x;                                            
    *(p + 1) = *(x + 1);                                
    *(p + 2) = *(x + 2);                                
    *(p + 3) = *(x + 3);                                
    *(p + 4) = *(x + 4);                                
    *(p + 5) = *(x + 5);                                
}                                                       
                                                        
COB_STATIC void                                         
COB_OPER_NAME(_u56_binary) (unsigned char *p, const int val)                  
{                                                       
    unsigned long long  n = 0;                          
    unsigned char       *x;                             
                                                        
    #ifdef      WORDS_BIGENDIAN                         
    x = ((unsigned char *)&n) + 1;                      
    #else                                               
    x = (unsigned char *)&n;                            
    #endif                                              
    *x = *p;                                            
    *(x + 1) = *(p + 1);                                
    *(x + 2) = *(p + 2);                                
    *(x + 3) = *(p + 3);                                
    *(x + 4) = *(p + 4);                                
    *(x + 5) = *(p + 5);                                
    *(x + 6) = *(p + 6);                                
    n COB_OPER val;                                         
    *p = *x;                                            
    *(p + 1) = *(x + 1);                                
    *(p + 2) = *(x + 2);                                
    *(p + 3) = *(x + 3);                                
    *(p + 4) = *(x + 4);                                
    *(p + 5) = *(x + 5);                                
    *(p + 6) = *(x + 6);                                
}                                                       
                                                        
COB_STATIC void                                         
COB_OPER_NAME(_s56_binary) (unsigned char *p, const int val)                  
{                                                       
    long long       n = 0;                              
    unsigned char       *x;                             
                                                        
    #ifdef      WORDS_BIGENDIAN                         
    x = (unsigned char *)&n;                            
    #else                                               
    x = ((unsigned char *)&n) + 1;                      
    #endif                                              
    *x = *p;                                            
    *(x + 1) = *(p + 1);                                
    *(x + 2) = *(p + 2);                                
    *(x + 3) = *(p + 3);                                
    *(x + 4) = *(p + 4);                                
    *(x + 5) = *(p + 5);                                
    *(x + 6) = *(p + 6);                                
    n >>= 8;    /* shift with sign */                   
    n COB_OPER val;                                         
    #ifdef      WORDS_BIGENDIAN                         
    x = ((unsigned char *)&n) + 1;                      
    #else                                               
    x = (unsigned char *)&n;                            
    #endif                                              
    *p = *x;                                            
    *(p + 1) = *(x + 1);                                
    *(p + 2) = *(x + 2);                                
    *(p + 3) = *(x + 3);                                
    *(p + 4) = *(x + 4);                                
    *(p + 5) = *(x + 5);                                
    *(p + 6) = *(x + 6);                                
}                                                       
                                                        
COB_STATIC void                                         
COB_OPER_NAME(_u64_binary) (unsigned char *p, const int val)                  
{                                                       
    #ifdef ALLOW_MISALIGNED                             
    *(unsigned long long MISALIGNED *)p COB_OPER val;       
    #else                                               
    unsigned char       *x;                             
    unsigned long long  n;                              
                                                        
    x = (unsigned char *)&n;                            
    *x = *p;                                            
    *(x + 1) = *(p + 1);                                
    *(x + 2) = *(p + 2);                                
    *(x + 3) = *(p + 3);                                
    *(x + 4) = *(p + 4);                                
    *(x + 5) = *(p + 5);                                
    *(x + 6) = *(p + 6);                                
    *(x + 7) = *(p + 7);                                
    n COB_OPER val;                                         
    *p = *x;                                            
    *(p + 1) = *(x + 1);                                
    *(p + 2) = *(x + 2);                                
    *(p + 3) = *(x + 3);                                
    *(p + 4) = *(x + 4);                                
    *(p + 5) = *(x + 5);                                
    *(p + 6) = *(x + 6);                                
    *(p + 7) = *(x + 7);                                
    #endif                                              
}                                                       
                                                        
COB_STATIC void                                         
COB_OPER_NAME(_s64_binary) (unsigned char *p, const int val)                  
{                                                       
    #ifdef ALLOW_MISALIGNED                             
    *(long long MISALIGNED *)p COB_OPER val;                
    #else                                               
    unsigned char       *x;                             
    long long       n;                                  
                                                        
    x = (unsigned char *)&n;                            
    *x = *p;                                            
    *(x + 1) = *(p + 1);                                
    *(x + 2) = *(p + 2);                                
    *(x + 3) = *(p + 3);                                
    *(x + 4) = *(p + 4);                                
    *(x + 5) = *(p + 5);                                
    *(x + 6) = *(p + 6);                                
    *(x + 7) = *(p + 7);                                
    n COB_OPER val;                                         
    *p = *x;                                            
    *(p + 1) = *(x + 1);                                
    *(p + 2) = *(x + 2);                                
    *(p + 3) = *(x + 3);                                
    *(p + 4) = *(x + 4);                                
    *(p + 5) = *(x + 5);                                
    *(p + 6) = *(x + 6);                                
    *(p + 7) = *(x + 7);                                
    #endif                                              
}                                                       

 
/* Binary swapped add */
COB_STATIC void
COB_OPER_NAME(swp_u16_binary) (unsigned char *p, const int val)
{
    unsigned short  n;

#ifdef ALLOW_MISALIGNED
    n = COB_BSWAP_16 (*(unsigned short MISALIGNED *)p);
    n COB_OPER val;
    *(unsigned short MISALIGNED *)p = COB_BSWAP_16(n);
#else
    n = (unsigned short)((p[0] << 8) | p[1]);
    n COB_OPER val;
    p[0] = (unsigned char)(n >> 8);
    p[1] = (unsigned char)n;
#endif
}

COB_STATIC void
COB_OPER_NAME(swp_s16_binary) (unsigned char *p, const int val)
{
    short       n;

#ifdef ALLOW_MISALIGNED
    n = COB_BSWAP_16 (*(short MISALIGNED *)p);
    n COB_OPER val;
    *(short MISALIGNED *)p = COB_BSWAP_16(n);
#else
    n = (short)((p[0] << 8) | p[1]);
    n COB_OPER val;
    p[0] = (unsigned char)(n >> 8);
    p[1] = (unsigned char)n;
#endif
}

COB_STATIC void
COB_OPER_NAME(swp_u24_binary) (unsigned char *p, const int val)
{
    unsigned char   *x;
    unsigned int    n = 0;

    x = (unsigned char *)&n;
    *x = *(p + 2);
    *(x + 1) = *(p + 1);
    *(x + 2) = *p;
    n COB_OPER val;
    *p = *(x + 2);
    *(p + 1) = *(x + 1);
    *(p + 2) = *x;
}

COB_STATIC void
COB_OPER_NAME(swp_s24_binary) (unsigned char *p, const int val)
{
    unsigned char   *x;
    int     n = 0;

    x = ((unsigned char *)&n) + 1;
    *x = *(p + 2);
    *(x + 1) = *(p + 1);
    *(x + 2) = *p;
    n >>= 8;    /* shift with sign */
    n COB_OPER val;
    x = (unsigned char *)&n;
    *p = *(x + 2);
    *(p + 1) = *(x + 1);
    *(p + 2) = *x;
}

COB_STATIC void
COB_OPER_NAME(swp_u32_binary) (unsigned char *p, const int val)
{
    unsigned int    n;

#ifdef ALLOW_MISALIGNED
    n = COB_BSWAP_32 (*(unsigned int MISALIGNED *)p);
    n COB_OPER val;
    *(unsigned int MISALIGNED *)p = COB_BSWAP_32(n);
#else
    n = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
    n COB_OPER val;
    *p++ = (unsigned char)(n >> 24);
    *p++ = (unsigned char)(n >> 16);
    *p++ = (unsigned char)(n >> 8);
    *p = (unsigned char)n;
#endif
}

COB_STATIC void
COB_OPER_NAME(swp_s32_binary) (unsigned char *p, const int val)
{
    int     n;

#ifdef ALLOW_MISALIGNED
    n = COB_BSWAP_32 (*(int MISALIGNED *)p);
    n COB_OPER val;
    *(int MISALIGNED *)p = COB_BSWAP_32(n);
#else
    n = (int)((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
    n COB_OPER val;
    *p++ = (unsigned char)(n >> 24);
    *p++ = (unsigned char)(n >> 16);
    *p++ = (unsigned char)(n >> 8);
    *p = (unsigned char)n;
#endif
}

COB_STATIC void
COB_OPER_NAME(swp_u40_binary) (unsigned char *p, const int val)
{
    unsigned long long  n = 0;
    unsigned char       *x;

    x = (unsigned char *)&n;
    *x = *(p + 4);
    *(x + 1) = *(p + 3);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 1);
    *(x + 4) = *p;
    n COB_OPER val;
    *p = *(x + 4);
    *(p + 1) = *(x + 3);
    *(p + 2) = *(x + 2);
    *(p + 3) = *(x + 1);
    *(p + 4) = *x;
}

COB_STATIC void
COB_OPER_NAME(swp_s40_binary) (unsigned char *p, const int val)
{
    long long       n = 0;
    unsigned char       *x;

    x = ((unsigned char *)&n) + 3;
    *x = *(p + 4);
    *(x + 1) = *(p + 3);
    *(x + 2) = *(p + 2);
    *(x + 3) = *(p + 1);
    *(x + 4) = *p;
    n >>= 24;   /* shift with sign */
    n COB_OPER val;
    x = (unsigned char *)&n;
    *p = *(x + 4);
    *(p + 1) = *(x + 3);
    *(p + 2) = *(x + 2);
    *(p + 3) = *(x + 1);
    *(p + 4) = *x;
}

COB_STATIC void
COB_OPER_NAME(swp_u48_binary) (unsigned char *p, const int val)
{
    unsigned long long  n = 0;
    unsigned char       *x;

    x = (unsigned char *)&n;
    *x = *(p + 5);
    *(x + 1) = *(p + 4);
    *(x + 2) = *(p + 3);
    *(x + 3) = *(p + 2);
    *(x + 4) = *(p + 1);
    *(x + 5) = *p;
    n COB_OPER val;
    *p = *(x + 5);
    *(p + 1) = *(x + 4);
    *(p + 2) = *(x + 3);
    *(p + 3) = *(x + 2);
    *(p + 4) = *(x + 1);
    *(p + 5) = *x;
}

COB_STATIC void
COB_OPER_NAME(swp_s48_binary) (unsigned char *p, const int val)
{
    long long       n = 0;
    unsigned char       *x;

    x = ((unsigned char *)&n) + 2;
    *x = *(p + 5);
    *(x + 1) = *(p + 4);
    *(x + 2) = *(p + 3);
    *(x + 3) = *(p + 2);
    *(x + 4) = *(p + 1);
    *(x + 5) = *p;
    n >>= 16;   /* shift with sign */
    n COB_OPER val;
    x = (unsigned char *)&n;
    *p = *(x + 5);
    *(p + 1) = *(x + 4);
    *(p + 2) = *(x + 3);
    *(p + 3) = *(x + 2);
    *(p + 4) = *(x + 1);
    *(p + 5) = *x;
}

COB_STATIC void
COB_OPER_NAME(swp_u56_binary) (unsigned char *p, const int val)
{
    unsigned long long  n = 0;
    unsigned char       *x;

    x = (unsigned char *)&n;
    *x = *(p + 6);
    *(x + 1) = *(p + 5);
    *(x + 2) = *(p + 4);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 2);
    *(x + 5) = *(p + 1);
    *(x + 6) = *p;
    n COB_OPER val;
    *p = *(x + 6);
    *(p + 1) = *(x + 5);
    *(p + 2) = *(x + 4);
    *(p + 3) = *(x + 3);
    *(p + 4) = *(x + 2);
    *(p + 5) = *(x + 1);
    *(p + 6) = *x;
}

COB_STATIC void
COB_OPER_NAME(swp_s56_binary) (unsigned char *p, const int val)
{
    long long       n = 0;
    unsigned char       *x;

    x = ((unsigned char *)&n) + 1;
    *x = *(p + 6);
    *(x + 1) = *(p + 5);
    *(x + 2) = *(p + 4);
    *(x + 3) = *(p + 3);
    *(x + 4) = *(p + 2);
    *(x + 5) = *(p + 1);
    *(x + 6) = *p;
    n >>= 8;    /* shift with sign */
    n COB_OPER val;
    x = (unsigned char *)&n;
    *p = *(x + 6);
    *(p + 1) = *(x + 5);
    *(p + 2) = *(x + 4);
    *(p + 3) = *(x + 3);
    *(p + 4) = *(x + 2);
    *(p + 5) = *(x + 1);
    *(p + 6) = *x;
}

COB_STATIC void
COB_OPER_NAME(swp_u64_binary) (unsigned char *p, const int val)
{
#ifdef ALLOW_MISALIGNED
    unsigned long long  n;

    n = COB_BSWAP_64 (*(unsigned long long MISALIGNED *)p);
    n COB_OPER val;
    *(unsigned long long MISALIGNED *)p = COB_BSWAP_64(n);
#else
    size_t  i;
    union {
        unsigned long long  n;
        unsigned char       c[8];
    } u;

    for ( i = 0; i < 8; ++i ) {
        u.c[7-i] = p[i];
    }
    u.n COB_OPER val;
    for ( i = 0; i < 8; ++i ) {
        p[i] = u.c[7-i];
    }
#endif
}

COB_STATIC void
COB_OPER_NAME(swp_s64_binary) (unsigned char *p, const int val)
{
#ifdef ALLOW_MISALIGNED
    long long   n;

    n = COB_BSWAP_64 (*(long long MISALIGNED *)p);
    n COB_OPER val;
    *(long long MISALIGNED *)p = COB_BSWAP_64(n);
#else
    size_t  i;
    union {
        long long   n;
        unsigned char   c[8];
    } u;

    for ( i = 0; i < 8; ++i ) {
        u.c[7-i] = p[i];
    }
    u.n COB_OPER val;
    for ( i = 0; i < 8; ++i ) {
        p[i] = u.c[7-i];
    }
#endif
}

/*#ifndef     ALLOW_MISALIGNED */

COB_STATIC void
COB_OPER_NAME(_align_u16_binary) (unsigned char *p, const int val)
{
    *(unsigned short MISALIGNED *)p COB_OPER val;
}

COB_STATIC void
COB_OPER_NAME(_align_s16_binary) (unsigned char *p, const int val)
{
    *(short MISALIGNED *)p COB_OPER val;
}

COB_STATIC void
COB_OPER_NAME(_align_u32_binary) (unsigned char *p, const int val)
{
    *(unsigned int MISALIGNED *)p COB_OPER val;
}

COB_STATIC void
COB_OPER_NAME(_align_s32_binary) (unsigned char *p, const int val)
{
    *(int MISALIGNED *)p COB_OPER val;
}

COB_STATIC void
COB_OPER_NAME(_align_u64_binary) (unsigned char *p, const int val)
{
    *(unsigned long long MISALIGNED *)p COB_OPER val;
}

COB_STATIC void
COB_OPER_NAME(_align_s64_binary) (unsigned char *p, const int val)
{
    *(long long MISALIGNED *)p COB_OPER val;
}
/*#endif*/
#endif
