ZOBOS RESOURCES
===============

File extensions and contents
----------------------------
.syn:  Source with syntax errors
.src:  Use the source, Luke!
.tok:  Token stream from .src
.out:  ${COMPGRADING}/output-line filtered results, the minimum your
       ZOBOS should generate
.vrb:  My own more verbose output, contains REASON lines detailing why
       each SYNTAX, ERROR, or WARNING is generated
.pdf:  AST tree visual
.sym:  emit symtable output (if it is generated in source)


Particular files for testing and debug
--------------------------------------
Each type of non-syntax semantic check has a corresponding *-1.src file,
for instance novar-1.src is for the NOVAR semantic checks.

These files have been constructed to test a wide array of the particular
semantic issues, and in most cases, NO OTHER SEMANTIC issues should be 
reported for the file.  THE EXCEPTION being the const-1.src file, where
there is a lone UNINIT issue --- but this was need to show that
uninitialized const variables are still const.

In all cases the accompanying .out files show precisely the error or 
warning messages your code should generate.  Output will be sorted by
line number, column, and ID before grading comparisons, so it doesn't
matter the order of your output lines.

garrett-1.src
-------------
https://matrix.to/#/!dYtEsTLbatQVNIkdnf:csci498ab.modular.im/$--dtkbmBCX4p7v3lajGJXaBrpKt4mEyx4waSw0hoBJM?via=csci498ab.modular.im

