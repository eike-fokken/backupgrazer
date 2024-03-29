#include <Eigen/Sparse>
#include <Equationcomponent.hpp>
#include <Exception.hpp>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

namespace Model::Networkproblem {

  // int Equationcomponent::get_number_of_printout_states() const {
  //   return get_number_of_states();
  // }

  int Equationcomponent::set_indices(int const next_free_index) {
    start_state_index = next_free_index;
    int number_of_states = get_number_of_states();

    after_state_index = next_free_index + number_of_states;
    // // This doesn't work, because the overall vector has number_of_time_steps
    // // many entries, this is for the inner vector... if (values.empty()) {
    //   values = std::vector<std::vector<std::map<double, double>>>(
    //       static_cast<unsigned int>(get_number_of_printout_states()));
    // }
    return after_state_index;
  }

  int Equationcomponent::get_start_state_index() const {
    return start_state_index;
  }
  int Equationcomponent::get_after_state_index() const {
    return after_state_index;
  }

  void Equationcomponent::print_indices() const {
    std::cout << "start: " << start_state_index
              << ", end: " << after_state_index << ", ";
  }

  void Equationcomponent::push_to_values(
      double t, std::vector<std::map<double, double>> value_vector) {
    times.push_back(t);
    values.push_back(value_vector);
  }

  std::vector<double> const &Equationcomponent::get_times() const {
    return times;
  }
  std::vector<std::vector<std::map<double, double>>> const &
  Equationcomponent::get_values() const {
    return values;
  }

} // namespace Model::Networkproblem
