# Storsge-Format (IR)
Developed a new storage formats for Sparse and Diagonally Dominant Sparse Matrices that is better than existing formats

## Abstract
This repository investigates information retrieval from sparse matrices, a crucial task across diverse fields like natural language processing, computer vision, and scientific computing. Sparse matrices, defined by a high density of zero elements, pose unique challenges for traditional retrieval methods. Existing techniques are often inefficient due to the substantial storage space devoted to zeros. This research explores the limitations of current methods and investigates novel approaches to enhance retrieval efficiency and accuracy, particularly for large-scale sparse matrices. The study critically evaluates existing indexing and searching strategies, proposing new algorithms tailored to exploit the inherent sparsity structure. This innovative approach seeks to optimize resource consumption and retrieval speed while maintaining data integrity and ensuring high-quality results.

## Contiguous Clustering (CC)
The ourMethodStr structure is where the CC format comes into play. It aims to organize the non-zero elements into clusters based on similar row and column offsets.
 - clusterSizes: Stores the size of each cluster.
 - startRowClus: Starting row index for each cluster.
 - startColClus: Starting column index for each cluster.
 - storeValues: Stores the values associated with elements in the cluster format.

### Example
![Depiction of working of Contiguous Clustering in a Sparse Matrix](https://github.com/bhusan-satya-007/Storsge-Format-IR-/blob/main/CC%20Diagramatic%20Depiction.png)

- **diagStorage:** The diagStorage struct would store the (row, col, value) and offset (col
row) for each non-zero element. These would be sorted by offset (col-row).
- **CCStorage:** The cc() function will create a clustering of the elements.

The CCStorage structure, after clustering, would contain information like this (using the example data):
– **numOfClusters:** 5 _(one for each diagonal)_
– **clusterSizes:** [2, 2, 1, 1, 1]_(size of each diagonal)_
– **startRowClus:** [3, 5, 2, 0, 1] _(starting rows of each diagonal)_
– **startColClus:** [0, 4, 2, 3, 5] _(starting columns of each diagonal)_
– **storeValues:** [7, 2, 6, 8, 1, 9, 3] _(values corresponding to the start rows)_

## Sparse Matrix Vector Multiplication (SpMV)
Sparse Matrix-Vector Multiplication (SpMV) is a fundamental operation in scientific and engineering computing, involving multiplying a sparse matrix with a vector, and is crucial for optimizing performance in various applications and platforms.

![A diagram of a hardware architecture designed to perform sparse matrix-vector multiplication](https://github.com/bhusan-satya-007/Storsge-Format-IR-/blob/main/SpMV%20Depiction.png)

- Result Vector Register: This register holds the elements of the resulting vector (often called ’x’ in the equation Ax = b) as the computation progresses. It is typically large enough to hold all the elements of the result vector.
- Column Position Register: This register likely stores information about the column index of non-zero elements in the sparse matrix.
- Column Position Register: This register likely stores information about the column index of non-zero elements in the sparse matrix.
- Matrix element Vector Register: This register is where elements of the sparse matrix (the 'A' in Ax = b) are stored, and from which they are read for computation.
- Vector Register containing elements of dense vector b: This register stores elements of the dense vector ’b’, the one that is being multiplied by a sparse matrix.
- Multiplier: This block performs multiplication. One of its inputs comes from the Matrix element Vector Register and the other from the Vector Register containing elements of dense vector b.
- Adder: This block performs addition. Its purpose is to accumulate the partial products from the multiplier.
- MUX(Multiplexer): The MUX acts as a selector. It decides where the initial value of the accumulator (part of the Result Vector Register) comes from.
