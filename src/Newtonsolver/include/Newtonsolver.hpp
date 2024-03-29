#pragma once
#include "../../testingWithGoogle/TestProblem.hpp"
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <Eigen/SparseQR>
#include <Matrixhandler.hpp>
#include <Problem.hpp>
#include <iostream>

namespace Model {}

/// \brief This namespace holds tools for solving numerical problems, e.g.
/// finding the root of a non-linear function.
namespace Solver {

  /// \brief This struct holds info on the solution of a solve-execution.
  ///
  /// @param success is true, if solve found a solution.
  /// @param residual is the absolute value of f(new_state) after solve.
  /// #param the number of Newton steps need.
  struct Solutionstruct {
    bool success{false};
    double residual{1000000.0};
    int used_iterations{0};
  };

  /// \brief Manages solving non-linear systems and (to be implemented)
  ///        computing derivatives with respect to controls.
  ///
  /// This class holds a SparseMatrix and a corresponding Sparse-matrix solver,
  /// so it can compute the solution of a non-linear problem.
  template <typename Problemtype> class Newtonsolver_temp {
  public:
    Newtonsolver_temp(double _tolerance, int _maximal_iterations)
        : tolerance(_tolerance), maximal_iterations(_maximal_iterations){};

    void evaluate_state_derivative_triplets(Problemtype &problem,
                                            double last_time, double new_time,
                                            Eigen::VectorXd const &last_state,
                                            Eigen::VectorXd &new_state) {
      new_state = last_state;
      {
        jacobian.resize(new_state.size(), new_state.size());
        Aux::Triplethandler handler(&jacobian);

        Aux::Triplethandler *const handler_ptr = &handler;
        problem.evaluate_state_derivative(handler_ptr, last_time, new_time,
                                          last_state, new_state);
        handler.set_matrix();
      }
      sparselusolver.analyzePattern(jacobian);
    };

    void evaluate_state_derivative_coeffref(Problemtype &problem,
                                            double last_time, double new_time,
                                            Eigen::VectorXd const &last_state,
                                            Eigen::VectorXd const &new_state) {
      Aux::Coeffrefhandler handler(&jacobian);
      Aux::Coeffrefhandler *const handler_ptr = &handler;
      problem.evaluate_state_derivative(handler_ptr, last_time, new_time,
                                        last_state, new_state);
    };

    /// \brief This method computes a solution to f(new_state) == 0.
    ///
    /// It uses
    /// an affine-invariant Newton method described in chapter 4.2 in
    /// "Deuflhard and Hohmann: Numerical Analysis in Modern Scientific
    /// Computing". Afterwards there should hold f(new_state) == 0 (up to
    /// tolerance).
    Solutionstruct solve(Eigen::VectorXd &new_state, Problemtype &problem,
                         double last_time, double new_time,
                         Eigen::VectorXd const &last_state) {
      Solutionstruct solstruct;

      Eigen::VectorXd rootvalues(new_state.size());

      // compute f(x_k);
      problem.evaluate(rootvalues, last_time, new_time, last_state, new_state);
      // compute f'(x_k) and write it to the jacobian.
      evaluate_state_derivative_triplets(problem, last_time, new_time,
                                         last_state, new_state);

      while (rootvalues.norm() > tolerance &&
             solstruct.used_iterations < maximal_iterations) {
        sparselusolver.compute(jacobian);
        // compute Dx_k:
        Eigen::VectorXd step = -sparselusolver.solve(rootvalues);

        double lambda = 1.0;
        // candidate for x_{k+1}
        Eigen::VectorXd candidate_vector = new_state + lambda * step;

        // f(x_{k+1}
        Eigen::VectorXd candidate_values(new_state.size());
        problem.evaluate(candidate_values, last_time, new_time, last_state,
                         candidate_vector);

        // Delta^bar x_k+1
        Eigen::VectorXd delta_x_bar = -sparselusolver.solve(candidate_values);

        double current_norm = delta_x_bar.norm();

        double testnorm = step.norm();
        while (current_norm > (1 - 0.5 * lambda) * testnorm) {
          lambda *= 0.5;
          candidate_vector = new_state + lambda * step;
          problem.evaluate(candidate_values, last_time, new_time, last_state,
                           candidate_vector);
          current_norm = (-sparselusolver.solve(candidate_values)).norm();
        }
        new_state = candidate_vector;
        rootvalues = candidate_values;
        ++solstruct.used_iterations;
        solstruct.residual = rootvalues.norm();
      }
      if (solstruct.used_iterations == maximal_iterations) {
        solstruct.success = false;
      } else {
        solstruct.success = true;
      }

      return solstruct;
    };

  private:
    /// Holds an instance of the actual solver, to save computation time it
    /// is kept from previous time steps because usually the sparsity
    /// pattern will not change.
    Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>>
        sparselusolver;
    /// This must be revisited when including on the fly refinement of
    /// meshes.

    /// This will be the jacobian matrix.  We hold it here so its sparsity
    /// pattern is preserved.
    Eigen::SparseMatrix<double> jacobian;

    /// Tolerance under which equality is accepted.
    double tolerance;

    /// highest number of iterations after which to throw an exception
    int maximal_iterations;

    /// technical constant of the solve algorithm.
    constexpr static double const decrease_value{0.001};
    /// The minimal stepsize of a Newton step.
    constexpr static double const minimal_stepsize{1e-10};
  };
  typedef Newtonsolver_temp<Model::Problem> Newtonsolver;
  typedef Newtonsolver_temp<TestProblem> Newtonsolver_test;
} // namespace Solver
