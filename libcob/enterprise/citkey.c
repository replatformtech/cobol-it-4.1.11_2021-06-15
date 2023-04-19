/*
   * Copyright (C) 2008-2020 Cobol-IT
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 51 Franklin Street, Fifth Floor
 * Boston, MA 02110-1301 USA
 */
#ifdef _MSC_VER
#  include <windows.h>
#endif
#include "config.h"
#include "defaults.h"
#include "globaldefine.h"
#include "defaults.h"
#define _GNU_SOURCE / * glibc2 needs this */
#include "stdio.h"
#include "stdlib.h"
#include <ctype.h>
#include <string.h>
#include "xmlb.h"
#include "newmeta.h"
#include "citkey.h"
#include "sha1.h"
#include "time.h"
#include "gmp.h"

#ifdef HAVE_STRINGS_H
#  include <strings.h>
#endif
#ifndef HAVE_STRCASESTR
const char*
strcasestr(const char *haystack, const char *needle)
{
   const char *p, *startn = 0, *np = 0;

   for (p = haystack; *p; p++)
   {
      if (np)
      {
         if (toupper(*p) == toupper(*np))
         {
            if (!*++np) return startn;
         }
         else np = 0;
      }
      else if (toupper(*p) == toupper(*needle))
      {
         np = needle + 1;
         startn = p;
      }
   }

   return 0;
}
#endif

#ifndef HAVE_STRNCASECMP
#  ifdef _WIN32
static int
strncasecmp(const char *_Str1, const char *_Str2,  size_t _MaxCount)
{
   return (_strnicmp(_Str1, _Str2, _MaxCount));
}
#  else
static int
strncasecmp(char *s1, char *s2, size_t n)
{
   if (n == 0) return 0;

   while (n-- != 0 && tolower(*s1) == tolower(*s2))
   {
      if (n == 0 || *s1 == '\0' || *s2 == '\0') break;
      s1++;
      s2++;
   }

   return tolower(*(unsigned char *)s1) - tolower(*(unsigned char *)s2);
}
#  endif
#endif

#ifdef _WIN32
#  include <time.h>

static int
strcasecmp(const char *_Str1, const char *_Str2)
{
   return (_stricmp(_Str1, _Str2));
}
#endif

/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E          0x01
#define ALT_O          0x02
//#define LEGAL_ALT(x)       { if (alt_format & ~(x)) return (0); }
#define LEGAL_ALT(x)       { ; }
#define TM_YEAR_BASE   (1900)

static   int conv_num(const char **, int *, int, int);

static const char *day[7] = {
   "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
   "Friday", "Saturday"
};
static const char *abday[7] = {
   "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
static const char *mon[12] = {
   "January", "February", "March", "April", "May", "June", "July",
   "August", "September", "October", "November", "December"
};
static const char *abmon[12] = {
   "Jan", "Feb", "Mar", "Apr", "May", "Jun",
   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
static const char *am_pm[2] = {
   "AM", "PM"
};


char* cob_strptime(const char *buf, const char *fmt, struct tm *tm)
{
   char c;
   const char *bp;
   size_t len = 0;
   int alt_format, i, split_year = 0;

   bp = buf;

   while ((c = *fmt) != '\0')
   {
      /* Clear `alternate' modifier prior to new conversion. */
      alt_format = 0;

      /* Eat up white-space. */
      if (isspace(c))
      {
         while (isspace(*bp)) bp++;

         fmt++;
         continue;
      }

      if ((c = *fmt++) != '%') goto literal;


    again:        switch (c = *fmt++)
      {
         case '%': /* "%%" is converted to "%". */
       literal:
            if (c != *bp++) return (0);
            break;

            /*
             * "Alternative" modifiers. Just set the appropriate flag
              * and start over again.
               */
         case 'E': /* "%E?" alternative conversion modifier. */
            LEGAL_ALT(0);
            alt_format |= ALT_E;
            goto again;

         case 'O': /* "%O?" alternative conversion modifier. */
            LEGAL_ALT(0);
            alt_format |= ALT_O;
            goto again;

            /*
             * "Complex" conversion rules, implemented through recursion.
             */
         case 'c': /* Date and time, using the locale's format. */
            LEGAL_ALT(ALT_E);
            if (!(bp = cob_strptime(bp, "%x %X", tm))) return (0);
            break;

         case 'D': /* The date as "%m/%d/%y". */
            LEGAL_ALT(0);
            if (!(bp = cob_strptime(bp, "%m/%d/%y", tm))) return (0);
            break;

         case 'R': /* The time as "%H:%M". */
            LEGAL_ALT(0);
            if (!(bp = cob_strptime(bp, "%H:%M", tm))) return (0);
            break;

         case 'r': /* The time in 12-hour clock representation. */
            LEGAL_ALT(0);
            if (!(bp = cob_strptime(bp, "%I:%M:%S %p", tm))) return (0);
            break;

         case 'T': /* The time as "%H:%M:%S". */
            LEGAL_ALT(0);
            if (!(bp = cob_strptime(bp, "%H:%M:%S", tm))) return (0);
            break;

         case 'X': /* The time, using the locale's format. */
            LEGAL_ALT(ALT_E);
            if (!(bp = cob_strptime(bp, "%H:%M:%S", tm))) return (0);
            break;

         case 'x': /* The date, using the locale's format. */
            LEGAL_ALT(ALT_E);
            if (!(bp = cob_strptime(bp, "%m/%d/%y", tm))) return (0);
            break;

            /*
             * "Elementary" conversion rules.
             */
         case 'A': /* The day of week, using the locale's form. */
         case 'a':
            LEGAL_ALT(0);
            for (i = 0; i < 7; i++)
            {
               /* Full name. */
               len = strlen(day[i]);
               if (strncasecmp((char *)(day[i]), (char *)bp, len) == 0) break;

               /* Abbreviated name. */
               len = strlen(abday[i]);
               if (strncasecmp((char *)(abday[i]), (char *)bp, len) == 0) break;
            }

            /* Nothing matched. */
            if (i == 7) return (0);

            tm->tm_wday = i;
            bp += len;
            break;

         case 'B': /* The month, using the locale's form. */
         case 'b':
         case 'h':
            LEGAL_ALT(0);
            for (i = 0; i < 12; i++)
            {
               /* Full name. */

               len = strlen(mon[i]);
               if (strncasecmp((char *)(mon[i]), (char *)bp, len) == 0) break;

               /* Abbreviated name. */
               len = strlen(abmon[i]);
               if (strncasecmp((char *)(abmon[i]), (char *)bp, len) == 0) break;
            }

            /* Nothing matched. */
            if (i == 12) return (0);

            tm->tm_mon = i;
            bp += len;
            break;

         case 'C': /* The century number. */
            LEGAL_ALT(ALT_E);
            if (!(conv_num(&bp, &i, 0, 99))) return (0);

            if (split_year)
            {
               tm->tm_year = (tm->tm_year % 100) + (i * 100);
            }
            else
            {
               tm->tm_year = i * 100;
               split_year = 1;
            }
            break;

         case 'd': /* The day of month. */
         case 'e':
            LEGAL_ALT(ALT_O);
            if (!(conv_num(&bp, &tm->tm_mday, 1, 31))) return (0);
            break;

         case 'k': /* The hour (24-hour clock representation). */
            LEGAL_ALT(0);
            /* FALLTHROUGH */
         case 'H':
            LEGAL_ALT(ALT_O);
            if (!(conv_num(&bp, &tm->tm_hour, 0, 23))) return (0);
            break;

         case 'l': /* The hour (12-hour clock representation). */
            LEGAL_ALT(0);
            /* FALLTHROUGH */
         case 'I':
            LEGAL_ALT(ALT_O);
            if (!(conv_num(&bp, &tm->tm_hour, 1, 12))) return (0);
            if (tm->tm_hour == 12) tm->tm_hour = 0;
            break;

         case 'j': /* The day of year. */
            LEGAL_ALT(0);
            if (!(conv_num(&bp, &i, 1, 366))) return (0);
            tm->tm_yday = i - 1;
            break;

         case 'M': /* The minute. */
            LEGAL_ALT(ALT_O);
            if (!(conv_num(&bp, &tm->tm_min, 0, 59))) return (0);
            break;

         case 'm': /* The month. */
            LEGAL_ALT(ALT_O);
            if (!(conv_num(&bp, &i, 1, 12))) return (0);
            tm->tm_mon = i - 1;
            break;

//            case 'p': /* The locale's equivalent of AM/PM. */
//                LEGAL_ALT(0);
//                /* AM? */
//                if (strcasecmp(am_pm[0], bp) == 0)
//                {
//                    if (tm->tm_hour > 11)
//                        return (0);
//
//                    bp += strlen(am_pm[0]);
//                    break;
//                }
//                /* PM? */
//                else if (strcasecmp(am_pm[1], bp) == 0)
//                {
//                    if (tm->tm_hour > 11)
//                        return (0);
//
//                    tm->tm_hour += 12;
//                    bp += strlen(am_pm[1]);
//                    break;
//                }
//
//                /* Nothing matched. */
//                return (0);

         case 'S': /* The seconds. */
            LEGAL_ALT(ALT_O);
            if (!(conv_num(&bp, &tm->tm_sec, 0, 61))) return (0);
            break;

         case 'U': /* The week of year, beginning on sunday. */
         case 'W': /* The week of year, beginning on monday. */
            LEGAL_ALT(ALT_O);
            /*
             * XXX This is bogus, as we can not assume any valid
             * information present in the tm structure at this
             * point to calculate a real value, so just check the
             * range for now.
             */
            if (!(conv_num(&bp, &i, 0, 53))) return (0);
            break;

         case 'w': /* The day of week, beginning on sunday. */
            LEGAL_ALT(ALT_O);
            if (!(conv_num(&bp, &tm->tm_wday, 0, 6))) return (0);
            break;

         case 'Y': /* The year. */
            LEGAL_ALT(ALT_E);
            if (!(conv_num(&bp, &i, 0, 9999))) return (0);

            tm->tm_year = i - TM_YEAR_BASE;
            break;

         case 'y': /* The year within 100 years of the epoch. */
            LEGAL_ALT(ALT_E | ALT_O);
            if (!(conv_num(&bp, &i, 0, 99))) return (0);

            if (split_year)
            {
               tm->tm_year = ((tm->tm_year / 100) * 100) + i;
               break;
            }
            split_year = 1;
            if (i <= 68) tm->tm_year = i + 2000 - TM_YEAR_BASE;
            else tm->tm_year = i + 1900 - TM_YEAR_BASE;
            break;

            /*
              * Miscellaneous conversions.
              */
         case 'n': /* Any kind of white-space. */
         case 't':
            LEGAL_ALT(0);
            while (isspace(*bp)) bp++;
            break;


         default: /* Unknown/unsupported conversion. */
            return (0);
      }


   }

   /* LINTED functional specification */
   return ((char *)bp);
}

static int conv_num(const char **buf, int *dest, int llim, int ulim)
{
   int result = 0;

   /* The limit also determines the number of valid digits. */
   int rulim = ulim;

   if (**buf < '0' || **buf > '9') return (0);

   do
   {
      result *= 10;
      result += *(*buf)++ - '0';
      rulim /= 10;
   }
   while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');

   if (result < llim || result > ulim) return (0);

   *dest = result;
   return (1);
}

#ifdef WITH_MF_KEYS

static void
cob_extract_version(char *buffer, int *v1, int *v2, int *v3)
{
   char *p;
   char *saveptr = NULL;

   *v1 = *v2 = *v3 = 0;

   p = strtok_r(buffer, ".-", &saveptr);
   if (p)
   {
      *v1 = atoi(p);
      p = strtok_r(NULL, ".-", &saveptr);
      if (p)
      {
         *v2 = atoi(p);
         p = strtok_r(NULL, ".-", &saveptr);
         if (p)
         {
            *v3 = atoi(p);
         }
      }
   }

}

typedef struct {
    license *citlicense;
    char *fname;
    S_XMLB_CONTEXT *ctxt;
} cit_license_T;
static cit_license_T citlicense_array[256] = { { NULL, NULL } };
static char *current_fname = (char *)"";
static SHA1Context sha1;
static const char *kMod = "16530158131766268849717210432155184653041673775168495224274761838024469351342899169601340943332530648236499519268023411890349236847495635833670768445619720661700457012076391700573961497812302352379879259081264735192358152505829143870105104761368675590595982243759282580006128609875894176572790900988816373289338485026574703908544226586516915377326249648575806180533753433035150518773731346075028337848992181609008955875759813315734194220586186327436296659774722315563184744939303987906780988268339282876341427632560711422531067858230032247525271658017808312193431108401982786545439200909224538080196736253601997018167";
static const char *kExp = "65537";
#define DEFAULT_KEY_NAME "citlicense*.xml"

static void add_to_sha1(const char *name, char *value)
{
   if (value)
   {
      if (name)
      {
         SHA1Input(&sha1, (const unsigned char *)name, (unsigned)strlen(name));
         SHA1Input(&sha1, (const unsigned char *)"\b", (unsigned)1);
      }
      SHA1Input(&sha1, (const unsigned char *)value, (unsigned)strlen(value));
      SHA1Input(&sha1, (const unsigned char *)"\b", (unsigned)1);
   }
}

static void mp_int_to_bin(unsigned char *buffer, unsigned length, MP_INT *x)
{

   unsigned char *dst = buffer + length - 1;

   unsigned size = mpz_size(x);
   unsigned i, j;

   memset(buffer, 0, length);

   for (i = 0; i < size; ++i)
   {
      mp_limb_t limb = mpz_getlimbn(x, i);
      for (j = 0; length && j < sizeof(mp_limb_t); ++j)
      {
         *dst-- = (limb & 0xff);
         limb >>= 8;
         --length;
      }
   }

}

static char inttohex(unsigned int c)
{
   static char *v = (char *)"0123456789ABCDEF";
   return v[c];
}

/*
static void bytesToHex(unsigned char *bytes, int len) {
    int i;
    for (i=0; i<len; ++i) {
        printf("%c", inttohex((bytes[i] >> 4) & 0xf));
        printf("%c", inttohex(bytes[i] & 0xf));
    }
    printf("\n");
}
*/
static void dump_license(license *citlicense);
static int test_gmp(unsigned char *sha1_digest, char *signatureStr)
{
   int ret = 0, i, j;
   MP_INT sig, mod, exp, res;
   unsigned char bytes[255];
   char tmpsig[513];
   static const unsigned char kSha1WithRsaId[] = {
      0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E, 0x03, 0x02, 0x1A, 0x05, 0x00, 0x04, 0x14
   };
   static const unsigned kSha1WithRsaIdSize = sizeof(kSha1WithRsaId) / sizeof(unsigned char);

   mpz_init(&mod);
   mpz_init(&exp);
   mpz_init(&sig);
   mpz_init(&res);

   if (mpz_set_str(&mod, kMod, 10)) goto cleanup;
   if (mpz_set_str(&exp, kExp, 10)) goto cleanup;
   if (mpz_size(&mod) * sizeof(mp_limb_t) != 256) goto cleanup;

   /*because I'm stupid*/
   for (i = 0; i < 256; ++i)
   {
      tmpsig[i << 1] = signatureStr[(i << 1) + 1];
      tmpsig[(i << 1) + 1] = signatureStr[i << 1];
   }
   tmpsig[512] = 0;

   if (mpz_set_str(&sig, tmpsig, 16)) goto cleanup;


   mpz_powm(&res, &sig, &exp, &mod);

   mp_int_to_bin(bytes, 255, &res);

   /* 
   printf("decript :");
   bytesToHex(bytes, 255);
   printf("hash    :");
   bytesToHex(sha1_digest, 20); 
   */
/*
    SHA1 + PKCS#1 padding:
    01 FF FF FF ... FF FF FF 00 ID DIGEST
*/

   i = 0;
   if (bytes[i++] != 0x01) goto cleanup;

   for (; i < 255; ++i)
   {
      if (bytes[i] == 0x00) break;
      if (bytes[i] != 0xff) goto cleanup;
   }
   if (i == 255) goto cleanup;
   ++i;


   for (j = 0; j < kSha1WithRsaIdSize && i < 255; ++i, ++j)
   {
      if (bytes[i] != kSha1WithRsaId[j]) goto cleanup;
   }

   if (i != 255 - 20) goto cleanup; /*20 bytes for the sha1 digest*/



   ret = memcmp(bytes + i, sha1_digest, 20) == 0;

 cleanup:
   mpz_clear(&res);
   mpz_clear(&mod);
   mpz_clear(&exp);
   mpz_clear(&sig);

   return ret;
}

static int cmp_e(cit_runtime_t *const context, const void *p1, const void *p2)
{
   const struct _e  *e1 = p1;
   const struct _e  *e2 = p2;

   return strcmp(e1->aValue, e2->aValue);
}

#define KEY_UNDEFINED 65535
#define KEY_Ok 1
#define KEY_ERR_NO_FILE        0
#define KEY_ERR_WRONG_FORMAT   -2
#define KEY_ERR_EXPIRED        -3
#define KEY_ERR_EXPIRED_IGNORE  3
#define KEY_ERR_WRONG_PLATFORM -4
#define KEY_ERR_WRONG_PRODUCT  -5
#define KEY_ERR_WRONG_VERSION  -6
#define KEY_NOT_CHECK_YET      -10
char *lic_fname[255] = { NULL };
int lic_fname_cnt = 0;
#include "readdir_win.h"

static void add_lic_fname(char *directory)
{
   DIR *d;
   struct dirent *dir;
   if (!directory) return;
   d = opendir(directory);
   if (d)
   {
      int l = strlen(DEFAULT_KEY_NAME);
      while (lic_fname_cnt < 255 && (dir = readdir(d)) != NULL)
      {
         int dl = strlen(dir->d_name);
         if (strmatch(dir->d_name, (char *)DEFAULT_KEY_NAME, dl, l))
         {
            lic_fname[lic_fname_cnt] = malloc(strlen(directory) + dl + 4);
            sprintf(lic_fname[lic_fname_cnt], "%s/%s", directory, dir->d_name);
            lic_fname_cnt++;
         }
      }
      closedir(d);
   }
}

static int local_check_one_cit_key(char *product, license *citlicense)
{
   unsigned char hash[100];
   unsigned char *p;
   char str[100];
   int timelimited = 0;
   int validplatform = 1;
   int validproduct  = 1;
   int validversion  = 1;
   int validtime     = 1;
   int ignoreexpired  = 0;
   time_t  tnow = time(NULL);
   time_t  texpire = 0;
   struct tm exptm;
   int i;
   sprintf(str, "%d", citlicense->aversion);

   SHA1Reset(&sha1);
   if (citlicense->aversion == 1)
   {
      SHA1Input(&sha1, (const unsigned char *)str, (unsigned)strlen(str));
      SHA1Input(&sha1, (const unsigned char *)"\b", 1);
      add_to_sha1("zip", citlicense->azip);
      add_to_sha1("expires", citlicense->aexpires);
      add_to_sha1("address", citlicense->aaddress);
      add_to_sha1("subscriptionType", citlicense->asubscriptionType);
      add_to_sha1("companyName", citlicense->acompanyName);
      add_to_sha1("country", citlicense->acountry);
      add_to_sha1("city", citlicense->acity);
   }
   else
   {
      add_to_sha1("address", citlicense->aaddress);
      add_to_sha1("city", citlicense->acity);
      add_to_sha1("companyName", citlicense->acompanyName);
      add_to_sha1("country", citlicense->acountry);
      if (citlicense->acreator)
      {
         add_to_sha1(NULL, (char *)"creator");
         add_to_sha1("address", citlicense->acreator->aaddress);
         add_to_sha1("city", citlicense->acreator->acity);
         add_to_sha1("companyName", citlicense->acreator->acompanyName);
         add_to_sha1("country", citlicense->acreator->acountry);
         add_to_sha1("uuid", citlicense->acreator->auuid);
         add_to_sha1("zip", citlicense->acreator->azip);
      }
      add_to_sha1("demoPurpose", citlicense->ademoPurpose);
      add_to_sha1("expires", citlicense->aexpires);
      add_to_sha1("noRuntimeBreak", citlicense->anoRuntimeBreak);
      if (citlicense->aplatformsCount > 0)
      {
         cob_qsort(citlicense->aplatforms, citlicense->aplatformsCount,
                   sizeof(struct _e), cmp_e, NULL);
         add_to_sha1(NULL, (char *)"platforms");
         for (i = 0; i < citlicense->aplatformsCount; i++)
         {
            add_to_sha1(NULL, citlicense->aplatforms[i].aValue);
         }
      }
      if (citlicense->aproductsCount > 0)
      {
         cob_qsort(citlicense->aproducts, citlicense->aproductsCount,
                   sizeof(struct _e), cmp_e, NULL);
         add_to_sha1(NULL, (char *)"products");
         for (i = 0; i < citlicense->aproductsCount; i++)
         {
            add_to_sha1(NULL, citlicense->aproducts[i].aValue);
         }
      }
      add_to_sha1("subscriptionType", citlicense->asubscriptionType);
      add_to_sha1("uuid", citlicense->auuid);
      add_to_sha1("version", (char *)"2");
      add_to_sha1("versionMajor", citlicense->aversionMajor);
      add_to_sha1("versionMinor", citlicense->aversionMinor);
      add_to_sha1("versionPatch", citlicense->aversionPatch);
      add_to_sha1("zip", citlicense->azip);
   }

   SHA1Result(&sha1);
   p = hash;
   for (i = 0; i < 5; i++)
   {
      unsigned int u = sha1.Message_Digest[i];
      p[0] = (u >> 24) & 0xff;
      p[1] = (u >> 16) & 0xff;
      p[2] = (u >>  8) & 0xff;
      p[3] = (u)&0xff;
      p += 4;
   }
   if (!test_gmp(hash, citlicense->asignature))
   {
      if (getenv("DEBUG_KEY"))
      {
         dump_license(citlicense);
      }
      return -1;
   }
   if (citlicense->aexpires && (strlen(citlicense->aexpires) != 0))
   {
      int valid_error = 0;
      memset(&exptm, 0, sizeof(exptm));
      if (strlen(citlicense->aexpires) != 10)
      {
         valid_error = KEY_ERR_WRONG_FORMAT;
         goto valerror;
      }
      if (cob_strptime(citlicense->aexpires, "%Y-%m-%d", &exptm) == NULL)
      {
         valid_error = KEY_ERR_WRONG_FORMAT;
         goto valerror;
      }
      /*printf("%s\n", asctime(&exptm));*/
      texpire = mktime(&exptm);
      if (texpire < tnow)
      {
         validtime = 0;
         if (citlicense->aversion == 1)
         {
            valid_error = KEY_ERR_EXPIRED;
         }
      }
    valerror:
      if (valid_error)
      {
         if (getenv("DEBUG_KEY"))
         {
            printf("exp a = %s\n", asctime(&exptm));
            printf("exp   = %d %d %d\n", exptm.tm_year, exptm.tm_mon, exptm.tm_mday);
            printf("time  = %d , expt = %d\n", (int)tnow, (int)mktime(&exptm));
         }
         return valid_error;
      }
      timelimited = 1;
   }
   if (citlicense->aversion > 1)
   {
      validplatform = 0;
      validproduct  = 0;
      if (!citlicense->auuid)
      {
         return KEY_ERR_WRONG_FORMAT;
      }
      for (i = 0; i < citlicense->aplatformsCount; i++)
      {
         if (strcasecmp(citlicense->aplatforms[i].aValue, "ALL PLATFORMS") == 0)
         {
            validplatform = 1;
         }
         if (strcasecmp(citlicense->aplatforms[i].aValue, COB_PLATFORM) == 0)
         {
            validplatform = 1;
         }
      }
      if (citlicense->anoRuntimeBreak && (strcasecmp(product, COB_PN_RUNTIME) == 0))
      {
         ignoreexpired = 1;
      }
      for (i = 0; i < citlicense->aproductsCount; i++)
      {
         if (strcasecmp(citlicense->aproducts[i].aValue, "ALL PRODUCTS") == 0)
         {
            validproduct = 1;
         }
         if ((strcasecmp(citlicense->aproducts[i].aValue, COB_PN_COMPILER) == 0) &&
             (strcasecmp(product, COB_PN_RUNTIME) == 0))
         {
            validproduct = 1;
         }
         if (strcasecmp(citlicense->aproducts[i].aValue, product) == 0)
         {
            validproduct = 1;
         }
      }
      if ((strcasecmp(product, COB_PN_SYNTAX) == 0) ||
          (strcasecmp(product, COB_PN_COMPILER) == 0) ||
          (strcasecmp(product, COB_PN_RUNTIME) == 0))
      {
         if (citlicense->aversionMajor)
         {
            int vM, vm, vp;
            int v;
            char *s = strdup(COB_VERSION);
            cob_extract_version(s, &vM, &vm, &vp);
            v = atoi(citlicense->aversionMajor);
            if (v < vM)
            {
               validversion = 0;
            }
            if ((v == vM) && citlicense->aversionMinor)
            {
               v = atoi(citlicense->aversionMinor);
               if (v < vm)
               {
                  validversion = 0;
               }
               if ((v == vm) && citlicense->aversionPatch)
               {
                  v = atoi(citlicense->aversionPatch);
                  if (v < vp)
                  {
                     validversion = 0;
                  }
               }
            }
            free(s);
         }
      }
   }

   if (!validtime)
   {
      if (ignoreexpired)
      {
         return KEY_ERR_EXPIRED_IGNORE;
      }
      else
      {
         return KEY_ERR_EXPIRED;
      }
   }
   if (!validplatform)  return KEY_ERR_WRONG_PLATFORM;
   if (!validproduct)
   {
      return KEY_ERR_WRONG_PRODUCT;
   }
   if (!validversion)  return KEY_ERR_WRONG_VERSION;
   return 1;
}
static char *fname_list = NULL;
static int local_check_citkey(char *product)
{
   int loaded = 0;
   license *citlicense = NULL;
   int last_error    = 0;
   int cur_fname = 0;

   citlicense = citlicense_array[0].citlicense;

   if (!citlicense)
   {
      char *homedir = NULL;
      char *keyfname = NULL;
      char *citdir = NULL;
      homedir = getenv("HOME");
      keyfname = getenv("COBOLIT_LICENSE");
      citdir = getenv("COBOLITDIR");
      if (!keyfname)
      {
         add_lic_fname((char *)".");
         add_lic_fname(homedir);
         if (!citdir) citdir = (char *)COB_BASEDIR;
         add_lic_fname(citdir);
      }
      else
      {
         char *p;
         fname_list = strdup(keyfname);
         p = strtok(fname_list, ";,");
         while (lic_fname_cnt < 255 && p)
         {
            lic_fname[lic_fname_cnt++] = p;
            p = strtok(NULL, ";,");
         }
      }
      loaded = 0;
      do
      {
         if (lic_fname[cur_fname])
         {
            if (!citlicense_array[loaded].ctxt)
            {
               citlicense_array[loaded].ctxt = (S_XMLB_CONTEXT *)calloc(1, sizeof(S_XMLB_CONTEXT));
               xmlb_init_context(citlicense_array[loaded].ctxt);
            }

            if (xmlb_load(citlicense_array[loaded].ctxt, lic_fname[cur_fname]))
            {
               citlicense = accept_license(citlicense_array[loaded].ctxt);
               if (citlicense)
               {
                  citlicense_array[loaded].citlicense = citlicense;
                  citlicense_array[loaded].fname = lic_fname[cur_fname];
                  loaded++;
               }
            }
         }
         cur_fname++;
      }
      while (cur_fname < 255);
      if (!loaded)
      {
         return KEY_ERR_WRONG_FORMAT;
      }
   }
   else
   {
      int i;
      for (i = 0; i < 255; i++)
      {
         if (citlicense_array[i].citlicense) loaded++;
      }
   }


   while (loaded)
   {
      int err;
      citlicense = citlicense_array[--loaded].citlicense;
      err = local_check_one_cit_key(product, citlicense);
      if (err > 0) return err;
      if (err < 0) last_error = err;
   }
   return last_error;
}

static void dump_license(license *citlicense)
{
   int i;
   printf("Owner    :  %s\n", citlicense->acompanyName);
   if (citlicense->aaddress)
   {
      printf("            %s\n", citlicense->aaddress);
   }
   if (citlicense->azip || citlicense->acity)
   {
      printf("            %s %s\n", citlicense->azip ? citlicense->azip : "",
             citlicense->acity ? citlicense->acity : "");
   }
   if (citlicense->acountry)
   {
      printf("            %s\n\n", citlicense->acountry);
   }
   printf("Type     :  %s\n", citlicense->asubscriptionType ? citlicense->asubscriptionType : "Demo");
   if (citlicense->aplatformsCount > 0)
   {
      printf("Systems  :  ");
      for (i = 0; i < citlicense->aplatformsCount; i++)
      {
         printf("%s ", citlicense->aplatforms[i].aValue);
      }
      printf("\n");
   }
   if (citlicense->aproductsCount > 0)
   {
      printf("Products :  ");
      for (i = 0; i < citlicense->aproductsCount; i++)
      {
         printf("%s ", citlicense->aproducts[i].aValue);
      }
      printf("\n");
   }
   if (citlicense->acreator)
   {
      printf("Generator:  %s\n", citlicense->acreator->acompanyName);

   }
   if (citlicense->anoRuntimeBreak)
   {
      printf("Expire   :  %s (runtime never stop)\n", citlicense->aexpires ? citlicense->aexpires : "Never");
   }
   else
   {
      printf("Expire   :  %s\n", citlicense->aexpires ? citlicense->aexpires : "Never");
   }
   if (citlicense->aversionMajor)
   {
      printf("Version  :  Restricted up to %s", citlicense->aversionMajor);
      if (citlicense->aversionMinor)
      {
         printf(".%s", citlicense->aversionMinor);
         if (citlicense->aversionPatch)
         {
            printf(".%s", citlicense->aversionPatch);
         }
      }
      printf("\n");
   }
   printf("\n");

}


static void
dump_citkey_message(int v)
{
   int i;
   switch (v)
   {
      case 0:
         printf("\nNo enterprise license found.\n\n");
         break;
      case 1:
         for (i = 0; i < 255; i++)
         {
            if (citlicense_array[i].citlicense)
            {
               printf("\nCOBOL-IT License (v%d): %s\n", citlicense_array[i].citlicense->aversion, citlicense_array[i].fname);
               printf("----------------------\n");
               dump_license(citlicense_array[i].citlicense);
            }
         }
         break;
      case KEY_ERR_WRONG_FORMAT:
         printf("\nInvalid enterprise license file format: %s\n\n", current_fname);
         break;
      case KEY_ERR_EXPIRED_IGNORE:
      case KEY_ERR_EXPIRED:
         printf("\nAll enterprise license expired: \n");
         dump_citkey_message(1);
         break;
      case KEY_ERR_WRONG_PLATFORM:
         printf("\nNo Enterprise license do match current platform \n");
         dump_citkey_message(1);
         break;
      case KEY_ERR_WRONG_PRODUCT:
         printf("\nNo Enterprise license do match product \n");
         dump_citkey_message(1);
         break;
      case KEY_ERR_WRONG_VERSION:
         printf("\nEnterprise license restrict product version (current is %s)\n", COB_VERSION);
         dump_citkey_message(1);
         break;
      case -1:
      default:
         printf("\nInvalid enterprise license: %s\n\n", current_fname);
         break;
   }
}
int dump_citkey(const char *product)
{
   int v = local_check_citkey((char *)product);
   dump_citkey_message(v);
   return 0;
}

static void dump_error_message(void)
{
   printf("\n\nEnterprise license is not validated. \n");
   printf("Please Verify the environment variable COBOLIT_LICENSE value,\n");
   printf("contact your local dealer or sales@cobol-it.com\n");
}

int check_citkey(const  char *product)
{
   int v = local_check_citkey((char *)product);
   switch (v)
   {
      case 1:
         /* printf("COBOL-IT enterprise registered to: %s\n", citlicense->acompanyName);*/
         return v;
         break;
      case KEY_ERR_EXPIRED_IGNORE:
         dump_citkey_message(v);
         return v;
         break;
      default:
         dump_citkey_message(v);
         break;
   }
   dump_error_message();
   exit(-1);
}

int citkey_test_feature(const  char *product)
{
   int v = local_check_citkey((char *)product);
   switch (v)
   {
      case 1:
         /* printf("COBOL-IT enterprise registered to: %s\n", citlicense->acompanyName);*/
         return 1;
         break;
      case KEY_ERR_EXPIRED_IGNORE:
         dump_citkey_message(v);
         return 1;
         break;
      case KEY_ERR_WRONG_PLATFORM:
      case KEY_ERR_WRONG_FORMAT:
      case KEY_ERR_EXPIRED:
      case KEY_ERR_WRONG_VERSION:
         dump_citkey_message(v);
         break;
      case KEY_ERR_WRONG_PRODUCT:
         return 0;
      default:

         break;
   }
   dump_error_message();
   exit(-1);
}

static int rtd_key = KEY_NOT_CHECK_YET;

int runtime_check_enterprise_key(void)
{
   if (rtd_key == KEY_NOT_CHECK_YET)
   {
      rtd_key = local_check_citkey((char *)COB_PN_RUNTIME);
   }
   switch (rtd_key)
   {
      case 1:
         /* printf("COBOL-IT enterprise registered to: %s\n", citlicense->acompanyName);*/
         return 1;
         break;
      case KEY_ERR_EXPIRED_IGNORE:
         return 1;
         break;
      default:
         dump_error_message();
         break;
   }
   exit(-1);
}
void free_citkey(void)
{
   int i;
   for (i = 0; i < 255; i++)
   {
      if (citlicense_array[i].ctxt)
      {
         xmlb_dispose_context(citlicense_array[i].ctxt);
         free(citlicense_array[i].ctxt);
         citlicense_array[i].ctxt = NULL;
      }
   }
   if (fname_list)
   {
      free(fname_list);
   }
   else
   {
      {
         if (lic_fname[i])
         {
            free(lic_fname[i]);
         }
      }
   }
   for (i = 0; i < 255; i++) lic_fname[i] = NULL;
}

#else /* WITH_MF_KEYS */
int check_citkey (const char*product) {return 1;};
int citkey_test_feature (const  char*product) {return 1;};
int runtime_check_enterprise_key (void) {return 1;};
void free_citkey(void) {};

int dump_citkey (const char*product) {return 0;};

#endif  /* WITH_MF_KEYS */
