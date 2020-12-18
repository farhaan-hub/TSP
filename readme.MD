# Dependencies
    - C++

## (1) tsp.cpp
TSP solver (See details in `tsp.cpp`)
#### usage
```bash
g++ tsp.cpp
./a.out

Input File Name : tests\sample_test.txt
```
NOTE: 
1. make sure input file name points to the  folder/directory containing the input problem 
   (For simplicity, place the input file in the tests folder and enter the file name. eg: tests\sample_test.txt)
2. Format of the input is described here : https://github.com/baiqiushi/cs271p/blob/20a562c8b33125a8bdc8f9ce312a2622c328fabd/genMaxSAT.py
3. getTSP.py is included in tests folder for generating tsp problems

This program supports early termination either by USERS INTERRUPT i.e. ^C or if the time runs out (default 15 mins)

Checkout examples in **sample_run_screenshot.png** file in the zip for more info