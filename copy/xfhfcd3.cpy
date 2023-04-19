*     * Copy Right Cobol-IT 
*     **********************************************************
*     * Uninitialised FCD definition for Callable File Handler *
*     **********************************************************
*     * As most as possible compatible with MF definition
*     * 'FCD3' used for 64 Bits 
           40  FCD-FILE-STATUS.
              42 FCD-STATUS-KEY-1    PIC X.
              42 FCD-STATUS-KEY-2    PIC X.
              42 FCD-BINARY          REDEFINES FCD-STATUS-KEY-2
                                     PIC X COMP-X.

           40  FCD-LENGTH            PIC XX COMP-X.
           40  FCD-VERSION           PIC X COMP-X.
              88 FCD--VERSION-NUMBER           VALUE 1.
           40  FCD-ORGANIZATION      PIC X COMP-X.
              88 FCD--LINE-SEQUENTIAL-ORG      VALUE 0.
              88 FCD--SEQUENTIAL-ORG           VALUE 1.
              88 FCD--INDEXED-ORG              VALUE 2.
              88 FCD--RELATIVE-ORG             VALUE 3.
              88 FCD--DETERMINE-ORG            VALUE 255.
           40  FCD-ACCESS-MODE       PIC X COMP-X.
              88 FCD--SEQUENTIAL-ACCESS        VALUE 0.
              88 FCD--DUP-PRIME-ACCESS         VALUE 1.
              88 FCD--RANDOM-ACCESS            VALUE 4.
              88 FCD--DYNAMIC-ACCESS           VALUE 8.
              88 FCD--STATUS-DEFINED           VALUE H"80".

           40  FCD-OPEN-MODE         PIC X COMP-X.
              88 FCD--OPEN-INPUT               VALUE 0.
              88 FCD--OPEN-OUTPUT              VALUE 1.
              88 FCD--OPEN-I-O                 VALUE 2.
              88 FCD--OPEN-EXTEND              VALUE 3.
              88 FCD--OPEN-MAX                 VALUE 3.
              88 FCD--OPEN-CLOSED              VALUE 128.

           40  FCD-RECORDING-MODE    PIC X COMP-X.
              88 FCD--RECMODE-FIXED            VALUE 0.
              88 FCD--RECMODE-VARIABLE         VALUE 1.

           40  FCD-FILE-FORMAT       PIC X COMP-X.
              88 FCD--FORMAT-LIIV1             VALUE 0.
              88 FCD--FORMAT-CISAM             VALUE 1.
              88 FCD--FORMAT-LIIV2             VALUE 2.
              88 FCD--FORMAT-COBOL2            VALUE 3.
              88 FCD--FORMAT-IDX4              VALUE 4.
              88 FCD--FORMAT-BTRIEVE-ANSI      VALUE 5.
              88 FCD--FORMAT-BTRIEVE-NON-ANSI  VALUE 6.
              88 FCD--FORMAT-BIG               VALUE 8.
              88 FCD--FORMAT-LEAFREC           VALUE 9.
              88 FCD--FORMAT-CST               VALUE 10.
              88 FCD--FORMAT-MVS-PRINT         VALUE 11.
              88 FCD--FORMAT-HEAP              VALUE 14.
              88 FCD--FORMAT-ESDS              VALUE 15.
              88 FCD--FORMAT-QSAMV             VALUE 255.
              88 FCD--MAX-FILE-FORMAT VALUE 16.
           40  FCD-DEVICE-FLAG       PIC X COMP-X.
              88 FCD--DEV-NORMAL               VALUE 0.
              88 FCD--DEV-DEVICE               VALUE 1.
              88 FCD--DEV-STDIN                VALUE 2.
              88 FCD--DEV-STDOUT               VALUE 3.
              88 FCD--DEV-STDERR               VALUE 4.
              88 FCD--DEV-BADNAME              VALUE 5.
              88 FCD--DEV-INPUT-PIPE           VALUE 6.
              88 FCD--DEV-OUTPUT-PIPE          VALUE 7.
              88 FCD--DEV-I-O-PIPE             VALUE 8.
              88 FCD--DEV-LIBRARY              VALUE 9.
              88 FCD--DEV-DISK-FILE            VALUE 10.
              88 FCD--DEV-NULL                 VALUE 11.
              88 FCD--DEV-DISK-REDIR           VALUE 12.
              88 FCD--DEV-NO-MAP               VALUE 13.

           40  FCD-LOCK-ACTION       PIC X COMP-X.
              88 FCD--GETLOCK                  VALUE 1.
              88 FCD--NOLOCK                   VALUE 2.
              88 FCD--IGNORELOCK               VALUE 3.

           40  FCD-DATA-COMPRESS     PIC X COMP-X.

           40  FCD-BLOCKING          PIC X COMP-X.
           40  FCD-ADDITIONAL-STATUS REDEFINES FCD-BLOCKING      
                                     PIC X    COMP-X.  

           40  FCD-IDXCACHE-SIZE     PIC X COMP-X.

           40  FCD-PERCENT           PIC X COMP-X.
           40  FCD-REC-COUNT-SET     REDEFINES FCD-PERCENT
                                     PIC X COMP-X.

           40  FCD-BLOCK-SIZE        PIC X COMP-X.

           40  FCD-FLAGS-1           PIC X COMP-X.
              88 FCD--MAINFRAME-COMPAT         VALUE H"80".
              88 FCD--ANSI-LINE-ADV            VALUE H"40".
              88 FCD--RETURN-KEY-ONLY          VALUE H"20".
              88 FCD--BYPASS-ESDS              VALUE H"10".
              88 FCD--NO-XFHNAME-MAPPING       VALUE H"08".
              88 FCD--DONT-CALL-XFHTRACE       VALUE H"04".
              88 FCD--CALL-XFHTRACE            VALUE H"02".
              88 FCD--FCD-DECL                 VALUE H"01".
           40  FCD-FLAGS-2           PIC X COMP-X.
              88  FCD--CONVERT-DBSPACE         VALUE H"01".

           40  FCD-MVS-FLAGS         PIC X COMP-X.
              88  FCD--FILE-IS-SYSPUNCH        VALUE H"10".
              88  FCD--FILE-IS-INDD            VALUE H"08".
              88  FCD--FILE-IS-OUTDD           VALUE H"04".
              88  FCD--AMODE-31BIT             VALUE H"02".
              88  FCD--AMODE-24BIT             VALUE H"01".
              88  FCD--AMODE-BITS                    VALUE H"03".
           40  FCD-STATUS-TYPE       PIC X COMP-X.
              88 FCD--ANS85-STATUS             VALUE H"80".
              88 FCD--NO-SPACE-FILL            VALUE H"40".
              88 FCD--NO-STRIP-SPACES          VALUE H"20".
              88 FCD--NO-EXPAND-TABS           VALUE H"10".
              88 FCD--REC-TERM-BIT             VALUE H"08".
              88 FCD--INSERT-TABS              VALUE H"04".
              88 FCD--INSERT-NULLS             VALUE H"02".
              88 FCD--CR-DELIMITER             VALUE H"01".
              88 FCD--MODIFY-WRITES            VALUE H"03".
           40  FCD-OTHER-FLAGS       PIC X COMP-X.
              88 FCD--OPTIONAL-FILE            VALUE H"80".
              88 FCD--NODETECTLOCK-INPUT       VALUE H"40".
              88 FCD--NOT-OPTIONAL             VALUE H"20".
              88 FCD--EXTERNAL-NAME            VALUE H"10".
              88 FCD--GET-INFO                 VALUE H"08".
              88 FCD--NODETECTLOCK             VALUE H"04".
              88 FCD--MULTIPLE-REEL            VALUE H"02".
              88 FCD--LINE-ADVANCING           VALUE H"01".
              88 FCD--SPECIAL-SEQUENTIAL       VALUE H"03".
           40  FCD-TRANS-LOG         PIC X COMP-X.
              88 FCD--OPEN-INPUT-SHARED        VALUE H"80".
              88 FCD--ALLOW-INPUT-LOCKS        VALUE H"40".
              88 FCD--NO-READ-SEMA             VALUE H"20".
              88 FCD--EXPAND-POSITIONING-BIT   VALUE H"10".

              88 FCD--NO-SEQ-CHECK             VALUE H"08".
              88 FCD--DAT-TERM-BIT             VALUE H"04".
              88 FCD--SLOW-READ                VALUE H"02".
              88 FCD--SUPPRESS-ADV             VALUE H"01".
           40  FCD-LOCKTYPES         PIC X COMP-X.
              88 FCD--INTERLANG-LOCKING        VALUE H"80".
              88 FCD--ALLOW-READERS            VALUE H"40".
              88 FCD--SEPARATE-LOCK-FILE       VALUE H"20".
              88 FCD--SINGLE-OPEN              VALUE H"10".
              88 FCD--NFS-FILE-LOCK            VALUE H"08".
              88 FCD--NFS-FILE-LOCK-HP         VALUE H"04".
              88 FCD--NFS-FILE-LOCKS           VALUE H"03".
           40  FCD-FS-FLAGS          PIC X COMP-X.
              88 FCD--TRANSACTION-PROCESSING-BIT  VALUE H"80".
              88 FCD--RECOVERY-RUN-B           VALUE H"04".
              88 FCD--FS-SERVER-BIT            VALUE H"02".

           40  FCD-CONFIG-FLAGS      PIC X COMP-X.
              88 FCD--WRITETHRU-BIT            VALUE H"80".
              88 FCD--RELATIVE-BIT             VALUE H"40".
              88 FCD--SET-CRP-BIT              VALUE H"20".
              88 FCD--BIGFILE-BIT              VALUE H"10".
              88 FCD--CALL-COBFSTATCONV        VALUE H"02".
              88 FCD--IGNORELOCK-BIT           VALUE H"01".
           40  FCD-MISC-FLAGS        PIC X COMP-X.
              88 FCD--MAINFRAME-HOSTFD         VALUE H"80".
              88 FCD--SET-IDXDATBUF            VALUE H"40".
              88 FCD--LOAD-ONTO-HEAP           VALUE H"20".
              88 FCD--USAGE-UNKNOWN            VALUE H"10".  
              88 FCD--RECMODE-S                VALUE H"08".  
              88 FCD--RECMODE-U                VALUE H"04".
              88 FCD--EXTERNAL-FCD             VALUE H"02".
              88 FCD--CLOSED-WITH-LOCK         VALUE H"01".
           40  FCD-CONFIG-FLAGS2     PIC X COMP-X.
              88 FCD--FILE-IS-EBCDIC           VALUE H"80".
              88 FCD--FILE-HAS-WRITE-AFTER     VALUE H"40".
              88 FCD--FILE-HAS-WRITE-BEFORE    VALUE H"20".
              88 FCD--FILE-HAS-ADV-SPECIFIED   VALUE H"10".
              88 FCD--NO-MIN-LEN-CHECK         VALUE H"08".
              88 FCD--NO-KEY-CHECK             VALUE H"04".
              88 FCD--CONVERT-TO-ASCII         VALUE H"02".
              88 FCD--RM-BEHAVIOUR             VALUE H"01".
              88 FCD--FILE-HAS-BEFORE-OR-AFTER VALUE H"03".
           40  FCD-LOCK-MODE         PIC X COMP-X.
              88 FCD--MULTILOCK-BIT            VALUE H"80".
              88 FCD--WRITELOCK-BIT            VALUE H"40".
              88 FCD--RETRY-OPEN-BIT           VALUE H"20".
              88 FCD--SKIP-LOCK-BIT            VALUE H"10".
              88 FCD--RETRY-LOCK-BIT           VALUE H"08".
              88 FCD--MANUAL-LOCK-BIT          VALUE H"04".
              88 FCD--AUTO-LOCK-BIT            VALUE H"02".
              88 FCD--EXCLUSIVE-BIT            VALUE H"01".
              88 FCD--SHARING-BITS             VALUE H"03".
           40  FCD-SHR2              PIC X COMP-X.
              88 FCD--FILE-MAX-BIT             VALUE H"08".
              88 FCD--FILE-POINTER-BIT         VALUE H"04".
              88 FCD--RETRY-TIME-BIT           VALUE H"02".
              88 FCD--START-UNLOCK             VALUE H"01".

           40  FCD-IDXCACHE-BUFFS    PIC X COMP-X.

           40  FCD-INTERNAL-FLAGS-1 PIC X COMP-X.
           40  FCD-INTERNAL-FLAGS-2 PIC X COMP-X.
           40                         PIC X(15).
           40  FCD-NLS-ID            PIC XX COMP-X.
           40  FCD-FS-FILE-ID        PIC XX COMP-X.
           40  FCD-RETRY-OPEN-COUNT  PIC XX COMP-X.
           40  FCD-NAME-LENGTH       PIC XX COMP-X.
           40  FCD-IDXNAME-LENGTH    PIC XX COMP-X.
           40  FCD-RETRY-COUNT       PIC XX COMP-X.
           40  FCD-KEY-ID            PIC XX COMP-X.
           40  FCD-LINE-COUNT        PIC XX COMP-X.
           40  FCD-USE-FILES         PIC X COMP-X.
           40  FCD-GIVE-FILES        PIC X COMP-X.
           40  FCD-KEY-LENGTH        PIC XX COMP-X.
           40                        PIC X(20).
           40  FCD-CURRENT-REC-LEN   PIC X(4) COMP-X.
           40  FCD-MIN-REC-LENGTH    PIC X(4) COMP-X.
           40  FCD-MAX-REC-LENGTH    PIC X(4) COMP-X.
           40  FCD-SESSION-ID        PIC X(4) COMP-X.
           40                        PIC X(24).
           40  FCD-RELADDR-OFFSET    PIC X(8) COMP-X.
           40  FCD-RELADDR           REDEFINES FCD-RELADDR-OFFSET
                                     PIC X(8) COMP-X.
           40  FCD-RELADDR-BIG       REDEFINES FCD-RELADDR-OFFSET
                                     PIC X(8) COMP-X.
           40  FCD-MAX-REL-KEY       PIC X(8) COMP-X.
           40  FCD-RELATIVE-KEY      PIC X(8) COMP-X.
           40  FCD-HANDLE            USAGE POINTER.
           40  FCD-HANDLE-NUM        REDEFINES FCD-HANDLE
                                     PIC X(4) COMP-X.
           40  REDEFINES FCD-HANDLE  PIC X(8).
           40  FCD-RECORD-ADDRESS    USAGE POINTER.
           40  REDEFINES FCD-RECORD-ADDRESS    PIC X(8).
           40  FCD-FILENAME-ADDRESS  USAGE POINTER.
           40  REDEFINES FCD-FILENAME-ADDRESS  PIC X(8).
           40  FCD-IDXNAME-ADDRESS   USAGE POINTER.
           40  FCD-INDEX-NAME        REDEFINES FCD-IDXNAME-ADDRESS
                                     USAGE POINTER.
           40  REDEFINES FCD-IDXNAME-ADDRESS   PIC X(8).
           40  FCD-KEY-DEF-ADDRESS   USAGE POINTER.
           40  REDEFINES FCD-KEY-DEF-ADDRESS   PIC X(8).
           40  FCD-COL-SEQ-ADDRESS   USAGE POINTER.
           40  REDEFINES FCD-COL-SEQ-ADDRESS   PIC X(8).
           40  FCD-FILDEF-ADDRESS    USAGE POINTER.
           40  REDEFINES FCD-FILDEF-ADDRESS    PIC X(8).
           40  FCD-DFSORT-ADDRESS    USAGE POINTER.
           40  REDEFINES FCD-DFSORT-ADDRESS    PIC X(8).
