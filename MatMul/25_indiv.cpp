// 25_indiv.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <iostream>
#include <time.h>
#include <omp.h>
#include <Windows.h>
using namespace std;


int ** get_block(int **A, int position1, int position2, int k)
{
	int **D = new int*[k];
	int x = position1 * k;
	int y = position2 * k;
	for (int i = 0; i < k; i++)
	{
		D[i] = new int[k];
		for (int j = 0; j < k; j++)
		{
			D[i][j] = A[i + x][j + y];
		}
	}
	return D;
}



void calculate_parallel_without_memory(int **D, int **A, int **B, int n)
{
	cout << "start parallel without using memory for block: " << endl;
	for (int k = 2; k < n; k++)
	{
		if (n%k == 0)
		{
			double start_time, end_time;
			cout << "block size: " << k << " | time:";
			start_time = omp_get_wtime();
			for (int block_row = 0; block_row < int(n / k); block_row++)
			{

				for (int block_column = 0; block_column < int(n / k); block_column++)
				{

#pragma omp parallel num_threads(4) 
					{
#pragma omp for  schedule(static)
						for (int block_column2 = 0; block_column2 < n / k; block_column2++)
						{
							if (block_column2 <= block_column)
							{

								for (int i = 0; i < k; i++)
								{
									for (int j = 0; j < k; j++)
									{
										for (int f = 0; f < k; f++) 
										{
											D[block_row*k + i][block_column2*k + j] += A[block_row*k + i][block_column*k + f] * B[block_column*k + f][block_column2*k + j];
										}
									}
								}
							}
						}
					}


				}

			}
			end_time = omp_get_wtime();
			cout << end_time - start_time << "\n";
		}
	}
	cout << "stop parallel without using memory for block" << endl;
}


void calculate_parallel_with_memory_in_block(int **D, int **A, int **B, int n)
{
	cout << "start parallel with using memory for block: " << endl;
	for (int k = 2; k < n; k++)
	{
		if (n%k == 0)
		{
			cout << "block size: " << k << " | time:";
			double start_time, end_time;
			start_time = omp_get_wtime();
			for (int block_row = 0; block_row < int(n / k); block_row++)
			{

				for (int block_column = 0; block_column < int(n / k); block_column++)
				{
#pragma omp parallel num_threads(4)
					{
#pragma omp for  schedule(static)
						for (int block_column2 = 0; block_column2 < n / k; block_column2++)
						{
							if (block_column2 <= block_column) 
							{
								int ** sub_A = get_block(A, block_row, block_column, k);
								int ** sub_B = get_block(A, block_column, block_column2, k);
								for (int i = 0; i < k; i++) 
								{
									for (int j = 0; j < k; j++)
									{
										for (int f = 0; f < k; f++) 
										{
											D[block_row*k + i][block_column2*k + j] += sub_A[i][j] * sub_B[j][f];
										}
									}
								}
								for (int i = 0; i < k; ++i) 
								{
									delete[] sub_A[i];
									delete[] sub_B[i];
								}
								delete[] sub_A;
								delete[] sub_B;
							}
						}
					}
				}

			}
			end_time = omp_get_wtime();
			cout << end_time - start_time << "\n";
		}
	}
	cout << "stop parallel with using memory for block" << endl;
}



void calculate_test(int **D, int **A, int **B, int n)
{
	int c = 0;
	cout << "start test: " << endl;
	for (int k = 1; k < n + 1; k++)
	{
		if (n%k == 0)
		{
			cout << "block size: " << k << " | time:";
			double start_time, end_time;
			start_time = omp_get_wtime();
			for (int block_row = 0; block_row < n / k; block_row++)
			{
				for (int block_column = 0; block_column < n / k; block_column++)
				{
					for (int block_column2 = 0; block_column2 < n / k; block_column2++)
					{
						if (block_column2 <= block_column) 
						{
							c += 1;

							for (int i = 0; i < k; i++)
							{
								for (int j = 0; j < k; j++)
								{
									for (int f = 0; f < k; f++) 
									{
										D[block_row*k + i][block_column2*k + j] += A[block_row*k + i][block_column*k + f] * B[block_column*k + f][block_column2*k + j];
									}
								}
							}
						}
					}

				}
			}
			end_time = omp_get_wtime();

			cout << "   " << end_time - start_time << "\n\n";

			for (int i = 0; i < n; i++) 
			{
				cout << endl;
				for (int j = 0; j < n; j++) 
				{
					cout << D[i][j] << " ";
					D[i][j] = 0;
				}

			}
			cout << endl;
		}
	}
	cout << endl << c << endl;
	cout << "stop test" << endl;
}


void calculate_noparallel_with_memory_in_block(int **D, int **A, int **B, int n)
{
	cout << "start no parallel with using memory in block: " << endl;
	for (int k = 2; k < n; k++)
	{
		if (n%k == 0)
		{
			cout << "block size: " << k << " | time:";
			double start_time, end_time;
			start_time = omp_get_wtime();

			for (int block_row = 0; block_row < int(n / k); block_row++)
			{
				for (int block_column = 0; block_column < int(n / k); block_column++)
				{

					for (int block_column2 = 0; block_column2 < n / k; block_column2++)
					{
						if (block_column2 <= block_column) 
						{

							int ** sub_A = get_block(A, block_row, block_column, k);
							int ** sub_B = get_block(A, block_column, block_column2, k);
							for (int i = 0; i < k; i++) {
								for (int j = 0; j < k; j++)
								{
									for (int f = 0; f < k; f++) 
									{
										D[block_row*k + i][block_column2*k + j] += sub_A[i][j] * sub_B[j][f];
									}
								}
							}
							for (int i = 0; i < k; ++i) 
							{
								delete[] sub_A[i];
								delete[] sub_B[i];
							}
							delete[] sub_A;
							delete[] sub_B;
						}
					}

				}

			}
			end_time = omp_get_wtime();
			cout << "   " << end_time - start_time << "\n";
		}
	}

	cout << "end no parallel with using memory in block" << endl;
}

void calculate_noparallel_without_memory(int **D, int **A, int **B, int n)
{
	cout << "start noparallel without using memory for block: " << endl;
	for (int k = 2; k < n; k++)
	{
		if (n%k == 0)
		{
			double start_time, end_time;
			cout << "block size: " << k << " | time:";
			start_time = omp_get_wtime();
			for (int block_row = 0; block_row < int(n / k); block_row++)
			{

				for (int block_column = 0; block_column < int(n / k); block_column++)
				{
					for (int block_column2 = 0; block_column2 < n / k; block_column2++)
					{
						if (block_column2 <= block_column)
						{

							for (int i = 0; i < k; i++)
							{
								for (int j = 0; j < k; j++)
								{
									for (int f = 0; f < k; f++)
									{
										D[block_row*k + i][block_column2*k + j] += A[block_row*k + i][block_column*k + f] * B[block_column*k + f][block_column2*k + j];
									}
								}
							}
						}
					}
				}

			}
			end_time = omp_get_wtime();
			cout << end_time - start_time << "\n";
		}
	}
	cout << "stop noparallel without using memory for block" << endl;
}

void calculate_noparallel_without_block(int **D, int **A, int **B, int n)
{
	double start_time, end_time;
	start_time = omp_get_wtime();
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
			for (int k = 0; k < n; k++ )
			D[i][j] += A[i][k] * B[k][j];    
								
	}
	end_time = omp_get_wtime();
	cout << "   " << end_time - start_time << "\n\n";
	cout << endl;
			
}

int ** create_symmetrical_matrix(int n)
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

int ** create_rect_matrix(int **A, int n, int *sub_A, int count)
{
	int k = 0;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < i; j++)
		{
			A[j][i] = 0;
		}
		for (int j = i; j < n; j++)
		{
			A[j][i] = sub_A[k];
			k++;
		}
	}
	return A;
}


void print(int **A, int n)
{
	for (int i = 0; i < n; i++) 
	{
		cout << endl;
		for (int j = 0; j < n; j++)
		{
			cout << A[i][j] << " ";
		}
	}
}


int main()
{
	const int n = 720;  //  <= 8 for test
	int count = 0;
	for (int i = 1; i < n + 1; i++) 
	{
		count += i;
	}
	int *sub_A = new int[count];
	int *sub_B = new int[count];
	for (int i = 0; i < count; i++) 
	{
		sub_A[i] = rand() % 9;
		sub_B[i] = rand() % 9;
	}
	int **A = new int *[n];
	int **B = new int *[n];
	int **D = new int *[n];
	for (int i = 0; i < n; i++)
	{
		A[i] = new int[n];
		B[i] = new int[n];
		D[i] = new int[n];
	}

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++) 
		{
			D[i][j] = 0;
		}
	}


	A = create_symmetrical_matrix(n);
	B = create_rect_matrix(B, n, sub_B, count);

	if (n <= 8)
	{
		print(A, n);
		cout << endl;
		print(B, n);
		cout << endl;
		calculate_test(D, A, B, n);
	}

	calculate_parallel_without_memory(D, A, B, n);

	calculate_parallel_with_memory_in_block(D, A, B, n);

	calculate_noparallel_with_memory_in_block(D, A, B, n);

	calculate_noparallel_without_memory(D, A, B, n);

	calculate_noparallel_without_block(D, A, B, n);



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

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
