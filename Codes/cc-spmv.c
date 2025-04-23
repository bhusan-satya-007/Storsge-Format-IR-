int main() {
    FILE* file = fopen("matrixDataset/crystk02.mtx", "r"); // Open matrix data file
     if (file == NULL) { 
        perror("Error opening file");  // Print an error if the file cannot be opened
        return EXIT_FAILURE; // Return failure status
    }
    char line[256]; // Character array to store each line

    // Skip comments and empty lines in the input file to retrieve the matrix dimensions
    while (fgets(line, sizeof(line), file) != NULL) {
        if (line[0] != '%' && line[0] != '\n') {
            break;
        }
    }

    int rows, cols, nonZeros;  // Variables to store matrix dimensions and number of non-zero elements
    sscanf(line, "%d %d %d", &rows, &cols, &nonZeros);  // Read dimensions and non-zero count from the line

     struct StoreDiag* diagStorage = (struct StoreDiag*)malloc(sizeof(struct StoreDiag)); // Allocate memory for storing diagonal data
     if(diagStorage == NULL){
          fprintf(stderr, "Memory allocation for diagStorage failed\n");
        fclose(file);
        return EXIT_FAILURE;
    }
    diagStorage = allocate_mem_diag(diagStorage, nonZeros); // Allocate memory for storing diagonal data

    printf("The no. of Rows: %d \nThe no. of Columns: %d \nThe No. of NonZeros in this matrix: %d\n", rows, cols, nonZeros); // Print dimensions


    int row, col;  // Variable for row index and column index
    double value;  // Variable for the value at the specified index
    int i=0;
    // Read the matrix data from file and store it in the diagStorage struct
    while (fscanf(file, "%d %d %lf", &row, &col, &value) == 3) {
        if (row >= 1 && row <= rows && col >= 1 && col <= cols ) { // Check index to ensure within bounds
            diagStorage->storeRow[i] = row;    // Store the row index in storeRow array
            diagStorage->storeCol[i] = col;    // Store the column index in storeCol array
            diagStorage->storeValues[i] = value; // Store the value in storeValues array
            diagStorage->storeOffsets[i++] = col-row;    // Store the offset in storeOffsets array and increment the counter
        } else {
            fprintf(stderr, "Warning: Index out of bounds (\%*d, \%*d)\n", row, col);
        }
    }

    fclose(file); // Close the matrix file

    //   printf("Original array: \n"); // Print unsorted data
    // printArray(diagStorage->storeOffsets, diagStorage->nonZeros);
    // printArray(diagStorage->storeCol, diagStorage->nonZeros);
    // printArray(diagStorage->storeRow, diagStorage->nonZeros);
    // printArrayValDouble(diagStorage->storeValues, diagStorage->nonZeros);

    sortOurMethodStr(diagStorage); // Call merge sort instead to sort the data based on offsets

    //   printf("Sorted array with all sorted: \n"); // Print sorted data
    // printArray(diagStorage->storeOffsets, diagStorage->nonZeros);
    // printArray(diagStorage->storeCol, diagStorage->nonZeros);
    // printArray(diagStorage->storeRow, diagStorage->nonZeros);
    // printArrayValDouble(diagStorage->storeValues, diagStorage->nonZeros);
     // Allocate memory for the ourMethodStr structure
    struct ourMethodStr* CCStorage = (struct ourMethodStr*)malloc(sizeof(struct ourMethodStr));
        if (CCStorage == NULL) {
        fprintf(stderr, "Memory allocation for CCStorage failed\n");
          free(diagStorage->storeOffsets);
        free(diagStorage->storeCol);
        free(diagStorage->storeRow);
        free(diagStorage->storeValues);
         free(diagStorage);
        return EXIT_FAILURE;
    }
    CCStorage = allocate_mem_cc(CCStorage,rows, cols, nonZeros);  // Allocate memory

     CCStorage->rows = rows;     // Store the number of rows
     CCStorage->cols = cols;     // Store the number of columns
    CCStorage->nonZeros = nonZeros; // Store the number of non-zero elements
    cc(diagStorage,CCStorage); // Call the cc function for clustering and spmv

    // printf("\nCluster Information:\n"); // Print cluster information
    // for (int i = 0; i < CCStorage->numOfClusters; i++) {
    //     printf("  Cluster \%*d:\n", i + 1); // Print cluster number
    //     printf("    Size: \%*d\n", CCStorage->clusterSizes[i]);  // Print the size of the cluster
    //     printf("    Start Row: \%*d\n", CCStorage->startRowClus[i]);  // Print the starting row of the cluster
    //     printf("    Start Col: \%*d\n", CCStorage->startColClus[i]); // Print the starting column of the cluster
    // }

    // Free the allocated memory
    free(diagStorage->storeOffsets);
    free(diagStorage->storeCol);
    free(diagStorage->storeRow);
    free(diagStorage->storeValues);
    free(diagStorage);
    free(CCStorage->clusterSizes);
    free(CCStorage->startRowClus);
    free(CCStorage->startColClus);
    free(CCStorage->storeValues);
    free(CCStorage);
    // return....
    return 0; // Return success
}