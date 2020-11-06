#pragma once
#include <Eigen/Sparse>
#include <memory>
#include <vector>

/*! \namespace Model
 *  This namespace holds all data for setting up model equations and for taking
 * derivatives thereof.
 */
namespace Model {

// forward declaration:
class Subproblem;

class Problem {

public:
  /// The constructor needs to declare Delta_t
  ///
  Problem(double delta_t) : Delta_t(delta_t){};

  void add_subproblem(std::unique_ptr<Subproblem> subproblem_ptr);

  unsigned int set_indices();

  void evaluate(Eigen::VectorXd & rootfunction, double last_time, double new_time, const Eigen::VectorXd &last_state,
                Eigen::VectorXd &new_state);

  void evaluate_state_derivative(Eigen::SparseMatrix<double> & jacobian, double last_time, double new_time,
                                 const Eigen::VectorXd &last_state,
                                 Eigen::VectorXd & new_state);

  /// As we have unique pointers, we can only give back a pointer to our
  /// subproblems.
  std::vector<Subproblem*> get_subproblems();

private:
  /// collection of sub-problems
  std::vector<std::unique_ptr<Subproblem>> subproblems;

protected:
  /// The time stepsize, which is immutable.
  const double Delta_t;
};

} // namespace Model
