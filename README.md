1) Accepts an infix statement as input via the front.in file.

2) Deconstructs the statement into lexemes and logs them into the intermediateCode.txt file as intermediate code. The intermediate code is also allocated onto the abstract stack machine and visualized.

3) The presedence rules of a statement are respected through a specific set of grammar rules and are traversed in the output.txt file recursively.

4) A syntax tree is created based on this path and logged within the tree.txt file.

5) The syntax tree is then printed via post-order traversal in order to create the postfix representation of the original infix statement. This can be read in postorder.txt.

6) As nodes are returned to their parent, the semantic analyzer takes over to ensure that expected values align properly with the actual values. Whatever conversions necessary occur during this step.

7) Happy Compiling !
_____________________________________________________________
***If you would like to test this out locally:***

-Load the front.in file with your statement
-Compile the file <YOURNET_23653899.cpp> with the command line: g++ YOURNET_23653899.cpp -o YOURNET_23653899
-Run the file <YOURNET_23653899> with the command line: ./YOURNET_23653899
