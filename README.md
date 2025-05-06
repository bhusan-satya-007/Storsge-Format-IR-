# Storage-Format (IR)
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

### SpMV Function Block
```
void spmv_coo(const double* x, double* y, const sparse_matrix_coo* A) {
 int num_rows = A->num_rows;
 int num_nonzeros = A->num_nonzeros;
 int* row_ind = A->row_ind;
 int* col_ind = A->col_ind;
 double* values = A->values;
 // Initialize y to 0
 for (int i = 0; i < num_rows; ++i) {
 y[i] = 0.0;
 }
 // Iterate through the non-zero elements
 for (int i = 0; i < num_nonzeros; ++i) {
 int row = row_ind[i];
 int col = col_ind[i];
 y[row] += values[i] * x[col]; // y[row] = y[row] + A[row,col] * x[col]
 }
 }
```

## Code Explanation for CC
### Creating Structure
```
struct ourMethodStr {
	int rows; // Number of rows
	int cols ; // Number of columns
	int nonZeros ; // Number of non−zero elements
 	int∗ clusterSizes ; // Array to store the size of each cluster
};
```

- Stores matrix data in a Clustered Column (CC) format, the final format for SPMV.
- **rows, cols, nonZeros:** Matrix dimensions and number of non-zero elements.
- **clusterSizes:** Stores the number of elements in each cluster. A cluster represents a contiguous block of non-zero elements.
- **numOfClusters:** Total number of clusters identified.
- **startRowClus, startColClus:** Starting row and column indices for each cluster. These mark the top-left corner of each cluster.
- **storeValues:** Stores element values, but ordered according to the clustering. This is the primary data structure for the clustered representation.

### Allocate Memory for Arrays in CC Structure
```
struct StoreDiag∗ allocate_mem_diag(struct StoreDiag∗ diagStorage , int nonZeros) {
	diagStorage−>storeOffsets = (int ∗)malloc(nonZeros ∗ sizeof (int)) ;
	diagStorage−>storeRow = (int ∗)malloc(nonZeros ∗ sizeof (int)) ;
	diagStorage−>storeCol = (int ∗)malloc(nonZeros ∗ sizeof (int)) ;
	diagStorage−>storeValues = (double∗)malloc(nonZeros ∗ sizeof (double)) ;
	diagStorage−>nonZeros = nonZeros ;
	return diag
}
```
- Allocates memory for arrays within an _ourMethodStr_ (CC) structure.
- _clusterSizes_, _startRowClus_, and _startColClus_ are initially allocated for only 1 integer. This is inefficient and likely a placeholder, requiring later reallocation.
- Sets the _rows_, _cols_, and _nonZeros_ members.

### Merge Funtion to merge two sorted subarrays into one sorted array
```
/* Function to merge two sorted subarrays during merge sort
Parameters:

CCStorage: A pointer to the StoreDiag struct
l: The left index of the subarray to be merged
m: The middle index of the subarray to be merged
r: The right index of the subarray to be merged
Return Value: void (modifies the StoreDiag struct directly)
*/

void merge(struct StoreDiag* CCStorage, int l, int m, int r) {
	int i, j, k;
	int n1 = m - l + 1; // Size of the left subarray
	int n2 = r - m; // Size of the right subarray

	// Temporary arrays for left and right subarrays
	int* LOffsets = (int*)malloc(n1 * sizeof(int));
	int* LRows = (int*)malloc(n1 * sizeof(int));
	int* LCols = (int*)malloc(n1 * sizeof(int));
	double* LValues = (double*)malloc(n1 * sizeof(double));

	int* ROffsets = (int*)malloc(n2 * sizeof(int));
	int* RRows = (int*)malloc(n2 * sizeof(int));
	int* RCols = (int*)malloc(n2 * sizeof(int));
	double* RValues = (double*)malloc(n2 * sizeof(double));

	 if (!LOffsets || !LRows || !LCols || !LValues || !ROffsets || !RRows || !RCols || !RValues) {
		fprintf ( stderr , "Memory allocation failed in merge\n") ;
		exit (1) ; // Handle memory allocation failure
	}

	// Copy data to temporary arrays
	for (i = 0; i < n1; i++) {
		LOffsets[i] = CCStorage->storeOffsets[l + i];
		LRows[i] = CCStorage->storeRow[l + i];
		LCols[i] = CCStorage->storeCol[l + i];
		LValues[i] = CCStorage->storeValues[l + i];
	}

	for (j = 0; j < n2; j++) {
		ROffsets[j] = CCStorage->storeOffsets[m + 1 + j];
		RRows[j] = CCStorage->storeRow[m + 1 + j];
		RCols[j] = CCStorage->storeCol[m + 1 + j];
		RValues[j] = CCStorage->storeValues[m + 1 + j];
	}

	// Merge the temporary arrays back into the original arrays in CCStorage
	i=0;
	j=0;
	k=1;
	
	while (i < n1 && j < n2) {
		if (LOffsets[i] <= ROffsets[j]) {
			CCStorage->storeOffsets[k] = LOffsets[i];
			CCStorage->storeRow[k] = LRows[i];
			CCStorage->storeCol[k] = LCols[i];
			CCStorage->storeValues[k] = LValues[i];
			i++;
		} 
		else {
			CCStorage->storeOffsets[k] = ROffsets[j];
			CCStorage->storeRow[k] = RRows[j];
			CCStorage->storeCol[k] = RCols[j];
			CCStorage->storeValues[k] = RValues[j];
			j++;
		}
		k++;
	}

	// Copy remaining elements of ROffsets [], if any
	while (j < n2) {
		CCStorage->storeOffsets[k]=ROffsets[j];
		CCStorage->storeRow[k]=RRows[j];
		CCStorage->storeCol[k]=RCols[j];
		CCStorage->storeValues[k]=RValues[j];
		j++;
		k++;
	}

	free ( LOffsets ) ; free (LRows) ; free (LCols) ; free (LValues) ; // Free temporary arrays for left subarray
	free ( ROffsets) ; free (RRows) ; free (RCols) ; free (RValues) ; // Free temporary arrays for right subarray
}
```

- The core component of merge sort. Merges two sorted subarrays into a single sorted array.
- **l, m, r:** Left, middle, and right indices of the array segment being merged.
- Creates temporary arrays (LOffsets, etc.) to hold the two subarrays for efficient merging.
- Merges based on the _storeOffsets_ values, preserving the correspondence between _storeRow_, _storeCol_, and _storeValues_.
- Critically, ensures proper memory management by freeing the temporary arrays to prevent memory leaks. This is essential for long-running processes or handling large datasets

### Merge Sort Function
```
void mergeSort(struct StoreDiag∗ CCStorage,int l,int r){
	 if(l < r){
		int m=l+(r−l)/2;

		mergeSort(CCStorage,l,m)
		mergesort(CCStorage,m+l,r)
		merge(CCStorage,l,m,r)
	}
}
```
- Aclassic recursive implementation of the merge sort algorithm.
- **l, r:** Left and right indices of the subarray to be sorted.
- **Base Case:** The recursion stops when the subarray contains only one element _(l >= r)_, as a single-element array is already sorted.
- **Divide and Conquer:** The algorithm divides the subarray into two halves, recursively sorts each half, and then merges the sorted halves using the merge function.

### sortOurMethodStr function
```
void sortOurMethodStr(struct StoreDiag∗ diagStorage){
	mergeSort(diagStorage,0,diagStorage−>nonZeros−1) ;
}
```
- This is awrapper function to initiate merge sort on _StoreDiag_ structure
- Calls _mergeSort_ on the entire range of the _storeOffsets_ array, sorting the structure by diagonal offset. This pre-processing step is important for the subsequent clustering process.

### printArray Function
```
void printArray(int arr [] , int size ) {
	f or (int i=0;i<size; i++) {
		printf("%d ",arr[ i ]) ;
	}
	printf ("\n") ;
}
```

- Prints the elements of an integer array to the console for debugging or visualization.

### printArrayValDouble Function
```
void printArrayValDouble(double arr[],int size){
	for (int i = 0; i < size; i++) {
		printf("% lf " , arr [ i ]) ;
	}
	printf ("\n") ;
}
```
- Prints the elements of a double array to the console. Useful for inspecting floating-point data.

### cc Function (Clustered Column and SPMV)
```
// Main function : Entry point of the program
// Parameters: void
// Return Value: int (0 for success , non-zero for failure )
int main() {
	FILE* file = fopen("matrixDataset/crystk02.mtx", "r");
	if (file == NULL) {
		perror("Error opening file");
		return EXIT_FAILURE;
	}
	char line[256];
	while (fgets(line, sizeof(line), file) != NULL) {
		if (line[0] != '%' && line[0] != '\n') {
			break;
		}
	}
	int rows, cols, nonZeros;
	sscanf(line, "%d %d %d", &rows, &cols, &nonZeros);
	struct StoreDiag* diagStorage = (struct StoreDiag*)malloc(sizeof(struct StoreDiag));
	if (diagStorage==NULL) {
		fprintf(stderr, "Memory allocation for diagStorage failed\n");
		fclose(file);
		return EXIT_FAILURE;
	}
	diagStorage = allocate_mem_diag(diagStorage, nonZeros);
	printf("The no. of Rows: %d \nThe no. of Columns: %d \nThe No. of NonZeros in this matrix : %d\n", rows, cols, nonZeros);
	int row, col;
	double value;
	int i = 0;
	while (fscanf(file, "%d %d %lf ", &row, &col, &value) == 3) {
		if (row >= 1 && row <= rows && col >= 1 && col <= cols) {
			diagStorage->storeRow[i] = row;
			diagStorage->storeCol[i] = col;
			diagStorage->storeValues[i] = value;
			diagStorage->storeOffsets[i++] = col - row;
		}
		else {
			fprintf(stderr, "Warning: Index out of bounds (%d, %d)\n", row, col);
		}
	}
	fclose(file); // Close the matrix file

	sortOurMethodStr(diagStorage) ; // Call merge sort instead to sort the data based on offsets
	
	struct ourMethodStr∗ CCStorage = (struct ourMethodStr∗)malloc(sizeof(struct ourMethodStr)) ;
	if (CCStorage == NULL) {
		fprintf ( stderr , "Memory allocation for CCStorage failed\n") ;
		free(diagStorage−>storeOffsets);
		free(diagStorage−>storeCol);
		free(diagStorage−>storeRows);
		free(diagStorage−>storeValues);
		free(diagStorage);
		return EXIT_FAILURE;
	}
	CCStorage=allocate_mem_cc(CCStorage,rows, cols , nonZeros); // Allocate memory
	CCStorage−>rows = rows; // Store the number of rows
	CCStorage−>cols = cols; // Store the number of columns
	CCStorage−>nonZeros =nonZeros; // Store the number of non−zero elements
	cc(diagStorage ,CCStorage); // Call the cc function for clustering and spmv
	free(diagStorage−>storeOffsets);

	//Free the allocated meaning
	free(diagStorage−>storeCol);
	free(diagStorage−>storeRow);
	free(diagStorage−>storeValues);
	free(diagStorage);
	free(CCStorage−>clusterSizes);
	free(CCStorage−>startRowClus);
	free(CCStorage−>startColClus);
	free(CCStorage−>storeValues);
	free(CCStorage);
	return 0; // Return success
}
```
- The entry point of the program where execution begins.
- **File Handling:** Opens and reads a sparse matrix dataset from a file in a specific format (likely _Matrix Market format_).
- **Memory Allocation:** Allocates memory for the key data structures: _StoreDiag_ and _ourMethodStr_ (CC). Error handling is included to gracefully handle memory allocation failures.
- **Data Loading and Preprocessing:** Reads the non-zero elements of the sparse matrix from the file, stores them in the diagStorage structure, and then sorts them based on their diagonal offset.
- **Clustering and SPMV:** Invokes the cc function to perform the clustering and sparse matrix-vector multiplication.
- **Crucial Memory Deallocation:** Frees all dynamically allocated memory using free(). This is absolutely essential to prevent memory leaks and ensure the program operates efficiently, especially when dealing with large datasets.** Failing to deallocate memory can lead to program crashes or system instability.
- **Return:** Indicates successful program execution by returning 0.
