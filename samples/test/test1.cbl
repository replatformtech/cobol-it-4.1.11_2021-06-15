      *-----------------------------------------------------------------
       IDENTIFICATION DIVISION.
       PROGRAM-ID. 'test1'.
       ENVIRONMENT DIVISION.
       INPUT-OUTPUT SECTION.
       FILE-CONTROL.
       DATA DIVISION.
       FILE SECTION.
      *
       WORKING-STORAGE SECTION.
      *
       01  MYCHAR    PIC X(80).
       01  PID       PIC 9(8).
      *-----------------------------------------------------------------
       LINKAGE SECTION.
      *-----------------------------------------------------------------
       PROCEDURE DIVISION.
      *-----------------------------------------------------------------
       MAIN SECTION.
            CALL "C$PID" USING PID
            DISPLAY "ENTER TEST1 PID = " PID
            MOVE "Hello world" TO MYCHAR
            DISPLAY MYCHAR
            ACCEPT  MYCHAR
            EXIT PROGRAM.
      *-----------------------------------------------------------------
      *--- End of program --------------------------------------
