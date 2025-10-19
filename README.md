Solution to Task:
Investigate a given data operation on different data volumes, with different policies and with different transformations or predicates (if applicable). Namely, investigate
1) the performance of the library algorithm in the absence of a policy;
2) the performance of the library algorithm with different policies;
3) the performance of your own parallel algorithm using a different number of threads, namely: build your own algorithm that splits the sample into K parts of approximately equal length, processes each part in a separate thread (using a sequential library algorithm), and then sums the result (using a sequential library algorithm). Deduce the dependence of the running time on the value of the parameter K. At what K is the best speed achieved? How does this value correlate with the number of processor threads? By what law does the time increase with increasing K?
The code should perform the appropriate experiments and output the running time (+so that you can understand what exactly was measured). For your own algorithm, you should output the data in a tabular form. Separately, display the best value of K found and its relationship to the number of processor threads.
If the code displays the necessary information, then instead of a report, you can attach a text file with the result of its work.
Perform the study exclusively in compilation mode in RELEASE mode. Investigate how the time depends on the level of optimization (without optimization and with maximum).
Obtain sequences using random tools.
Use several sequences of different lengths.
Variant:
6) inner_product (with policies use transform_reduce)
