#pragma once

#ifndef BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#define BOOST_SPIRIT_NO_PREDEFINED_TERMINALS
#endif

#include "ast.hpp"
#include <boost/variant/apply_visitor.hpp>
#include <type_traits>
#include <vector>

namespace client {
///////////////////////////////////////////////////////////////////////////////
//  The annotation handler links the AST to a map of iterator positions
//  for the purpose of subsequent semantic error handling when the
//  program is being compiled.
///////////////////////////////////////////////////////////////////////////////
template <typename IT>
class annotation {
   private:
    class set_id {
       public:
        using result_type = void;

        explicit set_id(std::size_t id) : id_(id) {}

        template <typename T>
        result_type operator()(T& x) const {
            if constexpr (std::is_base_of_v<ast::tagged, T>) {
                x.id = id_;
            }
        }

       private:
        std::size_t id_;
    };

   public:
    template <typename, typename>
    struct result {
        using type = void;
    };

    explicit annotation(std::vector<IT>& iters) : iters_(iters) {}

    void operator()(ast::operand& ast, IT pos) const {
        auto id = iters_.size();
        iters_.push_back(pos);
        boost::apply_visitor(set_id(id), ast);
    }

    void operator()(ast::assignment& ast, IT pos) const {
        auto id = iters_.size();
        iters_.push_back(pos);
        ast.lhs.id = id;
    }

   private:
    std::vector<IT>& iters_;
};
}  // namespace client