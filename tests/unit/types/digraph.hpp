// Ossiaco JSON Converter Library
//
// Copyright (C) 2018 Ossiaco
//
// Licensed under the MIT license ("the license"); you may not use this file
// except in compliance with the license. The license can be found in the root
// directory of this project, or at
//
// http://opensource.org/licenses/MIT

#ifndef OSSIACO_CONVERTER_TESTS_UNIT_TYPES_DIGRAPH_HPP
#define OSSIACO_CONVERTER_TESTS_UNIT_TYPES_DIGRAPH_HPP

#include <ossiaco/converter/adapt.hpp>
#include <ossiaco/converter/allocate/simple.hpp>
#include <ossiaco/converter/core/char_types.hpp>
#include <ossiaco/converter/components/ranges.hpp>
#include <ossiaco/converter/components/smart_ptr.hpp>
#include <ossiaco/converter/utils/detect_specialization.hpp>
#include <ossiaco/converter/utils/string_convert.hpp>

#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/view/remove_if.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/to_container.hpp>

#include <memory>
#include <set>
#include <utility>
#include <vector>

namespace test_types {

// I hope nobody actually uses this as a graph structure, it would be terribly
// slow and is just chosen to illustrate conversion of shared/weak pointers!
class Digraph {
    using string_t = Ossiaco::converter::string_t;
public:
    using EdgePair = std::pair<string_t, string_t>;

    struct Node {
        OSSIACO_CONVERTER_FINAL_SUPPORTED(
            Node,
            (&Node::_ident,      OSSIACO_XPLATSTR("ident"))
            (&Node::_sinks, OSSIACO_XPLATSTR("sinks")));

        string_t _ident;

        std::vector<std::weak_ptr<Node>> _sinks;
    };

    Digraph() = default;
    Digraph(const Digraph&) = default;
    Digraph(Digraph&&) = default;

    Digraph& operator=(const Digraph&) = default;
    Digraph& operator=(Digraph&&) = default;

    ~Digraph() = default;

    Digraph(std::initializer_list<EdgePair> edges)
        : Digraph()
    {
        for (const EdgePair& edge : edges)
            addEdge(edge);
    }

    const auto& nodes() const { return _nodes; }

    void addEdge(EdgePair edge)
    {
        auto sourceNode = std::make_shared<Node>(Node{ edge.first, {} });
        auto sinkNode   = std::make_shared<Node>(Node{edge.second, {}});

        if (ranges::find_if(_nodes, [&sourceNode](const auto& nodePtr) { return nodePtr->_ident == sourceNode->_ident; }) == _nodes.end())
            _nodes.push_back(sourceNode);

        if (ranges::find_if(_nodes, [&sinkNode](const auto& nodePtr) { return nodePtr->_ident == sinkNode->_ident; }) == _nodes.end())
            _nodes.push_back(sinkNode);

        auto& sinks = sourceNode->_sinks;

        if (ranges::find_if(sinks, [&sinkNode](const auto& sinkPtr) { return (sinkPtr.lock())->_ident == sinkNode->_ident; }) == sinks.end())
            sinks.push_back(sinkNode);
    }

    std::set<EdgePair> edges() const
    {
        std::set<EdgePair> result;

        for (const auto& nodePtr : _nodes)
            for (const auto& nbrPtr : nodePtr->_sinks)
                result.emplace(nodePtr->_ident, (nbrPtr.lock())->_ident);

        return result;
    }

    OSSIACO_CONVERTER_FINAL_SUPPORTED(
        Digraph, (&Digraph::_nodes, OSSIACO_XPLATSTR("nodes")));

private:
    std::vector<std::shared_ptr<Node>> _nodes;
};

inline std::ostream& operator<<(std::ostream& os, const Digraph& g)
{
    for (const auto& nodePtr : g.nodes()) {
        os << "source: " << Ossiaco::converter::toNarrowString(nodePtr->_ident) << ", sinks: ";

        for (const auto& nbrPtr : nodePtr->_sinks)
            os << " " << Ossiaco::converter::toNarrowString((nbrPtr.lock())->_ident);

        os << "\n";
    }

    return os;
}

inline bool operator==(const Digraph::Node& lhs, const Digraph::Node& rhs)
{
    if (lhs._ident != rhs._ident)
        return false;

    using namespace ranges;

    static const auto transformVec = [](const auto& nbrs) {
        return nbrs
            | view::remove_if([](const auto& nodePtr) { return !(nodePtr.lock()); })
            | view::transform([](const auto& nodePtr) { return (nodePtr.lock())->_ident; })
            | to_vector;
    };

    return transformVec(lhs._sinks) == transformVec(rhs._sinks);
}

inline bool operator==(const Digraph& lhs, const Digraph& rhs)
{
    return lhs.edges() == rhs.edges();
}

} // namespace test_types

#endif // OSSIACO_CONVERTER_TESTS_UNIT_TYPES_DIGRAPH_HPP
