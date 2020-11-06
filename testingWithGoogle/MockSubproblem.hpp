#include "gmock/gmock.h"
#include <Eigen/Sparse>
#include <Subproblem.hpp>


namespace Model {

class MockSubproblem : public Subproblem {

public:
  MOCK_METHOD(void, evaluate,
              ((Eigen::VectorXd &),(double), (double), (const Eigen::VectorXd &),
               (Eigen::VectorXd &)),
              (override));
  MOCK_METHOD(void, evaluate_state_derivative,
              ((Eigen::SparseMatrix<double> &),(double), (double), (const Eigen::VectorXd & last_state),
               (Eigen::VectorXd & current_state)),
              (override));
  MOCK_METHOD(unsigned int, reserve_indices,
              (unsigned int const next_free_index), (override));
};
} // namespace Model
