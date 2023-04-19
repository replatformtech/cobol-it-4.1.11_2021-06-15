#ifndef NEWMETA_H
#define NEWMETA_H 1

/* XMLBooster-generated code (Version 2.16.8)
   This code is generated automatically. It is not meant
   to be maintained or even read. As it is generated, 
   it does not follow any coding standard. Please refrain
   from performing any change directly on this generated 
   code, as it might be overwritten anytime. */

#include <xmlbu.h>
#include <xmlb.h>
#ifndef XMLB_COMMIT_e
#define XMLB_COMMIT_e(x,y) XMLB_COMMIT(x,y)
#endif
#ifndef XMLB_COMMIT_creator
#define XMLB_COMMIT_creator(x,y) XMLB_COMMIT(x,y)
#endif
#ifndef XMLB_COMMIT_license
#define XMLB_COMMIT_license(x,y) XMLB_COMMIT(x,y)
#endif
#define SIG_e 9000
#define SIG_creator 9001
#define SIG_license 9002

#define IS_e(x) (XMLB_SIG(x)==SIG_e)
#ifndef XMLB_COMMIT_e
#define XMLB_COMMIT_e(x,y) XMLB_COMMIT(x,y)
#endif
#define IS_creator(x) (XMLB_SIG(x)==SIG_creator)
#ifndef XMLB_COMMIT_creator
#define XMLB_COMMIT_creator(x,y) XMLB_COMMIT(x,y)
#endif
#define IS_license(x) (XMLB_SIG(x)==SIG_license)
#ifndef XMLB_COMMIT_license
#define XMLB_COMMIT_license(x,y) XMLB_COMMIT(x,y)
#endif
#define AS_e(x) ((e*)(x))
#define AS_creator(x) ((creator*)(x))
#define AS_license(x) ((license*)(x))

typedef struct _e {
  /* Common prefix */
    struct _e *next;
    unsigned sig;
    void *father;
    void *first_son;
    void *sibling;
  /* Specific fields */
/* aValue */
    char * aValue;
  } e;


typedef struct _creator {
  /* Common prefix */
    struct _creator *next;
    unsigned sig;
    void *father;
    void *first_son;
    void *sibling;
  /* Specific fields */
/* azip */
    char * azip;
/* acompanyName */
    char * acompanyName;
/* acity */
    char * acity;
/* acountry */
    char * acountry;
/* aaddress */
    char * aaddress;
/* auuid */
    char * auuid;
  } creator;


typedef struct _license {
  /* Common prefix */
    struct _license *next;
    unsigned sig;
    void *father;
    void *first_son;
    void *sibling;
  /* Specific fields */
/* aversion */
    int aversion;
/* aversionMajor */
    char * aversionMajor;
/* aversionMinor */
    char * aversionMinor;
/* aversionPatch */
    char * aversionPatch;
/* auuid */
    char * auuid;
/* azip */
    char * azip;
/* aexpires */
    char * aexpires;
/* aaddress */
    char * aaddress;
/* asubscriptionType */
    char * asubscriptionType;
/* acompanyName */
    char * acompanyName;
/* acountry */
    char * acountry;
/* acity */
    char * acity;
/* asignature */
    char * asignature;
/* ademoPurpose */
    char * ademoPurpose;
/* anoRuntimeBreak */
    char * anoRuntimeBreak;
/* aproducts */
    int aproductsCount;
    e aproducts[15];
/* aplatforms */
    int aplatformsCount;
    e aplatforms[15];
/* acreator */
    struct _creator * acreator;
  } license;


e * new_e(XMLB_CONTEXT ctxt); 
creator * new_creator(XMLB_CONTEXT ctxt); 
license * new_license(XMLB_CONTEXT ctxt); 
void newmeta_generic_unparse(UNPARSE_FUNC uf, void * ctxt, void*obj,
                    char*tag,
                    BOOLEAN type_id);
void newmeta_generic_serialize(void*obj, char * beg, char *end);
void newmeta_generic_save(void*obj, char * fname);
void unparse_license(UNPARSE_FUNC f, void * ctxt, license *obj);
BOOLEAN serialize_license(license *obj, char * beg, char * end);
BOOLEAN save_license(license *obj, char * fname);
int unparsed_length_license(license *obj);
license* accept_license(XMLB_CONTEXT ctxt);
license* newmeta_generic_accept(XMLB_CONTEXT ctxt);
#ifndef XMLB_NO_TAG_TABLE
extern char * tag_table[] ;
#endif
#endif
