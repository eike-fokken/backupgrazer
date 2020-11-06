#include <Exception.hpp>
#include <Powernode.hpp>

#include <Boundaryvalue.hpp>

namespace Model::Networkproblem::Power {

class Vphinode:public Powernode {

public:

  /// In this node we just set V and phi to their respective boundary values.
  virtual void evaluate(Eigen::VectorXd & rootfunction, double current_time, double next_time,
                      const Eigen::VectorXd &last_state,
                      Eigen::VectorXd &current_state) override;

  virtual void evaluate_state_derivative(Eigen::SparseMatrix<double> & jacobian, double current_time, double next_time,
                                       const Eigen::VectorXd &,
                                       Eigen::VectorXd & current_state) override;

private:
  Boundaryvalue<Vphinode, 2> boundaryvalue;
};

} // namespace Model::Networkproblem::Power
