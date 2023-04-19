*     * Copy Right Cobol-IT 
*     **********************************************************
*     * Uninitialised FCD definition for Callable File Handler *
*     **********************************************************
*     * As most as possible compatible with MF definition
*     *
*     *'FCD2' used for 32 Bits 
           40  FCD-FILE-STATUS.
               42  FCD-STATUS-KEY-1 PIC X.
               42  FCD-STATUS-KEY-2 PIC X.
               42  FCD-BINARY       REDEFINES FCD-STATUS-KEY-2
                                    PIC X    COMP-X.
           40  FCD-LENGTH           PIC XX   COMP-X.
           40  FCD-VERSION          PIC X    COMP-X.
              88 FCD--VERSION-NUMBER           VALUE 0.

           40  FCD-ORGANIZATION     PIC X    COMP-X.
           40  FCD-ACCESS-MODE      PIC X    COMP-X.
           40  FCD-OPEN-MODE        PIC X    COMP-X.
           40  FILLER               PIC XX.
           40  FCD-BLOCK-SIZE       PIC X    COMP-X.
           40  FCD-NAME-LENGTH      PIC XX   COMP-X.
           40  FCD-RELADDR-BIG-AG.
	      42  FCD-RELADDR-BIG      PIC XXXX COMP-X.
	      42  FILLER               PIC XXXX COMP-X.
           40  REDEFINES FCD-RELADDR-BIG-AG.
              42  FCD-DEVICE-FLAG      PIC X COMP-X.
                   88 DEV-NORMAL      VALUE 0.
                   88 DEV-DEVICE      VALUE 1.
                   88 DEV-STDIN       VALUE 2.
                   88 DEV-STDOUT      VALUE 3.
                   88 DEV-STDERR      VALUE 4.
                   88 DEV-BADNAME     VALUE 5.
                   88 DEV-INPUT-PIPE  VALUE 6.
                   88 DEV-OUTPUT-PIPE VALUE 7.
                   88 DEV-I-O-PIPE    VALUE 8.
                   88 DEV-LIBRARY     VALUE 9.
                   88 DEV-DISK-FILE   VALUE 42.
                   88 DEV-NULL        VALUE 11.
                   88 DEV-DISK-REDIR  VALUE 12.
                   88 DEV-NO-MAP      VALUE 13.

              42  FCD-IDXNAME-LENGTH   PIC XX COMP-X.
              42  FCD-INDEX-NAME       USAGE POINTER.
           40  FILLER               PIC X.
           40  FCD-TRANS-LOG        PIC X    COMP-X.
           40  FILLER               PIC X.
           40  FCD-LOCK-MODE        PIC X    COMP-X.
           40  FCD-OTHER-FLAGS      PIC X    COMP-X.
           40  FILLER               PIC XX.
           40  FCD-HANDLE           USAGE POINTER.
           40  FCD-HANDLE-NUM       REDEFINES FCD-HANDLE
                                    PIC XXXX COMP-X.
           40  FCD-PERCENT          PIC X    COMP-X.
           40  FCD-REC-COUNT-SET    REDEFINES FCD-PERCENT
                                    PIC X    COMP-X.
           40  FCD-STATUS-TYPE      PIC X    COMP-X.
           40  FCD-FILE-FORMAT      PIC X    COMP-X.
           40  FILLER               PIC XXX.
           40  FCD-MAX-REC-LENGTH   PIC XX   COMP-X.
           40  FILLER               PIC XX.
           40  FCD-MVS-FLAGS        PIC X    COMP-X.
           40  FCD-RELATIVE-KEY     PIC XXXX COMP-X.
           40  FCD-RECORDING-MODE   PIC X    COMP-X.
           40  FCD-CURRENT-REC-LEN  PIC XX   COMP-X.
           40  FCD-MIN-REC-LENGTH   PIC XX   COMP-X.
           40  FCD-KEY-ID           PIC XX   COMP-X.
           40  FCD-LINE-COUNT       REDEFINES FCD-KEY-ID
                                    PIC XX   COMP-X.
           40                       REDEFINES FCD-KEY-ID.
               42  FCD-USE-FILES    PIC X    COMP-X.
               42  FCD-GIVE-FILES   PIC X    COMP-X.
           40  FCD-KEY-LENGTH       PIC XX   COMP-X.
           40  FCD-RECORD-ADDRESS   USAGE POINTER.
           40  FCD-FILENAME-ADDRESS USAGE POINTER.
           40  FCD-KEY-DEF-ADDRESS  USAGE POINTER.
           40  FCD-COL-SEQ-ADDRESS  USAGE POINTER.
           40  FCD-RELADDR-OFFSET   PIC XXXX COMP-X.
           40  FCD-RELADDR          REDEFINES FCD-RELADDR-OFFSET
                                    PIC XXXX COMP-X.
           40  FCD-FILDEF-ADDRESS   REDEFINES FCD-RELADDR-OFFSET
                                    USAGE POINTER.
           40  FCD-NLS-ID           PIC XX   COMP-X.
           40  FCD-DATA-COMPRESS    PIC X    COMP-X.
           40  FCD-SESSION-ID       PIC XXXX COMP-X.
           40  FCD-FS-FILE-ID       PIC XX   COMP-X.
           40  FCD-MAX-REL-KEY      PIC XXXX COMP-X.
           40  FCD-FLAGS-1          PIC X    COMP-X.
           40  FCD-BLOCKING         PIC X    COMP-X.
           40  FCD-ADDITIONAL-STATUS REDEFINES FCD-BLOCKING      
                                    PIC X    COMP-X.  
           40  FCD-LOCKTYPES        PIC X    COMP-X.
           40  FCD-FS-FLAGS         PIC X    COMP-X.
           40  FCD-CONFIG-FLAGS     PIC X    COMP-X.
           40  FCD-MISC-FLAGS       PIC X    COMP-X.
           40  FCD-CONFIG-FLAGS2    PIC X    COMP-X.
           40  FCD-IDXCACHE-SIZE    PIC X    COMP-X.
           40  FCD-IDXCACHE-BUFFS   PIC X    COMP-X.
           40  FCD-INTERNAL-FLAGS-1   PIC X COMP-X.                          
           40  FCD-INTERNAL-FLAGS-2   PIC X COMP-X.                          
