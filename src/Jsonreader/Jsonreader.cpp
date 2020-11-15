#include <Edge.hpp>
#include <Exception.hpp>
#include <Jsonreader.hpp>
#include <Net.hpp>
#include <Node.hpp>
#include <PQnode.hpp>
#include <PVnode.hpp>
#include <Problem.hpp>
#include <Vphinode.hpp>
#include <fstream>
#include <iostream>
#include <memory>

namespace Jsonreader {

  using json = nlohmann::ordered_json;

  std::unique_ptr<Model::Problem>
  setup_problem(std::filesystem::path topology, std::filesystem::path initial,
                std::filesystem::path boundary) {

    json topologyjson;
    {
      std::ifstream jsonfilestream(topology);
      jsonfilestream >> topologyjson;
    }
    json initialjson;
    {
      std::ifstream jsonfilestream(initial);
      jsonfilestream >> initialjson;
    }
    json boundaryjson;
    {
      std::ifstream jsonfilestream(boundary);
      jsonfilestream >> boundaryjson;
    }

    std::map<std::string, std::map<double, Eigen::Vector2d>>
        power_boundary_map = get_power_boundaries(boundaryjson);

    try {
      Network::Net net = construct_network(topologyjson, power_boundary_map);
    } catch (Exception &e) {
      std::cout << e.what() << std::endl;
      throw e;
    }
    auto problem_ptr = std::unique_ptr<Model::Problem>(new Model::Problem);
    return problem_ptr;
  }

  std::map<std::string, std::map<double, Eigen::Vector2d>>
  get_power_boundaries(json boundaryjson) {
    std::map<std::string, std::map<double, Eigen::Vector2d>>
        powerboundaryvalues;
    for (auto &node : boundaryjson["boundarycondition"]) {
      if (node["type"] == "PQ" or node["type"] == "PV" or
          node["type"] == "Vphi") {
        std::map<double, Eigen::Vector2d> node_boundary;
        for (auto &datapoint : node["data"]) {
          Eigen::Vector2d value;
          value[0] = datapoint[1];
          value[1] = datapoint[2];
          node_boundary.insert({datapoint[0], value});
        }
        powerboundaryvalues.insert(
            {node["id"].get<std::string>(), node_boundary});
      } else {
        std::cout << "not implemented yet: boundary values for nodes of type "
                  << node["type"] << " node: " << node["id"] << '\n';
      }
    }
    return powerboundaryvalues;
  }

  Network::Net
  construct_network(json topologyjson,
                    std::map<std::string, std::map<double, Eigen::Vector2d>>
                        power_boundary_map) {
    std::vector<std::unique_ptr<Network::Node>> nodes;
    std::vector<std::unique_ptr<Network::Edge>> edges;
    for (auto &node : topologyjson["nodes"]["Vphi_nodes"]) {
      auto bd_iterator = power_boundary_map.find(node["id"]);
      if (bd_iterator == power_boundary_map.end()) {
        gthrow({"The node with ", node["id"],
                " has no boundary condition in the boundary value file."});
      } else {
        // strange as it seems: Here we actually construct a Vphi node:
        nodes.push_back(std::unique_ptr<Model::Networkproblem::Power::Vphinode>(
            new Model::Networkproblem::Power::Vphinode(
                node["id"], (*bd_iterator).second, node["G"], node["B"])));
      }
    }
    for (auto &node : topologyjson["nodes"]["PQ_nodes"]) {
      auto bd_iterator = power_boundary_map.find(node["id"]);
      if (bd_iterator == power_boundary_map.end()) {
        gthrow({"The node with ", node["id"],
                " has no boundary condition in the boundary value file."});
      } else {
        nodes.push_back(std::unique_ptr<Model::Networkproblem::Power::PQnode>(
            new Model::Networkproblem::Power::PQnode(
                node["id"], (*bd_iterator).second, node["G"], node["B"])));
      }
    }
    for (auto &node : topologyjson["nodes"]["PV_nodes"]) {
      auto bd_iterator = power_boundary_map.find(node["id"]);
      if (bd_iterator == power_boundary_map.end()) {
        gthrow({"The node with ", node["id"],
                " has no boundary condition in the boundary value file."});
      } else {
        nodes.push_back(std::unique_ptr<Model::Networkproblem::Power::PVnode>(
            new Model::Networkproblem::Power::PVnode(
                node["id"], (*bd_iterator).second, node["G"], node["B"])));
      }
    }

    for (auto &transmissionline :
         topologyjson["connections"]["transmissionLine"]) {
      // geht the pointer from the node vector and put it in the line!
    }

    Network::Net net(std::move(nodes), std::move(edges));
    return net;
  }

} // namespace Jsonreader