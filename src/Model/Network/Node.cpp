#include <Node.hpp>

#include <memory>
#include <stdexcept>

void Network::Node::remove_edge(std::shared_ptr<Network::Edge> to_remove) {
  bool edge_was_connected = false;
  for (auto it = starting_edges.begin(); it != starting_edges.end(); ++it) {
    if ((*it).lock() == to_remove) {
      edge_was_connected = true;
      starting_edges.erase(it);
    }
  }
  for (auto it = ending_edges.begin(); it != ending_edges.end(); ++it) {
    if ((*it).lock() == to_remove) {
      edge_was_connected = true;
      ending_edges.erase(it);
    }
  }

  if (!edge_was_connected) {
    throw std::invalid_argument("The edge is not attached to the node");
  }
}

int Network::Node::get_id() const { return id; }

std::vector<std::weak_ptr<Network::Edge>> Network::Node::get_starting_edges() const {
  return starting_edges;
}

std::vector<std::weak_ptr<Network::Edge>> Network::Node::get_ending_edges() const {
  return ending_edges;
}

void Network::Node::attach_starting_edge(
    std::shared_ptr<Network::Edge> to_attach) {
  // to be implemented, K:
  std::weak_ptr<Network::Edge> weak_to_attach = to_attach;
  starting_edges.push_back(weak_to_attach);
}

void Network::Node::attach_ending_edge(
    std::shared_ptr<Network::Edge> to_attach) {
  // to be implemented, K:
  std::weak_ptr<Network::Edge> weak_to_attach = to_attach;
  ending_edges.push_back(to_attach);
}

// K:
bool Network::Node::has_id(const int pos_id) const {

  if (pos_id == id) {
    return true;
  } else {
    return false;
  }
}