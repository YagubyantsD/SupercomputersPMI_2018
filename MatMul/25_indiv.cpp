#include "pch.h"
#include <iostream>
#include <time.h>
#include <omp.h>
#include <Windows.h>
using namespace std;


int ** sm(int n)
{
	int **A = new int *[n];

	for (int i = 0; i < n; i++)
	{
		A[i] = new int[n];
	}
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			int c = rand() % 9;
			A[i][j] = c;
			A[j][i] = c;
		}
	}
	return A;
}

int ** rm(int n, int *S)
{
	int **B = new int *[n];
	for (int i = 0; i < n; i++)
	{
		B[i] = new int[n - i];
	}
	int k = 0;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n - i; j++)
		{
			B[i][j] = S[k];
			k++;
		}
	}
	return B;
}

int ** block_sm(int **A, int row, int b, int k)
{
	int **D = new int*[k];
	for (int i = 0; i < k; i++)
	{
		D[i] = new int[k];
		for (int j = 0; j < k; j++)
		{
			D[i][j] = A[i + row * k][j + b * k];
		}
	}
	return D;
}


int ** block_rm(int **A, int b, int column, int k)
{
	int **res = new int*[k];
	for (int j = 0; j < k; j++)
	{
		res[j] = new int[k];
		for (int f = 0; f < k; f++)
		{
			if ((column != b) | (b*k + j <= column * k + f)) 
			{
				res[j][f] = A[b*k + j][column*k + f - b * k - j];
			}
			else res[j][f] = 0;
		}
	}
	return res;
}

void simple_multiplication(int **D, int **A, int **B, int n)
{
	cout << " *Simple multiplication of mutrix* " << endl;
	double st, et;
	st = omp_get_wtime();
	int ** block_SM = block_sm(A, 0, 0, n);
	int ** block_RM = block_rm(B, 0, 0, n);
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			for (int f = 0; f < n; f++)
				D[i][j] += block_SM[i][j] * block_RM[j][f];
		}
	}
	et = omp_get_wtime();
	cout << " Time: " << et - st << "\n\n";
	cout << endl;
}

void test(int **D, int **A, int **B, int n)
{
	cout << "start test " << endl;
	for (int k = 2; k < n; k++)
	{
		if (n%k == 0)
		{
			for (int row = 0; row < n / k; row++)
			{
				for (int column = 0; column < n / k; column++)
				{
					for (int b = 0; b < n / k; b++)
					{
						if (b <= column) 
						{

							for (int i = 0; i < k; i++)
							{
								for (int j = 0; j < k; j++)
								{
									for (int f = 0; f < k; f++) 
									{
										if ((column != b) | (b*k + j <= column * k + f))
										{
											D[row*k + i][b*k + j] += A[row*k + i][b*k + j] * B[b*k + j][column*k + f - b * k - j];
										}
									}
								}
							}
						}
					}

				}
			}
			for (int i = 0; i < n; i++)
			{
				for (int j = 0; j < n; j++)
				{
					cout << D[i][j] << " ";
					D[i][j] = 0;
				}
				cout << endl;
			}
			cout << endl;
		}
	}
	cout << "end test" << endl;
	cout << endl;
}

void parallel_without_memory(int **D, int **A, int **B, int n)
{
	cout << "start parallel without using memory for block: " << endl;
	for (int k = 2; k < n; k++)
	{
		if (n%k == 0)
		{
			cout << "block size: " << k << " : time:";
			double start, end;
			start = omp_get_wtime();
			for (int row = 0; row < n / k; row++)
			{
				for (int column = 0; column < n / k; column++)
				{
#pragma omp parallel num_threads(2) 
					{
#pragma omp for  schedule(static)
						for (int b = 0; b < n / k; b++)
						{
							if (b <= column) 
							{

								for (int i = 0; i < k; i++)
								{
									for (int j = 0; j < k; j++)
									{
										for (int f = 0; f < k; f++)
										{
											if ((column != b) | (b*k + j <= column * k + f))
											{
												D[row*k + i][b*k + j] += A[row*k + i][b*k + j] * B[b*k + j][column*k + f - b * k - j];
											}
										}
									}
								}
							}
						}

					}
				}
			}
			for (int i = 0; i < n; i++)
			{
				//cout << endl;
				for (int j = 0; j < n; j++)
				{
					//cout << D[i][j] << " ";
					D[i][j] = 0;
				}

			}
			end = omp_get_wtime();

			cout << "   " << end - start << "\n\n";
		}
	}
	cout << "stop parallel without using memory for block" << endl;
	cout << endl;
}


void noparallel_without_memory(int **D, int **A, int **B, int n)
{
	cout << "start noparallel without using memory for block: " << endl;
	for (int k = 2; k < n; k++)
	{
		if (n%k == 0)
		{
			cout << "block size: " << k << " : time:";
			double start, end;
			start = omp_get_wtime();
			for (int row = 0; row < n / k; row++)
			{
				for (int column = 0; column < n / k; column++)
				{

					for (int b = 0; b < n / k; b++)
					{
						if (b <= column) 
						{
							for (int i = 0; i < k; i++)
							{
								for (int j = 0; j < k; j++)
								{
									for (int f = 0; f < k; f++) 
									{
										if ((column != b) | (b*k + j <= column * k + f))
										{
											D[row*k + i][b*k + j] += A[row*k + i][b*k + j] * B[b*k + j][column*k + f - b * k - j];
										}
									}
								}
							}
						}
					}
				}
			}
			for (int i = 0; i < n; i++)
			{
				//cout << endl;
				for (int j = 0; j < n; j++)
				{
					//cout << D[i][j] << " ";
					D[i][j] = 0;
				}

			}
			end = omp_get_wtime();

			cout << "   " << end - start << "\n\n";
		}
	}
	cout << "stop noparallel without using memory for block" << endl;
	cout << endl;
}

void noparallel_with_memory_in_block(int **D, int **A, int **B, int n)
{
	cout << "start noparallel with using memory in block: " << endl;
	for (int k = 2; k < n; k++)
	{
		if (n%k == 0)
		{
			cout << "block size: " << k << " : time:";
			double start, end;
			start = omp_get_wtime();
			for (int row = 0; row < n / k; row++)
			{
				for (int column = 0; column < n / k; column++)
				{
					for (int b = 0; b < n / k; b++)
					{
						if (b <= column) 
						{
							int ** block_SM = block_sm(A, row, b, k);
							int ** block_RM = block_rm(B, b, column, k);
							for (int i = 0; i < k; i++)
							{
								for (int j = 0; j < k; j++)
								{
									for (int f = 0; f < k; f++)
										if ((column != b) | (b*k + j <= column * k + f))
										{
											D[row*k + i][b*k + j] += block_SM[i][j] * block_RM[j][f];
										}
								}
							}

							for (int i = 0; i < k; ++i) 
							{
								delete[] block_RM[i];
								delete[] block_SM[i];
							}
							delete[] block_RM;
							delete[] block_SM;

						}
					}

				}
			}
			for (int i = 0; i < n; i++)
			{
				//cout << endl;
				for (int j = 0; j < n; j++)
				{
					//cout << D[i][j] << " ";
					D[i][j] = 0;
				}

			}
			end = omp_get_wtime();

			cout << "   " << end - start << "\n\n";
		}
	}

	cout << "end noparallel with using memory in block" << endl;
	cout << endl;
}


void parallel_with_memory_in_block(int **D, int **A, int **B, int n)
{
	cout << "start parallel with using memory in block: " << endl;
	for (int k = 2; k < n; k++)
	{
		if (n%k == 0)
		{
			cout << "block size: " << k << " : time:";
			double start, end;
			start = omp_get_wtime();
			for (int row = 0; row < n / k; row++)
			{
				for (int column = 0; column < n / k; column++)
				{
#pragma omp parallel num_threads(2) 
					{
#pragma omp for  schedule(static)
						for (int b = 0; b < n / k; b++)
						{
							if (b <= column) 
							{
								int ** block_SM = block_sm(A, row, b, k);
								int ** block_RM = block_rm(B, b, column, k);
								for (int i = 0; i < k; i++)
								{
									for (int j = 0; j < k; j++)
									{
										for (int f = 0; f < k; f++)
											if ((column != b) | (b*k + j <= column * k + f))
											{
												D[row*k + i][b*k + j] += block_SM[i][j] * block_RM[j][f];
											}
									}
								}

								for (int i = 0; i < k; ++i) 
								{
									delete[] block_RM[i];
									delete[] block_SM[i];
								}
								delete[] block_RM;
								delete[] block_SM;

							}
						}
					}

				}
			}
			for (int i = 0; i < n; i++)
			{
				//cout << endl;
				for (int j = 0; j < n; j++)
				{
					//cout << D[i][j] << " ";
					D[i][j] = 0;
				}

			}
			end = omp_get_wtime();

			cout << "   " << end - start << "\n\n";
		}
	}

	cout << "End parallel with using memory in block" << endl;
	cout << endl;
}




int main()
{
	const int n = 720;  //  <= 8 for test
	int m = 0;
	for (int i = 1; i < n + 1; i++) 
	{
		m += i;
	}
	int *A_d = new int[m];
	int *B_d = new int[m];
	for (int i = 0; i < m; i++) 
	{
		A_d[i] = rand() % 9;
		B_d[i] = rand() % 9;
	}
	int **D = new int *[n];
	for (int i = 0; i < n; i++)
	{
		D[i] = new int[n];
		for (int j = 0; j < n; j++) 
		{
			D[i][j] = 0;
		}
	}
	int **A = sm(n);
	int **B = rm(n, B_d);

	parallel_without_memory(D, A, B, n);

	noparallel_without_memory(D, A, B, n);

	parallel_with_memory_in_block(D, A, B, n);

	noparallel_with_memory_in_block(D, A, B, n);

	simple_multiplication(D, A, B, n);

	for (int i = 0; i < n; i++)
	{
		D[i] = new int[n];
		for (int j = 0; j < n; j++) 
		{
			D[i][j] = 0;
		}
	}

	if (n <= 8) 
	{
		test(D, A, B, n);
	}

	for (int i = 0; i < n; ++i) 
	{
		delete[] A[i];
		delete[] B[i];
		delete[] D[i];
	}
	delete[] A;
	delete[] B;
	delete[] D;


	return 0;
}