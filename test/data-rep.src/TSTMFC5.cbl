       IDENTIFICATION DIVISION.
      *------------------------
       PROGRAM-ID.             TSTMFC5.
       AUTHOR.                 VDE.
       DATE-WRITTEN.           11/07/08.
      *
       ENVIRONMENT DIVISION.
      *---------------------
       CONFIGURATION SECTION.
       SOURCE-COMPUTER.        UNIX.
       OBJECT-COMPUTER.        UNIX.
       SPECIAL-NAMES.          DECIMAL-POINT IS COMMA.
      *
       DATA DIVISION.
      *--------------

       WORKING-STORAGE SECTION.

          01 WORKER.
      * 1. PIC 9
            03 HELL-1 PIC 9(4).
      * 2. PIC S9
            03 HELL-2 PIC S9(4).
      * 3. PIC 9 REDEFINES PIC X
            03 VAR1-3 PIC X(4).
            03 HELL-3 REDEFINES VAR1-3 PIC 9(4).
      * 4. PIC S9 REDEFINES PIC X
            03 VAR1-4 PIC X(4).
            03 HELL-4 REDEFINES VAR1-4 PIC S9(4).
      * 5. PIC 9 IN GROUP
            03 VAR1-5.
              05 VAR2-5 PIC X(4).
              05 HELL-5 PIC 9(4).
      * 6. PIC S9 IN GROUP
            03 VAR1-6.
              05 VAR2-6 PIC X(4).
              05 HELL-6 PIC S9(4).
      * 7. PIC 9 IN GROUP REDEFINES PIC X
            03 VAR1-7 PIC X(8).
            03 VAR2-7 REDEFINES VAR1-7.
               05 VAR3-7 PIC X(4).
               05 HELL-7 PIC 9(4).
      * 8. PIC S9 IN GROUP REDEFINES PIC X
            03 VAR1-8 PIC X(8).
            03 VAR2-8 REDEFINES VAR1-8.
               05 VAR3-8 PIC X(4).
               05 HELL-8 PIC S9(4).
            03 SPACED-4 PIC X(4).
            03 SPACED-8 PIC X(8).
            03 HEADER.
               05 FILLER PIC X(30) VALUES "Case".
               05 FILLER PIC X(3) VALUES " | ".
               05 FILLER PIC X(20) VALUES "Init Type".
               05 FILLER PIC X(3) VALUES " | ".
               05 FILLER PIC X(8) VALUES "0".
               05 FILLER PIC X(3) VALUES " | ".
               05 FILLER PIC X(8) VALUES "ZEROES".
               05 FILLER PIC X(3) VALUES " | ".
               05 FILLER PIC X(8) VALUES """0000""".
               05 FILLER PIC X(3) VALUES " | ".
               05 FILLER PIC X(8) VALUES "SPACES".
               05 FILLER PIC X(3) VALUES " | ".
               05 FILLER PIC X(8) VALUES """    """.
               05 FILLER PIC X(3) VALUES " | ".
               05 FILLER PIC X(8) VALUES "DISPLAY.".
            03 RESULT-LINE OCCURS 2.
               05 NUMBER-TYPE PIC X(30).
               05 FILLER PIC X(3) VALUES " | ".
               05 INIT-TYPE PIC X(20).
               05 FILLER PIC X(3) VALUES " | ".
               05 EQ-0 PIC X(8).
               05 FILLER PIC X(3) VALUES " | ".
               05 EQ-ZEROES PIC X(8).
               05 FILLER PIC X(3) VALUES " | ".
               05 EQ-0CHAR PIC X(8).
               05 FILLER PIC X(3) VALUES " | ".
               05 EQ-SPACES PIC X(8).
               05 FILLER PIC X(3) VALUES " | ".
               05 EQ-BLANKS PIC X(8).
               05 FILLER PIC X(3) VALUES " | ".
               05 FILLER PIC X(2) VALUES "##".
               05 DUMP PIC X(4).
               05 FILLER PIC X(2) VALUES "##".
      *

       PROCEDURE DIVISION.
      *-------------------

       BEGIN.

       DISPLAY HEADER.

      * 1. Test PIC 9
      *--------------

       INITIALIZE RESULT-LINE(1) .
       INITIALIZE RESULT-LINE(2) .

       MOVE "Test PIC 9" TO NUMBER-TYPE OF RESULT-LINE(1).
       MOVE "Test PIC 9" TO NUMBER-TYPE OF RESULT-LINE(2).

       MOVE "No initialization" TO INIT-TYPE OF RESULT-LINE(1).
       MOVE "No initialization" TO INIT-TYPE OF RESULT-LINE(2).

       IF HELL-1 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-1 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-1 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-1 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-1 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-1 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-1 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC 9" TO NUMBER-TYPE OF RESULT-LINE(1).
       MOVE "Test PIC 9" TO NUMBER-TYPE OF RESULT-LINE(2).

       MOVE "MOVE SPACES" TO INIT-TYPE OF RESULT-LINE(1).
       MOVE "MOVE SPACES" TO INIT-TYPE OF RESULT-LINE(2).

       MOVE SPACES TO HELL-1 OF WORKER.

       IF HELL-1 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-1 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-1 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-1 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-1 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-1 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-1 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC 9" TO NUMBER-TYPE OF RESULT-LINE(1).
       MOVE "Test PIC 9" TO NUMBER-TYPE OF RESULT-LINE(2).

       MOVE "MOVE X" TO INIT-TYPE OF RESULT-LINE(1).
       MOVE "MOVE X" TO INIT-TYPE OF RESULT-LINE(2).
       
       MOVE SPACES TO SPACED-4.
       MOVE SPACED-4 TO HELL-1 OF WORKER.

       IF HELL-1 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-1 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-1 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-1 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-1 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-1 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-1 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).
       
       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC 9" TO NUMBER-TYPE OF RESULT-LINE(1).
       MOVE "Test PIC 9" TO NUMBER-TYPE OF RESULT-LINE(2).

       MOVE "Initialization" TO INIT-TYPE OF RESULT-LINE(1).
       MOVE "Initialization" TO INIT-TYPE OF RESULT-LINE(2).
       
       INITIALIZE HELL-1 OF WORKER.

       IF HELL-1 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-1 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-1 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-1 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-1 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-1 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-1 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-1 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

      * 2. Test PIC S9
      *---------------

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC S9" TO NUMBER-TYPE OF RESULT-LINE(1).
       MOVE "Test PIC S9" TO NUMBER-TYPE OF RESULT-LINE(2).

       MOVE "No initialization" TO INIT-TYPE OF RESULT-LINE(1).
       MOVE "No initialization" TO INIT-TYPE OF RESULT-LINE(2).

       IF HELL-2 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-2 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-2 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-2 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-2 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-2 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-2 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC S9" TO NUMBER-TYPE OF RESULT-LINE(1).
       MOVE "Test PIC S9" TO NUMBER-TYPE OF RESULT-LINE(2).

       MOVE "MOVE SPACES" TO INIT-TYPE OF RESULT-LINE(1).
       MOVE "MOVE SPACES" TO INIT-TYPE OF RESULT-LINE(2).

       MOVE SPACES TO HELL-2 OF WORKER.

       IF HELL-2 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-2 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-2 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-2 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-2 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-2 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-2 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC S9" TO NUMBER-TYPE OF RESULT-LINE(1).
       MOVE "Test PIC S9" TO NUMBER-TYPE OF RESULT-LINE(2).

       MOVE "MOVE X" TO INIT-TYPE OF RESULT-LINE(1).
       MOVE "MOVE X" TO INIT-TYPE OF RESULT-LINE(2).
       
       MOVE SPACES TO SPACED-4.
       MOVE SPACED-4 TO HELL-2 OF WORKER.

       IF HELL-2 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-2 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-2 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-2 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-2 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-2 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-2 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).
       
       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC S9" TO NUMBER-TYPE OF RESULT-LINE(1).
       MOVE "Test PIC S9" TO NUMBER-TYPE OF RESULT-LINE(2).

       MOVE "Initialization" TO INIT-TYPE OF RESULT-LINE(1).
       MOVE "Initialization" TO INIT-TYPE OF RESULT-LINE(2).
       
       INITIALIZE HELL-2 OF WORKER.

       IF HELL-2 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-2 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-2 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-2 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-2 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-2 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-2 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-2 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

      * 3. Test PIC 9 REDEFINES PIC X
      *-------------------------------

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC 9 REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(1).
       MOVE "Test PIC 9 REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(2).

       MOVE "No initialization" TO INIT-TYPE OF RESULT-LINE(1).
       MOVE "No initialization" TO INIT-TYPE OF RESULT-LINE(2).

       IF HELL-3 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-3 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-3 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-3 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-3 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-3 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-3 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC 9 REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(1).
       MOVE "Test PIC 9 REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(2).

       MOVE "MOVE SPACES" TO INIT-TYPE OF RESULT-LINE(1).
       MOVE "MOVE SPACES" TO INIT-TYPE OF RESULT-LINE(2).

       MOVE SPACES TO VAR1-3 OF WORKER.

       IF HELL-3 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-3 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-3 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-3 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-3 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-3 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-3 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC 9 REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(1).
       MOVE "Test PIC 9 REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(2).

       MOVE "MOVE X" TO INIT-TYPE OF RESULT-LINE(1).
       MOVE "MOVE X" TO INIT-TYPE OF RESULT-LINE(2).
       
       MOVE SPACES TO SPACED-4.
       MOVE SPACED-4 TO VAR1-3 OF WORKER.

       IF HELL-3 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-3 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-3 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-3 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-3 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-3 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-3 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).
       
       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC 9 REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(1).
       MOVE "Test PIC 9 REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(2).

       MOVE "Initialization" TO INIT-TYPE OF RESULT-LINE(1).
       MOVE "Initialization" TO INIT-TYPE OF RESULT-LINE(2).
       
       INITIALIZE VAR1-3 OF WORKER.

       IF HELL-3 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-3 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-3 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-3 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-3 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-3 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-3 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-3 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

      * 4. Test PIC S9 REDEFINES PIC X
      *--------------

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC S9 REDEFINES PIC X"
            TO NUMBER-TYPE OF RESULT-LINE(1).
       MOVE "Test PIC S9 REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(2).

       MOVE "No initialization" TO INIT-TYPE OF RESULT-LINE(1).
       MOVE "No initialization" TO INIT-TYPE OF RESULT-LINE(2).

       IF HELL-4 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-4 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-4 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-4 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-4 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-4 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-4 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC S9 REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(1).
       MOVE "Test PIC S9 REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(2).

       MOVE "MOVE SPACES" TO INIT-TYPE OF RESULT-LINE(1).
       MOVE "MOVE SPACES" TO INIT-TYPE OF RESULT-LINE(2).

       MOVE SPACES TO VAR1-4 OF WORKER.

       IF HELL-4 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-4 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-4 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-4 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-4 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-4 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-4 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC S9 REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(1).
       MOVE "Test PIC S9 REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(2).

       MOVE "MOVE X" TO INIT-TYPE OF RESULT-LINE(1).
       MOVE "MOVE X" TO INIT-TYPE OF RESULT-LINE(2).
       
       MOVE SPACES TO SPACED-4.
       MOVE SPACED-4 TO VAR1-4 OF WORKER.

       IF HELL-4 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-4 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-4 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-4 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-4 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-4 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-4 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).
       
       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC S9 REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(1).
       MOVE "Test PIC S9 REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(2).

       MOVE "Initialization" TO INIT-TYPE OF RESULT-LINE(1).
       MOVE "Initialization" TO INIT-TYPE OF RESULT-LINE(2).
       
       INITIALIZE VAR1-4 OF WORKER.

       IF HELL-4 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-4 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-4 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-4 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-4 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-4 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-4 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-4 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

      * 5. Test PIC 9 IN GROUP
      *-----------------------

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC 9 IN GROUP" 
            TO NUMBER-TYPE OF RESULT-LINE(1)
       MOVE "Test PIC 9 IN GROUP" 
            TO NUMBER-TYPE OF RESULT-LINE(2)

       MOVE "No initialization" TO INIT-TYPE OF RESULT-LINE(1)
       MOVE "No initialization" TO INIT-TYPE OF RESULT-LINE(2)

       IF HELL-5 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-5 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-5 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-5 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-5 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-5 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-5 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC 9 IN GROUP" 
            TO NUMBER-TYPE OF RESULT-LINE(1)
       MOVE "Test PIC 9 IN GROUP" 
            TO NUMBER-TYPE OF RESULT-LINE(2)

       MOVE "MOVE SPACES" TO INIT-TYPE OF RESULT-LINE(1)
       MOVE "MOVE SPACES" TO INIT-TYPE OF RESULT-LINE(2)

       MOVE SPACES TO VAR1-5 OF WORKER.

       IF HELL-5 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-5 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-5 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-5 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-5 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-5 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-5 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC 9 IN GROUP" 
            TO NUMBER-TYPE OF RESULT-LINE(1)
       MOVE "Test PIC 9 IN GROUP" 
            TO NUMBER-TYPE OF RESULT-LINE(2)

       MOVE "MOVE X" TO INIT-TYPE OF RESULT-LINE(1)
       MOVE "MOVE X" TO INIT-TYPE OF RESULT-LINE(2)
       
       MOVE SPACES TO SPACED-8.
       MOVE SPACED-8 TO VAR1-5 OF WORKER.

       IF HELL-5 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-5 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-5 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-5 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-5 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-5 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-5 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).
       
       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC 9 IN GROUP" 
            TO NUMBER-TYPE OF RESULT-LINE(1)
       MOVE "Test PIC 9 IN GROUP" 
            TO NUMBER-TYPE OF RESULT-LINE(2)

       MOVE "Initialization" TO INIT-TYPE OF RESULT-LINE(1)
       MOVE "Initialization" TO INIT-TYPE OF RESULT-LINE(2)
       
       INITIALIZE VAR1-5 OF WORKER.

       IF HELL-5 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-5 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-5 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-5 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-5 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-5 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-5 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-5 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

      * 6. Test PIC S9 IN GROUP
      *-----------------------

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC S9 IN GROUP" 
            TO NUMBER-TYPE OF RESULT-LINE(1)
       MOVE "Test PIC S9 IN GROUP" 
            TO NUMBER-TYPE OF RESULT-LINE(2)

       MOVE "No initialization" TO INIT-TYPE OF RESULT-LINE(1)
       MOVE "No initialization" TO INIT-TYPE OF RESULT-LINE(2)

       IF HELL-6 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-6 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-6 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-6 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-6 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-6 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-6 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC S9 IN GROUP" 
            TO NUMBER-TYPE OF RESULT-LINE(1)
       MOVE "Test PIC S9 IN GROUP" 
            TO NUMBER-TYPE OF RESULT-LINE(2)

       MOVE "MOVE SPACES" TO INIT-TYPE OF RESULT-LINE(1)
       MOVE "MOVE SPACES" TO INIT-TYPE OF RESULT-LINE(2)

       MOVE SPACES TO VAR1-6 OF WORKER.

       IF HELL-6 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-6 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-6 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-6 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-6 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-6 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-6 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC S9 IN GROUP" 
            TO NUMBER-TYPE OF RESULT-LINE(1)
       MOVE "Test PIC S9 IN GROUP" 
            TO NUMBER-TYPE OF RESULT-LINE(2)

       MOVE "MOVE X" TO INIT-TYPE OF RESULT-LINE(1)
       MOVE "MOVE X" TO INIT-TYPE OF RESULT-LINE(2)
       
       MOVE SPACES TO SPACED-8.
       MOVE SPACED-8 TO VAR1-6 OF WORKER.

       IF HELL-6 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-6 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-6 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-6 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-6 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-6 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-6 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).
       
       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC S9 IN GROUP" 
            TO NUMBER-TYPE OF RESULT-LINE(1)
       MOVE "Test PIC S9 IN GROUP" 
            TO NUMBER-TYPE OF RESULT-LINE(2)

       MOVE "Initialization" TO INIT-TYPE OF RESULT-LINE(1)
       MOVE "Initialization" TO INIT-TYPE OF RESULT-LINE(2)
       
       INITIALIZE VAR1-6 OF WORKER.

       IF HELL-6 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-6 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-6 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-6 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-6 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-6 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-6 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-6 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

      * 7. Test PIC 9 IN GROUP REDEFINES PIC X
      *-----------------------

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC 9 IN GROUP REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(1)
       MOVE "Test PIC 9 IN GROUP REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(2)

       MOVE "No initialization" TO INIT-TYPE OF RESULT-LINE(1)
       MOVE "No initialization" TO INIT-TYPE OF RESULT-LINE(2)

       IF HELL-7 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-7 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-7 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-7 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-7 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-7 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-7 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC 9 IN GROUP REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(1)
       MOVE "Test PIC 9 IN GROUP REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(2)

       MOVE "MOVE SPACES" TO INIT-TYPE OF RESULT-LINE(1)
       MOVE "MOVE SPACES" TO INIT-TYPE OF RESULT-LINE(2)

       MOVE SPACES TO VAR1-7 OF WORKER.

       IF HELL-7 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-7 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-7 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-7 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-7 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-7 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-7 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC 9 IN GROUP REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(1)
       MOVE "Test PIC 9 IN GROUP REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(2)

       MOVE "MOVE X" TO INIT-TYPE OF RESULT-LINE(1)
       MOVE "MOVE X" TO INIT-TYPE OF RESULT-LINE(2)
       
       MOVE SPACES TO SPACED-8.
       MOVE SPACED-8 TO VAR1-7 OF WORKER.

       IF HELL-7 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-7 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-7 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-7 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-7 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-7 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-7 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).
       
       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC 9 IN GROUP REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(1)
       MOVE "Test PIC 9 IN GROUP REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(2)

       MOVE "Initialization" TO INIT-TYPE OF RESULT-LINE(1)
       MOVE "Initialization" TO INIT-TYPE OF RESULT-LINE(2)
       
       INITIALIZE VAR1-7 OF WORKER.

       IF HELL-7 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-7 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-7 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-7 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-7 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-7 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-7 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-7 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

      * 8. Test PIC S9 IN GROUP REDEFINES PIC X
      *-----------------------

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC S9 IN GROUP REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(1)
       MOVE "Test PIC S9 IN GROUP REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(2)

       MOVE "No initialization" TO INIT-TYPE OF RESULT-LINE(1)
       MOVE "No initialization" TO INIT-TYPE OF RESULT-LINE(2)

       IF HELL-8 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-8 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-8 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-8 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-8 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-8 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-8 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC S9 IN GROUP REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(1)
       MOVE "Test PIC S9 IN GROUP REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(2)

       MOVE "MOVE SPACES" TO INIT-TYPE OF RESULT-LINE(1)
       MOVE "MOVE SPACES" TO INIT-TYPE OF RESULT-LINE(2)

       MOVE SPACES TO VAR1-8 OF WORKER.

       IF HELL-8 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-8 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-8 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-8 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-8 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-8 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-8 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC S9 IN GROUP REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(1)
       MOVE "Test PIC S9 IN GROUP REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(2)

       MOVE "MOVE X" TO INIT-TYPE OF RESULT-LINE(1)
       MOVE "MOVE X" TO INIT-TYPE OF RESULT-LINE(2)
       
       MOVE SPACES TO SPACED-8.
       MOVE SPACED-8 TO VAR1-8 OF WORKER.

       IF HELL-8 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-8 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-8 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-8 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-8 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-8 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-8 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).
       
       INITIALIZE RESULT-LINE(1).
       INITIALIZE RESULT-LINE(2).

       MOVE "Test PIC S9 IN GROUP REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(1)
       MOVE "Test PIC S9 IN GROUP REDEFINES PIC X" 
            TO NUMBER-TYPE OF RESULT-LINE(2)

       MOVE "Initialization" TO INIT-TYPE OF RESULT-LINE(1)
       MOVE "Initialization" TO INIT-TYPE OF RESULT-LINE(2)
       
       INITIALIZE VAR1-8 OF WORKER.

       IF HELL-8 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = 0
          MOVE "==" TO EQ-0 OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0 OF RESULT-LINE(2)
       END-IF.

       IF HELL-8 OF WORKER = ZEROES
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = ZEROS
          MOVE "==" TO EQ-ZEROES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-ZEROES OF RESULT-LINE(2)
       END-IF.

       IF HELL-8 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = "0000"
          MOVE "==" TO EQ-0CHAR OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-0CHAR OF RESULT-LINE(2)
       END-IF.

       IF HELL-8 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = SPACES
          MOVE "==" TO EQ-SPACES OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-SPACES OF RESULT-LINE(2)
       END-IF.

       IF HELL-8 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(1)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(1)
       END-IF.
       IF HELL-8 OF WORKER = "    "
          MOVE "==" TO EQ-BLANKS OF RESULT-LINE(2)
       ELSE
          MOVE "<>" TO EQ-BLANKS OF RESULT-LINE(2)
       END-IF.

       MOVE HELL-8 OF WORKER TO DUMP OF RESULT-LINE(1).
       MOVE HELL-8 OF WORKER TO DUMP OF RESULT-LINE(2).

       DISPLAY RESULT-LINE(1).
       DISPLAY RESULT-LINE(2).

       EXIT PROGRAM.
