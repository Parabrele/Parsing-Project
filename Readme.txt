This is our implementation of the third level of the parsing project.

The files 'crc32.c' and 'hash.c' are those provided in the project description.
The subject of the project (found in 'subject.pdf') is to implement a parser for a simple programming language, and to enumerate all reachable states.

The language is defined in the subject, the lexer is in 'langlex.l' and the parser in 'lang.y'.

There are a few test programs in the test files, containing valid and invalid programs. They go from very basic just to see if the lexer and parser have the intended behavior, to more complex programs that test subtilities of the language and reachability.
The hardest test we had to pass were those using a do, as it made finding the next statement to execute not trivial.

To run a test :
    if not already done, run 'make langlex.c' then 'make lang' in the 'src' directory.

    then, outside of the 'src' directory, run :
        './src/lang test/<test_file_path>'

    or inside the 'src' directory :
        './lang ../test/<test_file_path>'