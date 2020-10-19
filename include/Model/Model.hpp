#pragma once
#include <Eigen/Sparse>
#include <memory>
#include <vector>

namespace Model {

// forward declaration:
class Subproblem;

class Model {

public:
  // The constructor needs to declare Delta_t
  Model(double delta_t) : Delta_t(delta_t){};

  void add_subproblem(Subproblem subproblem);

  void reserve_indices();

  void evaluate(const Eigen::VectorXd &current_state,
                Eigen::VectorXd &new_state);

  void evaluate_state_derivative(const Eigen::VectorXd &,
                                 Eigen::SparseMatrix<double> &);

private:
  // collection of sub-problems
  std::vector<std::unique_ptr<Subproblem>> subproblems;

  // The time stepsize, which is immutable.
  const double Delta_t;
};

} // namespace Model