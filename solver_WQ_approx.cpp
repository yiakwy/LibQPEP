#include "solver_WQ_approx.h"
#include "solver_WQ_approx_helper.h"
#include "utils.h"
#include <ccomplex>
#include <cassert>

struct QPEP_runtime solver_WQ_approx(Eigen::MatrixXcd& sol_, const Eigen::VectorXd& data, const struct QPEP_options& opt) {
    assert(opt.ModuleName == "solver_WQ_approx");

    struct QPEP_runtime stat;
    Eigen::MatrixXd C1_;
    stat = GaussJordanElimination(C1_, data,
                                  reinterpret_cast<data_func_handle>(data_func_WQ_approx_sparse),
                                  249, 27,
                                  opt, stat);
    stat.statusDecomposition = 0;

    clock_t time1 = clock();
    Eigen::Matrix<double, 40, 27> RR;
    RR << - C1_.bottomRows(13), Eigen::Matrix<double, 27, 27>::Identity();
    int AM_ind[27] = {38, 16, 1, 19, 2, 3, 21, 22, 4, 25, 5, 6, 28, 7, 8, 30, 31, 9, 34, 10, 11, 36, 37, 12, 39, 40, 13};
    Eigen::Matrix<double, 27, 27> AM;
    for(int i = 0; i < 27; ++i)
    {
        AM.row(i) = RR.row(AM_ind[i] - 1);
    }

    Eigen::EigenSolver<Eigen::Matrix<double, 27, 27> > eigs(AM);
    Eigen::Matrix<std::complex<double>, 27, 27> V = eigs.eigenvectors();
    Eigen::Matrix<std::complex<double>, 27, 27> D(eigs.pseudoEigenvalueMatrix());

    for(int i = 0; i < 27; ++i)
    {
        std::complex<double> val = V(0, i);
        V.col(i) = V.col(i) / val;
    }

    for(int i = 0; i < 27; ++i)
    {
        std::complex<double> val = V(9, i);
        val = sqrt(val);
        sol_(1, i) = val;
        sol_(0, i) = V(1, i) / val;
        sol_(2, i) = V(3, i) / sol_(0, i);
        sol_(3, i) = V(2, i) / (sol_(0, i) * V(15, i));
    }

    clock_t time2 = clock();
    stat.timeGrobner = (time2 - time1) / double(CLOCKS_PER_SEC);
    stat.statusGrobner = 0;
    return stat;
}
