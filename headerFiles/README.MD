### TO EXECUTE -
make clean && make
./myshell

### Complete Code Review

1. Display requirement:
Format - <user@system:path>
Approach - Using the functionality of getlogin(), gethostname(), and getcwd() and also
identifies the shell's home directory (shellHomeDir) and replaces the path with ~ when appropriate.
a. set up  the home directory
b. diplay prompt 

2. cd, echo, pwd:
a. pwd -> using getcwd() gets the absolute path of the current working directory.
b. echo -> prints its arguments back to the screen by iteratating loop through the vector of tokens
c. cd -> in this multiple cases has been handled for different flags like “.”, “..”, “-” and “~”.

3. ls:
a. Handle the flags(-a, -l) and directory names