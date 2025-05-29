#define ssthg_app_name x2
#include <libraries/system/replacements/unistd.h>
#include <mask_mpi.h>
#include <iostream>
#include <mercury/common/skeleton.h>

int main(int argc, char** argv) {

  int rank, size, intervals = 1e8;
  //double sum = 0.0;
  //double result = 0.0;
  void *sum = sst_hg_nullptr;
  void *result = sst_hg_nullptr;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

//  for (int i=0; i <= intervals / 2; ++i) {
//    double x = rank/2.0 + i * 1/(1.0*intervals) + 1/(2.0 * intervals);
//    sum += x * x * 1/(1.0 * intervals);
//  }

  sleep(1);

  MPI_Reduce(&sum,&result,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
  if(rank==0) std::cerr << "Rieman sum is " << "not computed" << std::endl;

  MPI_Finalize();
  return 0;
}
