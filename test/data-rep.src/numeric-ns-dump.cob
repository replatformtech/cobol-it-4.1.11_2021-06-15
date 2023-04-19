       IDENTIFICATION   DIVISION.
       PROGRAM-ID.      prog.
       DATA             DIVISION.
       WORKING-STORAGE  SECTION.
       01 G-1.
         02 X-1         PIC 9(1) VALUE 1
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-2.
         02 X-2         PIC 9(2) VALUE 12
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-3.
         02 X-3         PIC 9(3) VALUE 123
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-4.
         02 X-4         PIC 9(4) VALUE 1234
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-5.
         02 X-5         PIC 9(5) VALUE 12345
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-6.
         02 X-6	        PIC 9(6) VALUE 123456
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-7.
         02 X-7         PIC 9(7) VALUE 1234567
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-8.
         02 X-8         PIC 9(8) VALUE 12345678
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-9.
         02 X-9         PIC 9(9) VALUE 123456789
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-10.
         02 X-10        PIC 9(10) VALUE 1234567890
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-11.
         02 X-11        PIC 9(11) VALUE 12345678901
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-12.
         02 X-12        PIC 9(12) VALUE 123456789012
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-13.
         02 X-13        PIC 9(13) VALUE 1234567890123
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-14.
         02 X-14        PIC 9(14) VALUE 12345678901234
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-15.
         02 X-15        PIC 9(15) VALUE 123456789012345
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-16.
         02 X-16        PIC 9(16) VALUE 1234567890123456
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-17.
         02 X-17        PIC 9(17) VALUE 12345678901234567
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       01 G-18.
         02 X-18        PIC 9(18) VALUE 123456789012345678
                        @USAGE@.
         02 FILLER      PIC X(18) VALUE SPACE.
       PROCEDURE        DIVISION.
      * dump each values
           CALL "dump" USING G-1
           END-CALL.
           CALL "dump" USING G-2
           END-CALL.
           CALL "dump" USING G-3
           END-CALL.
           CALL "dump" USING G-4
           END-CALL.
           CALL "dump" USING G-5
           END-CALL.
           CALL "dump" USING G-6
           END-CALL.
           CALL "dump" USING G-7
           END-CALL.
           CALL "dump" USING G-8
           END-CALL.
           CALL "dump" USING G-9
           END-CALL.
           CALL "dump" USING G-10
           END-CALL.
           CALL "dump" USING G-11
           END-CALL.
           CALL "dump" USING G-12
           END-CALL.
           CALL "dump" USING G-13
           END-CALL.
           CALL "dump" USING G-14
           END-CALL.
           CALL "dump" USING G-15
           END-CALL.
           CALL "dump" USING G-16
           END-CALL.
           CALL "dump" USING G-17
           END-CALL.
           CALL "dump" USING G-18
           END-CALL.
           INITIALIZE X-1.    CALL "dump" USING G-1
           END-CALL.
           INITIALIZE X-2.    CALL "dump" USING G-2
           END-CALL.
           INITIALIZE X-3.    CALL "dump" USING G-3
           END-CALL.
           INITIALIZE X-4.    CALL "dump" USING G-4
           END-CALL.
           INITIALIZE X-5.    CALL "dump" USING G-5
           END-CALL.
           INITIALIZE X-6.    CALL "dump" USING G-6
           END-CALL.
           INITIALIZE X-7.    CALL "dump" USING G-7
           END-CALL.
           INITIALIZE X-8.    CALL "dump" USING G-8
           END-CALL.
           INITIALIZE X-9.    CALL "dump" USING G-9
           END-CALL.
           INITIALIZE X-10.   CALL "dump" USING G-10
           END-CALL.
           INITIALIZE X-11.   CALL "dump" USING G-11
           END-CALL.
           INITIALIZE X-12.   CALL "dump" USING G-12
           END-CALL.
           INITIALIZE X-13.   CALL "dump" USING G-13
           END-CALL.
           INITIALIZE X-14.   CALL "dump" USING G-14
           END-CALL.
           INITIALIZE X-15.   CALL "dump" USING G-15
           END-CALL.
           INITIALIZE X-16.   CALL "dump" USING G-16
           END-CALL.
           INITIALIZE X-17.   CALL "dump" USING G-17
           END-CALL.
           INITIALIZE X-18.   CALL "dump" USING G-18
           END-CALL.
           MOVE ZERO TO X-1.    CALL "dump" USING G-1
           END-CALL.
           MOVE ZERO TO X-2.    CALL "dump" USING G-2
           END-CALL.
           MOVE ZERO TO X-3.    CALL "dump" USING G-3
           END-CALL.
           MOVE ZERO TO X-4.    CALL "dump" USING G-4
           END-CALL.
           MOVE ZERO TO X-5.    CALL "dump" USING G-5
           END-CALL.
           MOVE ZERO TO X-6.    CALL "dump" USING G-6
           END-CALL.
           MOVE ZERO TO X-7.    CALL "dump" USING G-7
           END-CALL.
           MOVE ZERO TO X-8.    CALL "dump" USING G-8
           END-CALL.
           MOVE ZERO TO X-9.    CALL "dump" USING G-9
           END-CALL.
           MOVE ZERO TO X-10.   CALL "dump" USING G-10
           END-CALL.
           MOVE ZERO TO X-11.   CALL "dump" USING G-11
           END-CALL.
           MOVE ZERO TO X-12.   CALL "dump" USING G-12
           END-CALL.
           MOVE ZERO TO X-13.   CALL "dump" USING G-13
           END-CALL.
           MOVE ZERO TO X-14.   CALL "dump" USING G-14
           END-CALL.
           MOVE ZERO TO X-15.   CALL "dump" USING G-15
           END-CALL.
           MOVE ZERO TO X-16.   CALL "dump" USING G-16
           END-CALL.
           MOVE ZERO TO X-17.   CALL "dump" USING G-17
           END-CALL.
           MOVE ZERO TO X-18.   CALL "dump" USING G-18
           END-CALL.
           STOP RUN.
